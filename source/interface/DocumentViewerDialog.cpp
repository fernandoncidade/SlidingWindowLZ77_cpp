#include "DocumentViewerDialog.hpp"
#include "source/interface/AppIconHelper.hpp"
#include "source/locale/tr_01_gerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QScrollBar>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QEvent>

DocumentViewerDialog::DocumentViewerDialog(DocumentType type,
                                           source::language::GerenciadorTraducao *tradutor,
                                           QWidget *parent)
    : QDialog(parent),
      document_type_(type),
      tradutor_(tradutor)
{
    setWindowIcon(source::interface::get_app_window_icon());
    resize(850, 650);
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(8);

    label_title_ = new QLabel(this);
    QFont title_font = label_title_->font();
    title_font.setPointSize(12);
    title_font.setBold(true);
    label_title_->setFont(title_font);
    main_layout->addWidget(label_title_);

    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    main_layout->addWidget(separator);

    text_browser_ = new QTextBrowser(this);
    text_browser_->setOpenExternalLinks(false);
    text_browser_->setOpenLinks(false);
    text_browser_->setFrameShape(QFrame::StyledPanel);
    text_browser_->setFrameShadow(QFrame::Sunken);

    connect(text_browser_, &QTextBrowser::anchorClicked, this, [this](const QUrl &url) {
        const QString scheme = url.scheme().toLower();
        if (scheme == QLatin1String("http") || scheme == QLatin1String("https") || scheme == QLatin1String("mailto")) {
            QDesktopServices::openUrl(url);
            return;
        }

        QString anchor = url.fragment();
        if (anchor.isEmpty()) {
            QString urlStr = url.toString();
            if (urlStr.startsWith(QLatin1Char('#'))) {
                anchor = urlStr.mid(1);
            }
        }

        scrollToChapter(anchor);
    });

    main_layout->addWidget(text_browser_, 1);

    auto *btn_layout = new QHBoxLayout();
    btn_layout->addStretch();

    btn_close_ = new QPushButton(this);
    btn_close_->setIcon(source::interface::get_app_button_icon());
    btn_close_->setDefault(true);
    btn_close_->setAutoDefault(true);
    connect(btn_close_, &QPushButton::clicked, this, &QDialog::accept);
    btn_layout->addWidget(btn_close_);

    main_layout->addLayout(btn_layout);

    retranslateUi();
    loadDocumentContent();
}

void DocumentViewerDialog::setDocumentType(DocumentType type)
{
    document_type_ = type;
    retranslateUi();
    loadDocumentContent();
}

void DocumentViewerDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
        loadDocumentContent();
    }
    QDialog::changeEvent(event);
}

void DocumentViewerDialog::retranslateUi()
{
    btn_close_->setText(tr("Fechar"));

    switch (document_type_) {
    case DocumentType::About:
        setWindowTitle(tr("Sobre o Sliding Window LZ77 Visualizer"));
        label_title_->setText(tr("Sobre o Sistema"));
        break;
    case DocumentType::Readme:
        setWindowTitle(tr("Leia-me - Sliding Window LZ77 Visualizer"));
        label_title_->setText(tr("Informações do Projeto (Leia-me)"));
        break;
    case DocumentType::Manual:
        setWindowTitle(tr("Manual do Usuário - Sliding Window LZ77 Visualizer"));
        label_title_->setText(tr("Manual de Instruções e Guia de Uso"));
        break;
    case DocumentType::ReleaseNotes:
        setWindowTitle(tr("Nota de Lançamento - Sliding Window LZ77 Visualizer"));
        label_title_->setText(tr("Notas de Lançamento da Versão"));
        break;
    }
}

QString DocumentViewerDialog::findDocumentFilePath() const
{
    QString lang_code = tradutor_ ? tradutor_->obter_idioma_atual() : QStringLiteral("pt_BR");
    const bool is_pt = lang_code.startsWith(QStringLiteral("pt"), Qt::CaseInsensitive);

    QString doc_name;
    switch (document_type_) {
    case DocumentType::About:
        doc_name = is_pt ? QStringLiteral("SOBRE.md") : QStringLiteral("ABOUT.md");
        break;
    case DocumentType::Readme:
        doc_name = is_pt ? QStringLiteral("LEIAME.md") : QStringLiteral("README.md");
        break;
    case DocumentType::Manual:
        doc_name = QStringLiteral("MANUAL.md");
        break;
    case DocumentType::ReleaseNotes:
        doc_name = is_pt ? QStringLiteral("release_pt-BR.md") : QStringLiteral("release_en-US.md");
        break;
    }

    const QString lang_folder = is_pt ? QStringLiteral("pt_BR") : QStringLiteral("en_US");

    QStringList candidate_paths;
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString pwd_dir = QDir::currentPath();

    if (document_type_ == DocumentType::ReleaseNotes) {
        candidate_paths << app_dir + QStringLiteral("/source/assets/RELEASES/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/assets/RELEASES/") + doc_name;
        candidate_paths << pwd_dir + QStringLiteral("/source/assets/RELEASES/") + doc_name;
        candidate_paths << pwd_dir + QStringLiteral("/assets/RELEASES/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/../source/assets/RELEASES/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/../../source/assets/RELEASES/") + doc_name;
        candidate_paths << QStringLiteral("C:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/source/assets/RELEASES/") + doc_name;
    } else {
        // 1. Arquivos dedicados no diretório source/docs (pt_BR ou en_US)
        candidate_paths << app_dir + QStringLiteral("/source/docs/") + lang_folder + QStringLiteral("/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/docs/") + lang_folder + QStringLiteral("/") + doc_name;
        candidate_paths << pwd_dir + QStringLiteral("/source/docs/") + lang_folder + QStringLiteral("/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/../source/docs/") + lang_folder + QStringLiteral("/") + doc_name;
        candidate_paths << app_dir + QStringLiteral("/../../source/docs/") + lang_folder + QStringLiteral("/") + doc_name;

        // 2. Caminho absoluto do repositório
        candidate_paths << QStringLiteral("C:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/source/docs/") + lang_folder + QStringLiteral("/") + doc_name;

        // 3. Arquivos unificados na raiz como fallback
        const QString root_doc_name = (document_type_ == DocumentType::About)
                                          ? QStringLiteral("ABOUT.md")
                                          : (document_type_ == DocumentType::Readme)
                                                ? QStringLiteral("README.md")
                                                : QStringLiteral("MANUAL.md");

        candidate_paths << app_dir + QStringLiteral("/") + root_doc_name;
        candidate_paths << pwd_dir + QStringLiteral("/") + root_doc_name;
        candidate_paths << app_dir + QStringLiteral("/../") + root_doc_name;
        candidate_paths << app_dir + QStringLiteral("/../../") + root_doc_name;
        candidate_paths << QStringLiteral("C:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/") + root_doc_name;
    }

    for (const QString &path : candidate_paths) {
        if (QFileInfo::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }
    return QString();
}

void DocumentViewerDialog::loadDocumentContent()
{
    const QString path = findDocumentFilePath();
    if (path.isEmpty()) {
        text_browser_->setPlainText(tr("Arquivo de documentação não encontrado."));
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        text_browser_->setPlainText(tr("Não foi possível abrir o arquivo: %1").arg(path));
        return;
    }

    QString content = QString::fromUtf8(file.readAll());
    file.close();

    // Sanitizar tags de bloco HTML que quebram o analisador CommonMark do Qt6
    content.remove(QRegularExpression(QStringLiteral("</?(?:details|summary|p)(?:\\s+[^>]*)?>"), QRegularExpression::CaseInsensitiveOption));

    // Normalizar tags <br> para <br /> evitando que o analisador CommonMark do Qt6 descarte o Markdown seguinte
    content.replace(QRegularExpression(QStringLiteral("<br\\s*(?<!/)>"), QRegularExpression::CaseInsensitiveOption), QStringLiteral("<br />"));

    text_browser_->setMarkdown(content);

    QString lang_code = tradutor_ ? tradutor_->obter_idioma_atual() : QStringLiteral("pt_BR");
    const bool is_pt = lang_code.startsWith(QStringLiteral("pt"), Qt::CaseInsensitive);
    if (content.contains(QStringLiteral("id=\"ptbr\"")) || content.contains(QStringLiteral("id=\"enus\""))) {
        scrollToChapter(is_pt ? QStringLiteral("ptbr") : QStringLiteral("enus"));
    }
}

void DocumentViewerDialog::scrollToChapter(const QString &anchor)
{
    if (anchor.isEmpty() || anchor == QLatin1String("inicio") ||
        anchor == QLatin1String("top") || anchor == QLatin1String("sumario")) {
        if (text_browser_->verticalScrollBar()) {
            text_browser_->verticalScrollBar()->setValue(0);
        }
        return;
    }

    // Try standard anchor scrolling first
    text_browser_->scrollToAnchor(anchor);

    // Extract chapter number if present (e.g. "capitulo-1", "chapter-2", "1-introducao", "8")
    int chapterNum = -1;
    const QRegularExpression numRx(QStringLiteral("(\\d+)"));
    const QRegularExpressionMatch match = numRx.match(anchor);
    if (match.hasMatch()) {
        chapterNum = match.captured(1).toInt();
    }

    if (!text_browser_->document()) {
        return;
    }

    bool foundSumario = false;
    bool pastSumario = false;

    for (QTextBlock block = text_browser_->document()->begin(); block.isValid(); block = block.next()) {
        const QString blockText = block.text().trimmed();
        if (blockText.isEmpty()) {
            continue;
        }

        if (blockText.contains(QLatin1String("Sumário"), Qt::CaseInsensitive) ||
            blockText.contains(QLatin1String("Table of Contents"), Qt::CaseInsensitive)) {
            foundSumario = true;
            continue;
        }

        // Once we pass the Table of Contents and reach chapter 1
        if (foundSumario && !pastSumario) {
            if (blockText.startsWith(QLatin1String("1.")) || blockText.startsWith(QLatin1String("1 "))) {
                pastSumario = true;
            }
        }

        if (pastSumario || !foundSumario) {
            bool isMatch = false;
            if (chapterNum > 0) {
                const QString prefix = QString::number(chapterNum) + QLatin1Char('.');
                if (blockText.startsWith(prefix)) {
                    isMatch = true;
                }
            }

            if (!isMatch && !anchor.isEmpty()) {
                QString cleanAnchor = anchor;
                cleanAnchor.replace(QLatin1Char('-'), QLatin1Char(' ')).replace(QLatin1Char('_'), QLatin1Char(' '));
                if (blockText.contains(cleanAnchor, Qt::CaseInsensitive)) {
                    isMatch = true;
                }
            }

            if (isMatch) {
                QTextCursor cursor(block);
                text_browser_->setTextCursor(cursor);
                text_browser_->ensureCursorVisible();

                const QRect cursorRect = text_browser_->cursorRect(cursor);
                if (text_browser_->verticalScrollBar()) {
                    const int currentScroll = text_browser_->verticalScrollBar()->value();
                    const int targetScroll = qMax(0, currentScroll + cursorRect.top() - 6);
                    text_browser_->verticalScrollBar()->setValue(targetScroll);
                }
                return;
            }
        }
    }
}
