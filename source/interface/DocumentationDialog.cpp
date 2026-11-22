#include "DocumentationDialog.hpp"
#include "source/interface/AppIconHelper.hpp"
#include "source/locale/tr_01_gerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPlainTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QEvent>

DocumentationDialog::DocumentationDialog(source::language::GerenciadorTraducao *tradutor,
                                         QWidget *parent)
    : QDialog(parent),
      tradutor_(tradutor)
{
    setupUi();
    loadLicenses();
    retranslateUi();

    if (tradutor_) {
        connect(tradutor_, &source::language::GerenciadorTraducao::idioma_alterado,
                this, &DocumentationDialog::retranslateUi);
    }
}

void DocumentationDialog::changeEvent(QEvent *event)
{
    if (event && event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void DocumentationDialog::setupUi()
{
    setWindowIcon(source::interface::get_app_window_icon());
    resize(960, 680);
    setMinimumSize(750, 500);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    lbl_titulo_ = new QLabel(this);
    QFont titleFont = lbl_titulo_->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    lbl_titulo_->setFont(titleFont);
    mainLayout->addWidget(lbl_titulo_);

    auto *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    tab_widget_ = new QTabWidget(this);
    tab_widget_->setUsesScrollButtons(true);
    mainLayout->addWidget(tab_widget_, 1);

    // Bottom layout with Close button at the bottom right
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    btn_fechar_ = new QPushButton(this);
    btn_fechar_->setIcon(source::interface::get_app_button_icon());
    btn_fechar_->setDefault(true);
    btn_fechar_->setAutoDefault(true);
    connect(btn_fechar_, &QPushButton::clicked, this, &QDialog::accept);
    bottomLayout->addWidget(btn_fechar_);

    mainLayout->addLayout(bottomLayout);
}

QString DocumentationDialog::findLicensePath(const QString &fileName) const
{
    const QStringList candidate_paths = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/assets/LICENSES/%1").arg(fileName),
        QCoreApplication::applicationDirPath() + QStringLiteral("/source/assets/LICENSES/%1").arg(fileName),
        QCoreApplication::applicationDirPath() + QStringLiteral("/LICENSES/%1").arg(fileName),
        QDir::current().filePath(QStringLiteral("assets/LICENSES/%1").arg(fileName)),
        QDir::current().filePath(QStringLiteral("source/assets/LICENSES/%1").arg(fileName)),
        QDir::current().filePath(QStringLiteral("LICENSES/%1").arg(fileName)),
        QStringLiteral("C:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/source/assets/LICENSES/%1").arg(fileName)
    };

    for (const QString &path : candidate_paths) {
        if (QFileInfo::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }
    return QString();
}

void DocumentationDialog::loadLicenses()
{
    tabs_ = {
        { QStringLiteral("LICENSE"), QStringLiteral("Licença MIT"), nullptr },
        { QStringLiteral("THIRD_PARTY_COMPONENT_NOTICES.txt"), QStringLiteral("Componentes de Terceiros"), nullptr },
        { QStringLiteral("OPEN_SOURCE_SOURCE_OFFER_pt_BR.txt"), QStringLiteral("Oferta de Código (PT)"), nullptr },
        { QStringLiteral("OPEN_SOURCE_SOURCE_OFFER_en_US.txt"), QStringLiteral("Oferta de Código (EN)"), nullptr },
        { QStringLiteral("GNU LESSER GENERAL PUBLIC LICENSE Version 3.txt"), QStringLiteral("LGPL v3 (Qt 6)"), nullptr },
        { QStringLiteral("GNU GENERAL PUBLIC LICENSE Version 3.txt"), QStringLiteral("GPL v3"), nullptr },
        { QStringLiteral("GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.txt"), QStringLiteral("LGPL v2.1 (LZMA SDK)"), nullptr },
        { QStringLiteral("BSD License Clause-3.txt"), QStringLiteral("BSD 3-Clause (LZMA SDK)"), nullptr },
        { QStringLiteral("GCC Runtime Library Exception 3.1.txt"), QStringLiteral("Exceção GCC Runtime"), nullptr },
        { QStringLiteral("MinGW-w64 Runtime License.txt"), QStringLiteral("MinGW-w64 Runtime"), nullptr },
        { QStringLiteral("Microsoft Redistributable Runtime Notice.txt"), QStringLiteral("MSVC Redistributable"), nullptr }
    };

    const QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    for (auto &entry : tabs_) {
        auto *textEdit = new QPlainTextEdit(this);
        textEdit->setReadOnly(true);
        textEdit->setFont(monoFont);
        textEdit->setFrameShape(QFrame::StyledPanel);
        textEdit->setFrameShadow(QFrame::Sunken);

        const QString fullPath = findLicensePath(entry.fileName);
        if (!fullPath.isEmpty()) {
            QFile file(fullPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                textEdit->setPlainText(QString::fromUtf8(file.readAll()));
                file.close();
            } else {
                textEdit->setPlainText(tr("Não foi possível ler o arquivo: %1").arg(entry.fileName));
            }
        } else {
            textEdit->setPlainText(tr("Arquivo de licença não encontrado: %1").arg(entry.fileName));
        }

        entry.textEdit = textEdit;
        tab_widget_->addTab(textEdit, entry.tabKey);
    }
}

void DocumentationDialog::retranslateUi()
{
    setWindowTitle(tr("Documentação - Licenças, Privacidade e Permissões"));
    lbl_titulo_->setText(tr("Licenças e Notificações de Componentes de Terceiros"));
    btn_fechar_->setText(tr("Fechar"));

    // Update tab labels
    if (tab_widget_->count() >= 11) {
        tab_widget_->setTabText(0, tr("Licença MIT"));
        tab_widget_->setTabText(1, tr("Componentes de Terceiros"));
        tab_widget_->setTabText(2, tr("Oferta de Código (PT)"));
        tab_widget_->setTabText(3, tr("Oferta de Código (EN)"));
        tab_widget_->setTabText(4, tr("LGPL v3 (Qt 6)"));
        tab_widget_->setTabText(5, tr("GPL v3"));
        tab_widget_->setTabText(6, tr("LGPL v2.1 (LZMA SDK)"));
        tab_widget_->setTabText(7, tr("BSD 3-Clause (LZMA SDK)"));
        tab_widget_->setTabText(8, tr("Exceção GCC Runtime"));
        tab_widget_->setTabText(9, tr("MinGW-w64 Runtime"));
        tab_widget_->setTabText(10, tr("MSVC Redistributable"));
    }
}
