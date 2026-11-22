#pragma once

#include <QThread>
#include <QString>
#include <QVector>
#include <QAtomicInteger>
#include <cstdint>

class QIODevice;
class QElapsedTimer;

struct LzmaMatchInfo {
    quint32 distance = 0;
    quint32 length = 0;
    quint64 offset = 0;
};

class LzmaExtractor : public QThread {
    Q_OBJECT

public:
    explicit LzmaExtractor(const QString &archivePath, QObject *parent = nullptr);
    ~LzmaExtractor() override;

    void request_cancel();
    bool is_canceled() const;

    static QString find_7z_executable();
    static bool is_7z_archive(const QString &filePath);

signals:
    void progress(int percentage, const QString &statusText);
    void matchesBatch(const QVector<LzmaMatchInfo> &batch);
    void extractionFinished(bool success, const QString &message,
                            quint64 totalMatches, quint64 totalLiterals,
                            quint64 uncompressedSize, qint64 elapsedMs);
    void logMessage(const QString &msg);

protected:
    void run() override;

private:
    static void on_match(uint32_t distance, uint32_t length, uint64_t offset, void *userData);
    static void on_literal(uint8_t symbol, uint64_t offset, void *userData);

    void handle_match(uint32_t distance, uint32_t length, uint64_t offset);
    void handle_literal(uint8_t symbol, uint64_t offset);

    bool extract_7z_native(QElapsedTimer &timer);
    bool extract_generic_archive(QElapsedTimer &timer);
    void process_sliding_window_buffer(const QByteArray &data, quint64 streamOffset);
    static quint64 query_uncompressed_size(const QString &sevenZipExe, const QString &archivePath);

    QString archive_path_;
    QAtomicInteger<bool> cancel_requested_{false};

    quint64 total_matches_{0};
    quint64 total_literals_{0};
    quint64 uncompressed_size_{0};
    quint64 total_expected_uncompressed_bytes_{0};
    int last_emitted_progress_pct_{0};

    QVector<LzmaMatchInfo> batch_buffer_;
    qint64 last_batch_emit_ms_{0};

    // Reusable buffers for sliding window search
    std::vector<int> sw_head_;
    std::vector<int> sw_prev_;
};
