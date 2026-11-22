#include "source/config/LzmaExtractor.hpp"
#include "source/logs/LogManager.hpp"

#include <QElapsedTimer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QCoreApplication>
#include <vector>

extern "C" {
#include "source/lzma_sdk/7zTypes.h"
#include "source/lzma_sdk/7zFile.h"
#include "source/lzma_sdk/7z.h"
#include "source/lzma_sdk/7zAlloc.h"
#include "source/lzma_sdk/7zCrc.h"
}

LzmaExtractor::LzmaExtractor(const QString &archivePath, QObject *parent)
    : QThread(parent),
      archive_path_(archivePath)
{
}

LzmaExtractor::~LzmaExtractor()
{
    request_cancel();
    wait();
}

void LzmaExtractor::request_cancel()
{
    cancel_requested_.storeRelaxed(true);
}

bool LzmaExtractor::is_canceled() const
{
    return cancel_requested_.loadRelaxed();
}

QString LzmaExtractor::find_7z_executable()
{
    const QStringList candidate_paths = {
        QStringLiteral("C:/Program Files/7-Zip/7z.exe"),
        QStringLiteral("C:/Program Files (x86)/7-Zip/7z.exe"),
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("7z.exe")),
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("7za.exe")),
    };

    for (const QString &candidate : candidate_paths) {
        if (QFile::exists(candidate)) {
            return candidate;
        }
    }

    const QString systemPath = QStandardPaths::findExecutable(QStringLiteral("7z.exe"));
    if (!systemPath.isEmpty()) {
        return systemPath;
    }

    return QStandardPaths::findExecutable(QStringLiteral("7za.exe"));
}

bool LzmaExtractor::is_7z_archive(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return filePath.endsWith(QStringLiteral(".7z"), Qt::CaseInsensitive);
    }

    const QByteArray header = file.read(6);
    file.close();

    // 7z signature: 37 7A BC AF 27 1C ('7', 'z', 0xBC, 0xAF, 0x27, 0x1C)
    if (header.size() >= 6) {
        const unsigned char *p = reinterpret_cast<const unsigned char*>(header.constData());
        if (p[0] == '7' && p[1] == 'z' && p[2] == 0xBC && p[3] == 0xAF && p[4] == 0x27 && p[5] == 0x1C) {
            return true;
        }
    }

    return filePath.endsWith(QStringLiteral(".7z"), Qt::CaseInsensitive);
}

void LzmaExtractor::on_match(uint32_t distance, uint32_t length, uint64_t offset, void *userData)
{
    if (userData) {
        static_cast<LzmaExtractor*>(userData)->handle_match(distance, length, offset);
    }
}

void LzmaExtractor::on_literal(uint8_t symbol, uint64_t offset, void *userData)
{
    if (userData) {
        static_cast<LzmaExtractor*>(userData)->handle_literal(symbol, offset);
    }
}

void LzmaExtractor::handle_match(uint32_t distance, uint32_t length, uint64_t offset)
{
    total_matches_++;
    uncompressed_size_ += length;

    batch_buffer_.append({distance, length, offset});

    // High performance batch flushing: avoid time syscall on every single match
    if (batch_buffer_.size() >= 5000) {
        emit matchesBatch(batch_buffer_);
        batch_buffer_.clear();
        last_batch_emit_ms_ = QDateTime::currentMSecsSinceEpoch();

        if (total_expected_uncompressed_bytes_ > 0) {
            int pct = qBound(0, static_cast<int>(uncompressed_size_ * 98 / total_expected_uncompressed_bytes_), 98);
            if (pct > last_emitted_progress_pct_) {
                last_emitted_progress_pct_ = pct;
                emit progress(pct, tr("Descomprimindo e analisando repetições LZMA (%1%)...").arg(pct));
            }
        }
    } else if ((total_matches_ & 1023) == 0) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (now - last_batch_emit_ms_ >= 40 && !batch_buffer_.isEmpty()) {
            emit matchesBatch(batch_buffer_);
            batch_buffer_.clear();
            last_batch_emit_ms_ = now;

            if (total_expected_uncompressed_bytes_ > 0) {
                int pct = qBound(0, static_cast<int>(uncompressed_size_ * 98 / total_expected_uncompressed_bytes_), 98);
                if (pct > last_emitted_progress_pct_) {
                    last_emitted_progress_pct_ = pct;
                    emit progress(pct, tr("Descomprimindo e analisando repetições LZMA (%1%)...").arg(pct));
                }
            }
        }
    }
}

void LzmaExtractor::handle_literal(uint8_t symbol, uint64_t offset)
{
    (void)symbol;
    (void)offset;
    total_literals_++;
    uncompressed_size_++;

    if ((total_literals_ & 65535) == 0 && total_expected_uncompressed_bytes_ > 0) {
        int pct = qBound(0, static_cast<int>(uncompressed_size_ * 98 / total_expected_uncompressed_bytes_), 98);
        if (pct > last_emitted_progress_pct_) {
            last_emitted_progress_pct_ = pct;
            emit progress(pct, tr("Descomprimindo e analisando repetições LZMA (%1%)...").arg(pct));
        }
    }
}

void LzmaExtractor::process_sliding_window_buffer(const QByteArray &data, quint64 streamOffset)
{
    if (data.isEmpty()) {
        return;
    }

    const int size = data.size();
    const unsigned char *bytes = reinterpret_cast<const unsigned char*>(data.constData());

    constexpr int WINDOW_SIZE = 32768;
    constexpr int MIN_MATCH = 3;
    constexpr int MAX_MATCH = 258;

    // Fast reusable hash table for 3-byte prefixes
    if (sw_head_.size() != 65536) {
        sw_head_.assign(65536, -1);
    } else {
        std::fill(sw_head_.begin(), sw_head_.end(), -1);
    }
    if (static_cast<int>(sw_prev_.size()) < size) {
        sw_prev_.assign(size, -1);
    } else {
        std::fill(sw_prev_.begin(), sw_prev_.begin() + size, -1);
    }

    int i = 0;
    while (i < size) {
        if (is_canceled()) {
            break;
        }

        if (i + MIN_MATCH > size) {
            handle_literal(bytes[i], streamOffset + static_cast<quint64>(i));
            i++;
            continue;
        }

        uint16_t hash = static_cast<uint16_t>(((bytes[i] << 8) ^ (bytes[i + 1] << 4) ^ bytes[i + 2]) & 0xFFFF);
        int match_pos = sw_head_[hash];
        sw_prev_[i] = match_pos;
        sw_head_[hash] = i;

        int best_len = 0;
        int best_dist = 0;

        int cur_match = match_pos;
        int chain_len = 0;
        while (cur_match != -1 && (i - cur_match) <= WINDOW_SIZE && chain_len < 32) {
            if (bytes[cur_match] == bytes[i] && bytes[cur_match + 1] == bytes[i + 1]) {
                int len = 0;
                while (len < MAX_MATCH && (i + len) < size && bytes[cur_match + len] == bytes[i + len]) {
                    len++;
                }
                if (len >= MIN_MATCH && len > best_len) {
                    best_len = len;
                    best_dist = i - cur_match;
                    if (len >= MAX_MATCH) {
                        break;
                    }
                }
            }
            cur_match = sw_prev_[cur_match];
            chain_len++;
        }

        if (best_len >= MIN_MATCH) {
            handle_match(static_cast<uint32_t>(best_dist),
                         static_cast<uint32_t>(best_len),
                         streamOffset + static_cast<quint64>(i));
            for (int k = 1; k < best_len && (i + k + MIN_MATCH) <= size; ++k) {
                uint16_t h = static_cast<uint16_t>(((bytes[i + k] << 8) ^ (bytes[i + k + 1] << 4) ^ bytes[i + k + 2]) & 0xFFFF);
                sw_prev_[i + k] = sw_head_[h];
                sw_head_[h] = i + k;
            }
            i += best_len;
        } else {
            handle_literal(bytes[i], streamOffset + static_cast<quint64>(i));
            i++;
        }
    }
}

bool LzmaExtractor::extract_7z_native(QElapsedTimer &timer)
{
    auto *logger = source::logs::LogManager::get_instance();
    logger->info(QStringLiteral("Executando motor nativo LZMA C SDK com hooks para: %1").arg(archive_path_));

    CrcGenerateTable();

    CFileInStream archiveStream;
    CLookToRead2 lookStream;
    CSzArEx db;
    ISzAlloc allocImp = { SzAlloc, SzFree };
    ISzAlloc allocTempImp = { SzAllocTemp, SzFreeTemp };

    FileInStream_CreateVTable(&archiveStream);
    File_Construct(&archiveStream.file);

    std::wstring nativePath = QDir::toNativeSeparators(archive_path_).toStdWString();
    WRes wres = InFile_OpenW(&archiveStream.file, nativePath.c_str());
    if (wres != 0) {
        logger->error(QStringLiteral("Falha ao abrir arquivo .7z (erro %1): %2").arg(wres).arg(archive_path_));
        emit extractionFinished(false, tr("Não foi possível abrir o arquivo (código %1)").arg(wres), 0, 0, 0, timer.elapsed());
        return false;
    }

    LookToRead2_CreateVTable(&lookStream, False);
    LookToRead2_INIT(&lookStream);
    lookStream.buf = (Byte*)malloc(1 << 18);
    lookStream.bufSize = 1 << 18;
    lookStream.realStream = &archiveStream.vt;

    SzArEx_Init(&db);
    SRes res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
    if (res != SZ_OK) {
        logger->error(QStringLiteral("Falha ao analisar cabeçalho 7z (erro %1): %2").arg(res).arg(archive_path_));
        free(lookStream.buf);
        File_Close(&archiveStream.file);
        emit extractionFinished(false, tr("Erro ao ler cabeçalho do arquivo (código %1)").arg(res), 0, 0, 0, timer.elapsed());
        return false;
    }

    logger->info(QStringLiteral("Arquivo .7z aberto com sucesso. Arquivos: %1, Pastas de compressão: %2")
                     .arg(db.NumFiles).arg(db.db.NumFolders));

    total_expected_uncompressed_bytes_ = 0;
    for (UInt32 i = 0; i < db.NumFiles; i++) {
        if (!SzArEx_IsDir(&db, i)) {
            total_expected_uncompressed_bytes_ += SzArEx_GetFileSize(&db, i);
        }
    }

    SzSetDecodeHooks(&LzmaExtractor::on_match, &LzmaExtractor::on_literal, this);

    UInt32 blockIndex = 0xFFFFFFFF;
    Byte *outBuffer = nullptr;
    size_t outBufferSize = 0;
    bool success = true;
    QString errorMsg;

    for (UInt32 i = 0; i < db.NumFiles; i++) {
        if (is_canceled()) {
            logger->warning(QStringLiteral("Extração cancelada pelo usuário."));
            success = false;
            errorMsg = tr("Extração cancelada pelo usuário.");
            break;
        }

        size_t offset = 0;
        size_t outSizeProcessed = 0;
        res = SzArEx_Extract(&db, &lookStream.vt, i,
                             &blockIndex, &outBuffer, &outBufferSize,
                             &offset, &outSizeProcessed,
                             &allocImp, &allocTempImp);
        if (res != SZ_OK) {
            logger->error(QStringLiteral("Erro ao extrair arquivo %1 (código %2)").arg(i).arg(res));
            success = false;
            errorMsg = tr("Erro ao extrair item %1 (código %2)").arg(i).arg(res);
            break;
        }

        if (total_expected_uncompressed_bytes_ > 0) {
            int pct = qBound(0, static_cast<int>(uncompressed_size_ * 98 / total_expected_uncompressed_bytes_), 98);
            if (pct > last_emitted_progress_pct_) {
                last_emitted_progress_pct_ = pct;
                emit progress(pct, tr("Extraindo e analisando arquivo %1 de %2 (%3%)...").arg(i + 1).arg(db.NumFiles).arg(pct));
            }
        } else if (db.NumFiles > 0) {
            int pct = qBound(0, static_cast<int>((i + 1) * 98 / db.NumFiles), 98);
            if (pct > last_emitted_progress_pct_) {
                last_emitted_progress_pct_ = pct;
                emit progress(pct, tr("Extraindo e analisando arquivo %1 de %2 (%3%)...").arg(i + 1).arg(db.NumFiles).arg(pct));
            }
        }
    }

    if (!batch_buffer_.isEmpty()) {
        emit matchesBatch(batch_buffer_);
        batch_buffer_.clear();
    }

    if (outBuffer) {
        ISzAlloc_Free(&allocImp, outBuffer);
    }

    SzSetDecodeHooks(nullptr, nullptr, nullptr);
    SzArEx_Free(&db, &allocImp);
    free(lookStream.buf);
    File_Close(&archiveStream.file);

    qint64 elapsed = timer.elapsed();
    if (success) {
        emit progress(100, tr("Concluído!"));
        logger->info(QStringLiteral("Extração nativa .7z concluída em %1ms. Matches: %2, Literais: %3, Bytes: %4")
                         .arg(elapsed).arg(total_matches_).arg(total_literals_).arg(uncompressed_size_));
        emit extractionFinished(true, tr("Extração e análise de repetições concluída com sucesso!"),
                                total_matches_, total_literals_, uncompressed_size_, elapsed);
        return true;
    }

    emit extractionFinished(false, errorMsg.isEmpty() ? tr("Falha na extração") : errorMsg,
                            total_matches_, total_literals_, uncompressed_size_, elapsed);
    return false;
}

quint64 LzmaExtractor::query_uncompressed_size(const QString &sevenZipExe, const QString &archivePath)
{
    if (sevenZipExe.isEmpty() || !QFile::exists(archivePath)) {
        return 0;
    }

    QProcess process;
    process.start(sevenZipExe, {QStringLiteral("l"), QStringLiteral("-slt"), archivePath});
    if (!process.waitForFinished(5000)) {
        process.kill();
        return 0;
    }

    quint64 totalSize = 0;
    const QString output = QString::fromUtf8(process.readAllStandardOutput());
    const QStringList lines = output.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith(QLatin1String("Size = "))) {
            bool ok = false;
            quint64 sz = trimmed.mid(7).trimmed().toULongLong(&ok);
            if (ok) {
                totalSize += sz;
            }
        }
    }
    return totalSize;
}

bool LzmaExtractor::extract_generic_archive(QElapsedTimer &timer)
{
    auto *logger = source::logs::LogManager::get_instance();
    const QString sevenZipExe = find_7z_executable();

    if (!sevenZipExe.isEmpty()) {
        logger->info(QStringLiteral("Processando arquivo multi-formato (%1) via backend 7-Zip: %2")
                         .arg(QFileInfo(archive_path_).suffix().toUpper())
                         .arg(sevenZipExe));

        total_expected_uncompressed_bytes_ = query_uncompressed_size(sevenZipExe, archive_path_);
        if (total_expected_uncompressed_bytes_ == 0) {
            const qint64 compSize = QFileInfo(archive_path_).size();
            total_expected_uncompressed_bytes_ = compSize > 0 ? static_cast<quint64>(compSize) * 2 : 1024 * 1024;
        }

        QProcess process;
        QStringList arguments;
        arguments << QStringLiteral("e") << archive_path_ << QStringLiteral("-so") << QStringLiteral("-y") << QStringLiteral("-bso0") << QStringLiteral("-bsp0");

        process.start(sevenZipExe, arguments);
        if (!process.waitForStarted(5000)) {
            logger->warning(QStringLiteral("Não foi possível iniciar 7z.exe. Executando análise deslizante direta."));
        } else {
            quint64 streamOffset = 0;
            emit progress(1, tr("Iniciando extração e descompressão do fluxo (%1)...").arg(QFileInfo(archive_path_).suffix().toUpper()));

            while (process.state() == QProcess::Running || process.bytesAvailable() > 0) {
                if (is_canceled()) {
                    process.kill();
                    emit extractionFinished(false, tr("Extração cancelada pelo usuário."), total_matches_, total_literals_, uncompressed_size_, timer.elapsed());
                    return false;
                }

                if (process.waitForReadyRead(100) || process.bytesAvailable() > 0) {
                    const QByteArray chunk = process.read(131072);
                    if (!chunk.isEmpty()) {
                        process_sliding_window_buffer(chunk, streamOffset);
                        streamOffset += chunk.size();

                        if (total_expected_uncompressed_bytes_ > 0) {
                            int pct = qBound(0, static_cast<int>(streamOffset * 98 / total_expected_uncompressed_bytes_), 98);
                            if (pct > last_emitted_progress_pct_) {
                                last_emitted_progress_pct_ = pct;
                                emit progress(pct, tr("Analisando fluxo LZ77 (%1 KB descompactados - %2%)...").arg(streamOffset / 1024).arg(pct));
                            }
                        }
                    }
                }
            }

            process.waitForFinished(10000);

            if (!batch_buffer_.isEmpty()) {
                emit matchesBatch(batch_buffer_);
                batch_buffer_.clear();
            }

            if (streamOffset > 0) {
                emit progress(100, tr("Concluído!"));
                emit extractionFinished(true, tr("Extração e análise de repetições concluída com sucesso!"),
                                        total_matches_, total_literals_, uncompressed_size_, timer.elapsed());
                return true;
            }
        }
    }

    // Fallback: direct streaming analysis
    logger->info(QStringLiteral("Executando análise deslizante direta sobre o arquivo: %1").arg(archive_path_));
    QFile file(archive_path_);
    if (!file.open(QIODevice::ReadOnly)) {
        emit extractionFinished(false, tr("Não foi possível abrir o arquivo para leitura."), 0, 0, 0, timer.elapsed());
        return false;
    }

    quint64 streamOffset = 0;
    const qint64 fileSize = file.size();

    while (!file.atEnd()) {
        if (is_canceled()) {
            file.close();
            emit extractionFinished(false, tr("Extração cancelada pelo usuário."), total_matches_, total_literals_, uncompressed_size_, timer.elapsed());
            return false;
        }

        const QByteArray chunk = file.read(131072);
        if (!chunk.isEmpty()) {
            process_sliding_window_buffer(chunk, streamOffset);
            streamOffset += chunk.size();
            int pct = fileSize > 0 ? static_cast<int>(streamOffset * 98 / fileSize) : 50;
            pct = qBound(0, pct, 98);
            if (pct > last_emitted_progress_pct_) {
                last_emitted_progress_pct_ = pct;
                emit progress(pct, tr("Analisando repetições LZ77 no arquivo (%1%)...").arg(pct));
            }
        }
    }

    file.close();

    if (!batch_buffer_.isEmpty()) {
        emit matchesBatch(batch_buffer_);
        batch_buffer_.clear();
    }

    emit progress(100, tr("Concluído!"));
    emit extractionFinished(true, tr("Extração e análise de repetições concluída com sucesso!"),
                            total_matches_, total_literals_, uncompressed_size_, timer.elapsed());
    return true;
}

void LzmaExtractor::run()
{
    auto *logger = source::logs::LogManager::get_instance();
    logger->info(QStringLiteral("Iniciando processamento e desconstrução LZ77 do arquivo: %1").arg(archive_path_));

    QElapsedTimer timer;
    timer.start();

    total_matches_ = 0;
    total_literals_ = 0;
    uncompressed_size_ = 0;
    total_expected_uncompressed_bytes_ = 0;
    last_emitted_progress_pct_ = 0;
    batch_buffer_.clear();
    last_batch_emit_ms_ = QDateTime::currentMSecsSinceEpoch();

    emit progress(0, tr("Iniciando análise de fluxo LZ77 / LZMA..."));

    if (is_7z_archive(archive_path_)) {
        extract_7z_native(timer);
    } else {
        extract_generic_archive(timer);
    }
}
