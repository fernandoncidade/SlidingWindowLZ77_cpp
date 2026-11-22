#include "find_VersionEditor.hpp"

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QtGlobal>

#include <optional>

namespace mocks::version_editor {
namespace {

struct FileSpec {
    QString key;
    QString relative_path;
    QString content_key;
    QString group;
};

struct FileStatus {
    QString key;
    QString group;
    QString relative_path;
    QString path;
    bool exists = false;
    QStringList found;
    QStringList missing;
};

struct UpdateResult {
    QString key;
    QString group;
    QString relative_path;
    bool ok = false;
    QString message;
};

struct UpdateContext {
    QString pt_version;
    QString pt_date;
    QString en_version;
    QString en_date;
    QString pt_readme_version;
    QString en_readme_version;
};

const QStringList kPtMonths = {
    QStringLiteral("Janeiro"),
    QStringLiteral("Fevereiro"),
    QStringLiteral("Março"),
    QStringLiteral("Abril"),
    QStringLiteral("Maio"),
    QStringLiteral("Junho"),
    QStringLiteral("Julho"),
    QStringLiteral("Agosto"),
    QStringLiteral("Setembro"),
    QStringLiteral("Outubro"),
    QStringLiteral("Novembro"),
    QStringLiteral("Dezembro"),
};

const QStringList kPtMonthsLower = {
    QStringLiteral("janeiro"),
    QStringLiteral("fevereiro"),
    QStringLiteral("março"),
    QStringLiteral("abril"),
    QStringLiteral("maio"),
    QStringLiteral("junho"),
    QStringLiteral("julho"),
    QStringLiteral("agosto"),
    QStringLiteral("setembro"),
    QStringLiteral("outubro"),
    QStringLiteral("novembro"),
    QStringLiteral("dezembro"),
};

const QStringList kEnMonths = {
    QStringLiteral("January"),
    QStringLiteral("February"),
    QStringLiteral("March"),
    QStringLiteral("April"),
    QStringLiteral("May"),
    QStringLiteral("June"),
    QStringLiteral("July"),
    QStringLiteral("August"),
    QStringLiteral("September"),
    QStringLiteral("October"),
    QStringLiteral("November"),
    QStringLiteral("December"),
};

const QString &group_docs_root()
{
    static const QString value = QStringLiteral("Documentação Raiz");
    return value;
}

const QString &group_docs_internal()
{
    static const QString value = QStringLiteral("Documentação Interna (source/docs)");
    return value;
}

const QString &group_installers()
{
    static const QString value = QStringLiteral("Instaladores Inno Setup (library)");
    return value;
}

const QString &group_library()
{
    static const QString value = QStringLiteral("Biblioteca / Notas (library)");
    return value;
}

const QStringList &group_order()
{
    static const QStringList order = {
        group_docs_root(),
        group_docs_internal(),
        group_installers(),
        group_library(),
    };
    return order;
}

QString normalize_dir(const QString &path)
{
    if (path.trimmed().isEmpty()) {
        return QString();
    }

    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

QString detect_project_root()
{
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString current_dir = QDir::currentPath();
    const QString source_file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();

    QStringList seeds = {app_dir, current_dir, source_file_dir};
    QStringList checked;

    auto is_project_root = [](const QString &candidate) {
        const QDir dir(candidate);
        return dir.exists(QStringLiteral("mocks")) && dir.exists(QStringLiteral("source"))
               && QFileInfo(dir.filePath(QStringLiteral("README.md"))).exists();
    };

    for (const QString &seed : seeds) {
        QDir dir(seed);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = normalize_dir(dir.absolutePath());
            if (!checked.contains(candidate)) {
                checked.append(candidate);
                if (is_project_root(candidate)) {
                    return candidate;
                }
            }

            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return normalize_dir(QDir(source_file_dir).filePath(QStringLiteral("..")));
}

QString build_version_from_date(const QDate &date = QDate::currentDate())
{
    const QDate safe_date = date.isValid() ? date : QDate::currentDate();
    if (!safe_date.isValid()) {
        return QStringLiteral("2026.8.20.0");
    }

    return QStringLiteral("%1.%2.%3.0")
        .arg(safe_date.year())
        .arg(safe_date.month())
        .arg(safe_date.day());
}

QString readme_version_token(const QString &version)
{
    const QString trimmed = version.trimmed();
    if (trimmed.isEmpty()) {
        return QStringLiteral("v0.0.0.0");
    }

    return trimmed.startsWith(QLatin1Char('v'), Qt::CaseInsensitive) ? trimmed : QStringLiteral("v%1").arg(trimmed);
}

QString spec_content_key(const FileSpec &spec)
{
    return spec.content_key.isEmpty() ? spec.key : spec.content_key;
}

const QList<FileSpec> &file_specs()
{
    static const QList<FileSpec> specs = [] {
        QList<FileSpec> data;

        auto make_key = [](const QString &relative_path) {
            QString key;
            bool last_was_separator = false;
            for (const QChar ch : relative_path.toCaseFolded()) {
                if (ch.isLetterOrNumber()) {
                    key.append(ch);
                    last_was_separator = false;
                } else if (!last_was_separator) {
                    key.append(QLatin1Char('_'));
                    last_was_separator = true;
                }
            }

            while (key.endsWith(QLatin1Char('_'))) {
                key.chop(1);
            }
            return key;
        };

        auto append = [&](const QString &group,
                          const QString &relative_path,
                          const QString &content_key) {
            data.append({make_key(relative_path), relative_path, content_key, group});
        };

        // 1. Documentação Raiz (Multilíngue Unificada)
        append(group_docs_root(), QStringLiteral("ABOUT.md"), QStringLiteral("doc_multilang"));
        append(group_docs_root(), QStringLiteral("MANUAL.md"), QStringLiteral("doc_multilang"));
        append(group_docs_root(), QStringLiteral("README.md"), QStringLiteral("doc_multilang"));

        // 2. Documentação Interna (source/docs)
        append(group_docs_internal(), QStringLiteral("source/docs/en_US/ABOUT.md"), QStringLiteral("doc_en"));
        append(group_docs_internal(), QStringLiteral("source/docs/en_US/MANUAL.md"), QStringLiteral("doc_en"));
        append(group_docs_internal(), QStringLiteral("source/docs/en_US/README.md"), QStringLiteral("doc_en"));
        append(group_docs_internal(), QStringLiteral("source/docs/pt_BR/LEIAME.md"), QStringLiteral("doc_pt"));
        append(group_docs_internal(), QStringLiteral("source/docs/pt_BR/MANUAL.md"), QStringLiteral("doc_pt"));
        append(group_docs_internal(), QStringLiteral("source/docs/pt_BR/SOBRE.md"), QStringLiteral("doc_pt"));

        // 3. Instaladores Inno Setup (library)
        append(group_installers(), QStringLiteral("library/SlidingWindowLZ77.iss"), QStringLiteral("installer_unified"));
        append(group_installers(), QStringLiteral("library/SlidingWindowLZ77_MinGW.iss"), QStringLiteral("installer_mingw"));
        append(group_installers(), QStringLiteral("library/SlidingWindowLZ77_MSVC.iss"), QStringLiteral("installer_msvc"));

        // 4. Biblioteca / Notas (library)
        append(group_library(), QStringLiteral("library/SlidingWindowLZ77_cpp.txt"), QStringLiteral("library_notes"));

        return data;
    }();

    return specs;
}

const QMap<QString, QStringList> &patterns()
{
    static const QMap<QString, QStringList> data = {
        {QStringLiteral("doc_multilang"),
         {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*v?[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Data t(?:é|e)cnica desta revis(?:ã|a)o:\s*.*<br>)\s*$)"),
          QStringLiteral(R"(^(Version:\s*v?[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Technical revision date:\s*.*<br>)\s*$)"),
          QStringLiteral(R"(^(Author:\s*.*<br>?)\s*$)")}},
        {QStringLiteral("doc_pt"),
         {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*v?[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Data t(?:é|e)cnica desta revis(?:ã|a)o:\s*.*<br>)\s*$)"),
          QStringLiteral(R"(^(Autor:\s*.*<br>?)\s*$)")}},
        {QStringLiteral("doc_en"),
         {QStringLiteral(R"(^(Version:\s*v?[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Technical revision date:\s*.*<br>)\s*$)"),
          QStringLiteral(R"(^(Author:\s*.*<br>?)\s*$)")}},
        {QStringLiteral("installer_unified"),
         {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)")}},
        {QStringLiteral("installer_mingw"),
         {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)"),
          QStringLiteral(R"(^(OutputBaseFilename=SlidingWindowLZ77_mingw_v[\d\.]+)\r?$)")}},
        {QStringLiteral("installer_msvc"),
         {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)"),
          QStringLiteral(R"(^(OutputBaseFilename=SlidingWindowLZ77_msvc_v[\d\.]+)\r?$)")}},
        {QStringLiteral("library_notes"),
         {QStringLiteral(R"(^(NOTAS DE LAN(?:Ç|C)AMENTO:\s*Vers(?:ã|a)o\s+v?[\d\.]+<br>)\r?$)"),
          QStringLiteral(R"(^(Data:\s*.*<br>)\r?$)"),
          QStringLiteral(R"(^(RELEASE NOTES:\s*Version\s+v?[\d\.]+<br>)\r?$)"),
          QStringLiteral(R"(^(Date:\s*.*<br>)\r?$)")}},
    };

    return data;
}

QString resolve_file_path(const QString &base_dir, const QString &relative_path)
{
    const QDir dir(base_dir);
    return QFileInfo(dir.filePath(relative_path)).absoluteFilePath();
}

std::optional<QString> read_file_text(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QByteArray data = file.readAll();
    if (data.startsWith("\xEF\xBB\xBF")) {
        data = data.mid(3);
    }

    QString utf8 = QString::fromUtf8(data);
    if (!utf8.contains(QChar::ReplacementCharacter)) {
        return utf8;
    }

    QString latin1 = QString::fromLatin1(data);
    return latin1;
}

bool write_file_text(const QString &path, const QString &text, QString *error_message)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    const qint64 written = file.write(text.toUtf8());
    if (written < 0) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    return true;
}

QString detect_line_ending(const QString &text)
{
    return text.contains(QStringLiteral("\r\n")) ? QStringLiteral("\r\n") : QStringLiteral("\n");
}

QStringList split_lines_preserving_empty(const QString &text)
{
    return text.split(QRegularExpression(QStringLiteral(R"(\r\n|\n|\r)")), Qt::KeepEmptyParts);
}

QString join_lines(const QStringList &lines, const QString &separator)
{
    return lines.join(separator);
}

QStringList first_lines(const QString &text, int limit)
{
    QStringList lines = split_lines_preserving_empty(text);
    if (lines.size() > limit) {
        lines = lines.mid(0, limit);
    }
    return lines;
}

QList<FileStatus> check_expected_lines(const QString &base_dir)
{
    QList<FileStatus> statuses;

    for (const FileSpec &spec : file_specs()) {
        FileStatus status;
        status.key = spec.key;
        status.group = spec.group.isEmpty() ? group_docs_root() : spec.group;
        status.relative_path = spec.relative_path;
        status.path = resolve_file_path(base_dir, spec.relative_path);

        const std::optional<QString> text = read_file_text(status.path);
        if (!text.has_value()) {
            status.exists = false;
            statuses.append(status);
            continue;
        }

        status.exists = true;
        const QStringList pattern_list = patterns().value(spec_content_key(spec));
        if (pattern_list.isEmpty()) {
            status.found = first_lines(text.value(), 3);
            statuses.append(status);
            continue;
        }

        for (const QString &pattern : pattern_list) {
            const QRegularExpression rx(pattern, QRegularExpression::MultilineOption);
            const QRegularExpressionMatch match = rx.match(text.value());
            if (match.hasMatch()) {
                status.found.append(match.captured(1).trimmed());
            } else {
                status.missing.append(pattern);
            }
        }

        statuses.append(status);
    }

    return statuses;
}

bool apply_line_updates(const QString &key, QStringList *lines, const UpdateContext &context)
{
    if (lines == nullptr) {
        return false;
    }

    bool changed = false;
    for (QString &line : *lines) {
        const QString original = line;

        if (key == QStringLiteral("doc_pt") || key == QStringLiteral("doc_multilang")) {
            if (line.startsWith(QStringLiteral("NOTAS DE LANÇAMENTO: Versão"))) {
                line = QStringLiteral("NOTAS DE LANÇAMENTO: Versão %1<br>").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1<br>").arg(context.pt_readme_version);
            } else if (line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versao: %1<br>").arg(context.pt_readme_version);
            } else if (line.startsWith(QStringLiteral("Data técnica desta revisão:"))) {
                line = QStringLiteral("Data técnica desta revisão: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Data tecnica desta revisao:"))) {
                line = QStringLiteral("Data tecnica desta revisao: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Autor:"))) {
                line = QStringLiteral("Autor: Fernando Nillsson Cidade<br>");
            }
        }
        if (key == QStringLiteral("doc_en") || key == QStringLiteral("doc_multilang")) {
            if (line.startsWith(QStringLiteral("RELEASE NOTES: Version"))) {
                line = QStringLiteral("RELEASE NOTES: Version %1<br>").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1<br>").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1<br>").arg(context.en_readme_version);
            } else if (line.startsWith(QStringLiteral("Technical revision date:"))) {
                line = QStringLiteral("Technical revision date: %1<br>").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Author:"))) {
                line = QStringLiteral("Author: Fernando Nillsson Cidade<br>");
            }
        } else if (key == QStringLiteral("installer_unified") || key == QStringLiteral("installer_mingw")
                   || key == QStringLiteral("installer_msvc")) {
            if (line.startsWith(QStringLiteral("#define MyAppVersion"))) {
                const QRegularExpression rx(QStringLiteral(R"((#define\s+MyAppVersion\s+")[\d\.]+(";?))"));
                const QRegularExpressionMatch match = rx.match(line);
                if (match.hasMatch()) {
                    line = match.captured(1) + context.pt_version + match.captured(2);
                }
            } else if (key == QStringLiteral("installer_mingw")
                       && line.startsWith(QStringLiteral("OutputBaseFilename=SlidingWindowLZ77_mingw_v"))) {
                line = QStringLiteral("OutputBaseFilename=SlidingWindowLZ77_mingw_v%1").arg(context.pt_version);
            } else if (key == QStringLiteral("installer_msvc")
                       && line.trimmed().startsWith(QStringLiteral("OutputBaseFilename=SlidingWindowLZ77_msvc_v"))) {
                const QString indentation = line.left(line.size() - line.trimmed().size());
                line = QStringLiteral("%1OutputBaseFilename=SlidingWindowLZ77_msvc_v%2")
                           .arg(indentation, context.pt_version);
            }
        } else if (key == QStringLiteral("library_notes")) {
            if (line.startsWith(QStringLiteral("NOTAS DE LANÇAMENTO: Versão"))
                || line.startsWith(QStringLiteral("NOTAS DE LANCAMENTO: Versao"))) {
                line = QStringLiteral("NOTAS DE LANÇAMENTO: Versão %1<br>").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("RELEASE NOTES: Version"))) {
                line = QStringLiteral("RELEASE NOTES: Version %1<br>").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1<br>").arg(context.en_date);
            }
        }

        if (line != original) {
            changed = true;
        }
    }

    return changed;
}

QList<UpdateResult> apply_updates(const UpdateContext &context, const QString &base_dir)
{
    QList<UpdateResult> results;

    for (const FileSpec &spec : file_specs()) {
        UpdateResult result;
        result.key = spec.key;
        result.group = spec.group.isEmpty() ? group_docs_root() : spec.group;
        result.relative_path = spec.relative_path;

        const QString path = resolve_file_path(base_dir, spec.relative_path);
        const std::optional<QString> text = read_file_text(path);
        if (!text.has_value()) {
            result.ok = false;
            result.message = QStringLiteral("arquivo inexistente");
            results.append(result);
            continue;
        }

        const QString line_ending = detect_line_ending(text.value());
        QStringList lines = split_lines_preserving_empty(text.value());
        const bool changed = apply_line_updates(spec_content_key(spec), &lines, context);

        if (!changed) {
            result.ok = false;
            result.message = QStringLiteral("padrões não encontrados / sem alteração");
            results.append(result);
            continue;
        }

        QString error_message;
        if (write_file_text(path, join_lines(lines, line_ending), &error_message)) {
            result.ok = true;
            result.message = QStringLiteral("atualizado");
        } else {
            result.ok = false;
            result.message = QStringLiteral("erro escrita: %1").arg(error_message);
        }

        results.append(result);
    }

    return results;
}

QString mock_windows_icon_name()
{
#if defined(MOCKS_WINDOWS_ICON_NAME)
    return QStringLiteral(MOCKS_WINDOWS_ICON_NAME);
#else
    return QStringLiteral("lz77.ico");
#endif
}

QString find_icon_path(const QString &project_root, const QString &preferred_name = QString())
{
    const QString primary_icon_name = preferred_name.trimmed().isEmpty() ? mock_windows_icon_name() : preferred_name.trimmed();
    const QStringList candidate_names = {
        primary_icon_name,
        QStringLiteral("lz77.ico"),
        QStringLiteral("lz77.png")
    };

    const QDir root(project_root);
    const QDir app_dir(QCoreApplication::applicationDirPath());
    const QDir current_dir(QDir::currentPath());
    const QDir source_dir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());

    for (const QString &icon_name : candidate_names) {
        if (icon_name.isEmpty()) {
            continue;
        }
        QStringList candidates = {
            app_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("mocks/icones/%1").arg(icon_name)),
        };

        if (!project_root.isEmpty()) {
            candidates.append(root.filePath(QStringLiteral("mocks/icones/%1").arg(icon_name)));
            candidates.append(root.filePath(QStringLiteral("source/assets/icons/%1").arg(icon_name)));
            candidates.append(root.filePath(QStringLiteral("icones/%1").arg(icon_name)));
        }

        candidates.append(source_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)));
        candidates.append(source_dir.filePath(QStringLiteral("../source/assets/icons/%1").arg(icon_name)));

        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
        }
    }

    return QString();
}

}  // namespace

VersionEditorWidget::VersionEditorWidget(QWidget *parent) : QWidget(parent), selected_base_dir_(detect_project_root())
{
    setWindowTitle(QStringLiteral("Editor de Versões / Datas - LZ77 (PT/EN)"));
    resize(980, 640);

    const QDate today = QDate::currentDate();
    const QString default_version = build_version_from_date(today);
    const int default_day = today.isValid() ? today.day() : 20;
    const int default_month_index = today.isValid() ? qMax(0, today.month() - 1) : 7;
    const int default_year = today.isValid() ? today.year() : 2026;

    const QString icon_path = find_icon_path(selected_base_dir_, QStringLiteral("lz77.ico"));
    if (!icon_path.isEmpty()) {
        const QIcon icon(icon_path);
        QApplication::setWindowIcon(icon);
        setWindowIcon(icon);
    }

    auto *layout = new QVBoxLayout(this);

    auto *folder_layout = new QHBoxLayout();
    folder_layout->addWidget(new QLabel(QStringLiteral("Pasta dos arquivos:"), this));

    folder_path_ = new QLineEdit(selected_base_dir_, this);
    folder_path_->setReadOnly(true);
    folder_layout->addWidget(folder_path_);

    select_folder_btn_ = new QPushButton(QStringLiteral("Selecionar..."), this);
    folder_layout->addWidget(select_folder_btn_);
    layout->addLayout(folder_layout);

    auto *pt_group = new QGroupBox(QStringLiteral("Português (pt-BR)"), this);
    auto *pt_layout = new QHBoxLayout(pt_group);
    pt_layout->addWidget(new QLabel(QStringLiteral("Versão:"), pt_group));
    pt_version_ = new QLineEdit(default_version, pt_group);
    pt_layout->addWidget(pt_version_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Dia:"), pt_group));
    pt_day_ = new QSpinBox(pt_group);
    pt_day_->setRange(1, 31);
    pt_day_->setValue(default_day);
    pt_layout->addWidget(pt_day_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Mês:"), pt_group));
    pt_month_ = new QComboBox(pt_group);
    pt_month_->addItems(kPtMonths);
    pt_month_->setCurrentIndex(default_month_index);
    pt_layout->addWidget(pt_month_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Ano:"), pt_group));
    pt_year_ = new QSpinBox(pt_group);
    pt_year_->setRange(2000, 2100);
    pt_year_->setValue(default_year);
    pt_layout->addWidget(pt_year_);
    layout->addWidget(pt_group);

    auto *en_group = new QGroupBox(QStringLiteral("Inglês (en-US)"), this);
    auto *en_layout = new QHBoxLayout(en_group);
    en_layout->addWidget(new QLabel(QStringLiteral("Versão:"), en_group));
    en_version_ = new QLineEdit(default_version, en_group);
    en_layout->addWidget(en_version_);

    en_layout->addWidget(new QLabel(QStringLiteral("Dia:"), en_group));
    en_day_ = new QSpinBox(en_group);
    en_day_->setRange(1, 31);
    en_day_->setValue(default_day);
    en_layout->addWidget(en_day_);

    en_layout->addWidget(new QLabel(QStringLiteral("Mês:"), en_group));
    en_month_ = new QComboBox(en_group);
    en_month_->addItems(kEnMonths);
    en_month_->setCurrentIndex(default_month_index);
    en_layout->addWidget(en_month_);

    en_layout->addWidget(new QLabel(QStringLiteral("Ano:"), en_group));
    en_year_ = new QSpinBox(en_group);
    en_year_->setRange(2000, 2100);
    en_year_->setValue(default_year);
    en_layout->addWidget(en_year_);
    layout->addWidget(en_group);

    auto *buttons_layout = new QHBoxLayout();
    check_btn_ = new QPushButton(QStringLiteral("Verificar Padrões"), this);
    buttons_layout->addWidget(check_btn_);

    save_btn_ = new QPushButton(QStringLiteral("Salvar Alterações"), this);
    buttons_layout->addWidget(save_btn_);

    refresh_btn_ = new QPushButton(QStringLiteral("Recarregar Padrões"), this);
    buttons_layout->addWidget(refresh_btn_);
    layout->addLayout(buttons_layout);

    status_tabs_ = new QTabWidget(this);
    layout->addWidget(status_tabs_);

    connect(select_folder_btn_, &QPushButton::clicked, this, &VersionEditorWidget::selectBaseDirectory);
    connect(check_btn_, &QPushButton::clicked, this, &VersionEditorWidget::checkFiles);
    connect(save_btn_, &QPushButton::clicked, this, &VersionEditorWidget::saveChanges);
    connect(refresh_btn_, &QPushButton::clicked, this, &VersionEditorWidget::checkFiles);

    connect(pt_version_, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (en_version_->text() != text) {
            en_version_->setText(text);
        }
    });

    connect(pt_day_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
        if (en_day_->value() != value) {
            en_day_->setValue(value);
        }
    });

    connect(pt_month_, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (en_month_->currentIndex() != index) {
            en_month_->setCurrentIndex(index);
        }
    });

    connect(pt_year_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
        if (en_year_->value() != value) {
            en_year_->setValue(value);
        }
    });

    checkFiles();
}

void VersionEditorWidget::selectBaseDirectory()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("Selecionar pasta base do projeto"),
        selected_base_dir_);

    if (!dir.isEmpty()) {
        selected_base_dir_ = normalize_dir(dir);
        folder_path_->setText(selected_base_dir_);
        checkFiles();
    }
}

void VersionEditorWidget::setStatusLines(const QStringList &lines)
{
    QList<QPair<QString, QStringList>> groups;
    groups.append({QStringLiteral("Geral"), lines});
    setStatusGroups(groups);
}

void VersionEditorWidget::setStatusGroups(const QList<QPair<QString, QStringList>> &groups)
{
    const int previous_index = status_tabs_->currentIndex();
    const QString previous_title = (previous_index >= 0 && previous_index < status_tabs_->count())
                                       ? status_tabs_->tabText(previous_index)
                                       : QString();

    status_tabs_->clear();

    int restore_index = -1;
    for (int i = 0; i < groups.size(); ++i) {
        const auto &group = groups.at(i);
        auto *view = new QTextEdit(status_tabs_);
        view->setReadOnly(true);
        view->setPlainText(group.second.join(QLatin1Char('\n')));
        status_tabs_->addTab(view, group.first);

        if (restore_index < 0 && !previous_title.isEmpty() && group.first == previous_title) {
            restore_index = i;
        }
    }

    if (restore_index >= 0) {
        status_tabs_->setCurrentIndex(restore_index);
    } else if (status_tabs_->count() > 0) {
        status_tabs_->setCurrentIndex(0);
    }
}

void VersionEditorWidget::checkFiles()
{
    const QList<FileStatus> statuses = check_expected_lines(selected_base_dir_);
    QMap<QString, QStringList> grouped_lines;

    for (const QString &group : group_order()) {
        grouped_lines.insert(group, QStringList());
    }

    int total_files = statuses.size();
    int ok_files = 0;
    int missing_files = 0;
    int warning_files = 0;

    for (const FileStatus &status : statuses) {
        QStringList file_lines;
        file_lines.append(QStringLiteral("[%1]").arg(status.relative_path));

        if (!status.exists) {
            file_lines.append(QStringLiteral("  status: ARQUIVO NÃO ENCONTRADO"));
            file_lines.append(QStringLiteral("  caminho testado: %1").arg(status.path));
            ++missing_files;
        } else {
            file_lines.append(QStringLiteral("  status: OK"));
            if (!status.found.isEmpty()) {
                file_lines.append(QStringLiteral("  linhas encontradas:"));
                for (const QString &found : status.found) {
                    file_lines.append(QStringLiteral("    %1").arg(found));
                }
            }

            if (!status.missing.isEmpty()) {
                file_lines.append(QStringLiteral("  padrões ausentes:"));
                for (const QString &missing : status.missing) {
                    file_lines.append(QStringLiteral("    %1").arg(missing));
                }
                ++warning_files;
            } else {
                ++ok_files;
            }
        }

        file_lines.append(QString());
        grouped_lines[status.group].append(file_lines);
    }

    QStringList summary;
    summary.append(QStringLiteral("Base verificada: %1").arg(selected_base_dir_));
    summary.append(QStringLiteral("Total de arquivos monitorados: %1").arg(total_files));
    summary.append(QStringLiteral("Arquivos 100% conformes: %1").arg(ok_files));
    summary.append(QStringLiteral("Arquivos ausentes: %1").arg(missing_files));
    summary.append(QStringLiteral("Arquivos com padrões incompletos: %1").arg(warning_files));
    summary.append(QString());

    QList<QPair<QString, QStringList>> result_groups;
    result_groups.append({QStringLiteral("Visão Geral"), summary});

    for (const QString &group : group_order()) {
        const QStringList &lines = grouped_lines.value(group);
        if (!lines.isEmpty()) {
            result_groups.append({group, lines});
        }
    }

    setStatusGroups(result_groups);
}

void VersionEditorWidget::saveChanges()
{
    UpdateContext context;
    context.pt_version = pt_version_->text().trimmed();
    context.en_version = en_version_->text().trimmed();

    const int pt_month_index = pt_month_->currentIndex();
    const QString pt_month_name = (pt_month_index >= 0 && pt_month_index < kPtMonthsLower.size())
                                      ? kPtMonthsLower.at(pt_month_index)
                                      : QStringLiteral("agosto");

    context.pt_date = QStringLiteral("%1 de %2 de %3")
                          .arg(pt_day_->value())
                          .arg(pt_month_name)
                          .arg(pt_year_->value());

    const int en_month_index = en_month_->currentIndex();
    const QString en_month_name = (en_month_index >= 0 && en_month_index < kEnMonths.size())
                                      ? kEnMonths.at(en_month_index)
                                      : QStringLiteral("August");

    context.en_date = QStringLiteral("%1 %2, %3")
                          .arg(en_month_name)
                          .arg(en_day_->value())
                          .arg(en_year_->value());

    context.pt_readme_version = readme_version_token(context.pt_version);
    context.en_readme_version = readme_version_token(context.en_version);

    const QList<UpdateResult> results = apply_updates(context, selected_base_dir_);

    QMap<QString, QStringList> grouped_lines;
    for (const QString &group : group_order()) {
        grouped_lines.insert(group, QStringList());
    }

    int success_count = 0;
    int error_count = 0;

    for (const UpdateResult &result : results) {
        QStringList item_lines;
        item_lines.append(QStringLiteral("[%1]").arg(result.relative_path));
        item_lines.append(QStringLiteral("  resultado: %1").arg(result.ok ? QStringLiteral("SUCESSO") : QStringLiteral("FALHA")));
        item_lines.append(QStringLiteral("  mensagem: %1").arg(result.message));
        item_lines.append(QString());

        if (result.ok) {
            ++success_count;
        } else {
            ++error_count;
        }

        grouped_lines[result.group].append(item_lines);
    }

    QStringList summary;
    summary.append(QStringLiteral("Salvamento concluído."));
    summary.append(QStringLiteral("Base: %1").arg(selected_base_dir_));
    summary.append(QStringLiteral("Arquivos atualizados com sucesso: %1").arg(success_count));
    summary.append(QStringLiteral("Falhas/Não modificados: %1").arg(error_count));
    summary.append(QString());

    QList<QPair<QString, QStringList>> result_groups;
    result_groups.append({QStringLiteral("Resumo Salvar"), summary});

    for (const QString &group : group_order()) {
        const QStringList &lines = grouped_lines.value(group);
        if (!lines.isEmpty()) {
            result_groups.append({group, lines});
        }
    }

    setStatusGroups(result_groups);
}

}  // namespace mocks::version_editor

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mocks::version_editor::VersionEditorWidget widget;
    widget.show();
    return app.exec();
}
