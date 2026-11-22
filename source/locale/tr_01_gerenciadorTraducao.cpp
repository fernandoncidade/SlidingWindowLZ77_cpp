#include "source/locale/tr_01_gerenciadorTraducao.hpp"
#include "source/logs/LogManager.hpp"

#include <QAbstractButton>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTranslator>

namespace source::language {
namespace {

source::logs::LogManager *logger()
{
    return source::logs::LogManager::get_instance();
}

QString app_data_dir()
{
    QString local_app_data = qEnvironmentVariable("LOCALAPPDATA");
    if (local_app_data.trimmed().isEmpty()) {
        local_app_data = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    }
    if (local_app_data.trimmed().isEmpty()) {
        local_app_data = QDir(QDir::homePath()).filePath(QStringLiteral("AppData/Local"));
    }
    return QDir(local_app_data).filePath(QStringLiteral("LZ77_SlidingWindow"));
}

QString translations_dir()
{
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString current_dir = QDir::currentPath();
    const QStringList candidates = {
        QDir(app_dir).filePath(QStringLiteral("translations")),
        QDir(app_dir).filePath(QStringLiteral("source/locale/translations")),
        QDir(current_dir).filePath(QStringLiteral("source/locale/translations")),
        QDir(app_dir).filePath(QStringLiteral("../source/locale/translations")),
        QDir(app_dir).filePath(QStringLiteral("../../source/locale/translations")),
        QDir(app_dir).filePath(QStringLiteral("../../../source/locale/translations")),
    };

    for (const QString &candidate : candidates) {
        if (QDir(candidate).exists()) {
            return QFileInfo(candidate).absoluteFilePath();
        }
    }

    return QFileInfo(candidates.constFirst()).absoluteFilePath();
}

QStringList translation_file_candidates(const QString &language_code)
{
    return {
        QStringLiteral("slidingwindowlz77_%1.qm").arg(language_code),
        QStringLiteral("slidingwindowlz77_%1.ts").arg(language_code),
    };
}

}  // namespace

GerenciadorTraducao::GerenciadorTraducao(QObject *parent)
    : QObject(parent),
      tradutor_(nullptr),
      qt_translator_(nullptr),
      idioma_atual_(QStringLiteral("pt_BR")),
      app_(QCoreApplication::instance()),
      idioma_padrao_(QStringLiteral("pt_BR")),
      dir_traducoes_(translations_dir())
{
    idiomas_disponiveis_.insert(QStringLiteral("pt_BR"), QStringLiteral("Português (Brasil)"));
    idiomas_disponiveis_.insert(QStringLiteral("en_US"), QStringLiteral("English (United States)"));

    carregar_configuracao_idioma();
}

bool GerenciadorTraducao::aplicar_traducao()
{
    auto *app = qobject_cast<QCoreApplication *>(app_ ? app_ : QCoreApplication::instance());
    if (app == nullptr) {
        return false;
    }

    remover_tradutor_instalado();

    const QString locale_code = QString(idioma_atual_).replace(QLatin1Char('_'), QLatin1Char('-'));
    QLocale::setDefault(QLocale(locale_code));
    load_qt_translator_for_locale(idioma_atual_);

    dir_traducoes_ = translations_dir();

    bool aplicou_app_trad = false;
    for (const QString &translation_file : translation_file_candidates(idioma_atual_)) {
        const QString translation_path = QDir(dir_traducoes_).filePath(translation_file);
        if (!QFileInfo::exists(translation_path)) {
            continue;
        }

        tradutor_ = new QTranslator(this);
        if (tradutor_->load(translation_path)) {
            app->installTranslator(tradutor_);
            aplicou_app_trad = true;
            logger()->info(QStringLiteral("Arquivo de tradução carregado: %1").arg(translation_path));
            break;
        }

        logger()->error(QStringLiteral("Erro ao carregar arquivo de tradução: %1").arg(translation_path));
        tradutor_->deleteLater();
        tradutor_ = nullptr;
    }

    if (aplicou_app_trad || qt_translator_ != nullptr || idioma_atual_ == idioma_padrao_) {
        return true;
    }

    logger()->warning(QStringLiteral("Nenhum arquivo de tradução encontrado para %1 no diretório %2")
                          .arg(idioma_atual_, dir_traducoes_));
    return false;
}

bool GerenciadorTraducao::definir_idioma(const QString &codigo_idioma)
{
    if (!idiomas_disponiveis_.contains(codigo_idioma)) {
        return false;
    }

    idioma_atual_ = codigo_idioma;
    salvar_configuracao_idioma();
    const bool resultado = aplicar_traducao();
    emit idioma_alterado(codigo_idioma);
    return resultado;
}

QString GerenciadorTraducao::obter_idioma_atual() const
{
    return idioma_atual_;
}

void GerenciadorTraducao::traduzir_botoes_padrao(QObject *dialogo) const
{
    if (auto *message_box = qobject_cast<QMessageBox *>(dialogo)) {
        const struct ButtonText {
            QMessageBox::StandardButton button;
            QString text;
        } buttons[] = {
            {QMessageBox::Ok, QStringLiteral("OK")},
            {QMessageBox::Cancel, QStringLiteral("Cancelar")},
            {QMessageBox::Yes, QStringLiteral("Sim")},
            {QMessageBox::No, QStringLiteral("Não")},
            {QMessageBox::Abort, QStringLiteral("Abortar")},
            {QMessageBox::Retry, QStringLiteral("Tentar Novamente")},
            {QMessageBox::Ignore, QStringLiteral("Ignorar")},
            {QMessageBox::Close, QStringLiteral("Fechar")},
            {QMessageBox::Help, QStringLiteral("Ajuda")},
            {QMessageBox::Apply, QStringLiteral("Aplicar")},
            {QMessageBox::Reset, QStringLiteral("Redefinir")},
            {QMessageBox::RestoreDefaults, QStringLiteral("Restaurar Padrões")},
            {QMessageBox::Save, QStringLiteral("Salvar")},
            {QMessageBox::SaveAll, QStringLiteral("Salvar Tudo")},
            {QMessageBox::Open, QStringLiteral("Abrir")},
        };

        for (const ButtonText &button : buttons) {
            if (QAbstractButton *widget = message_box->button(button.button)) {
                widget->setText(QCoreApplication::translate("MainWindow", button.text.toUtf8().constData()));
            }
        }
    } else if (auto *button_box = qobject_cast<QDialogButtonBox *>(dialogo)) {
        const struct ButtonText {
            QDialogButtonBox::StandardButton button;
            QString text;
        } buttons[] = {
            {QDialogButtonBox::Ok, QStringLiteral("OK")},
            {QDialogButtonBox::Cancel, QStringLiteral("Cancelar")},
            {QDialogButtonBox::Yes, QStringLiteral("Sim")},
            {QDialogButtonBox::No, QStringLiteral("Não")},
            {QDialogButtonBox::Abort, QStringLiteral("Abortar")},
            {QDialogButtonBox::Retry, QStringLiteral("Tentar Novamente")},
            {QDialogButtonBox::Ignore, QStringLiteral("Ignorar")},
            {QDialogButtonBox::Close, QStringLiteral("Fechar")},
            {QDialogButtonBox::Help, QStringLiteral("Ajuda")},
            {QDialogButtonBox::Apply, QStringLiteral("Aplicar")},
            {QDialogButtonBox::Reset, QStringLiteral("Redefinir")},
            {QDialogButtonBox::RestoreDefaults, QStringLiteral("Restaurar Padrões")},
            {QDialogButtonBox::Save, QStringLiteral("Salvar")},
            {QDialogButtonBox::SaveAll, QStringLiteral("Salvar Tudo")},
            {QDialogButtonBox::Open, QStringLiteral("Abrir")},
        };

        for (const ButtonText &button : buttons) {
            if (QAbstractButton *widget = button_box->button(button.button)) {
                widget->setText(QCoreApplication::translate("MainWindow", button.text.toUtf8().constData()));
            }
        }
    }
}

const QMap<QString, QString> &GerenciadorTraducao::idiomas_disponiveis() const
{
    return idiomas_disponiveis_;
}

void GerenciadorTraducao::carregar_configuracao_idioma()
{
    QFile file(obter_caminho_configuracao());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonObject config = doc.object();
    if (config.contains(QStringLiteral("idioma"))) {
        idioma_atual_ = config.value(QStringLiteral("idioma")).toString(idioma_atual_);
    }
}

void GerenciadorTraducao::salvar_configuracao_idioma() const
{
    const QString config_path = obter_caminho_configuracao();
    QDir().mkpath(QFileInfo(config_path).absolutePath());

    QSaveFile file(config_path);
    if (!file.open(QIODevice::WriteOnly)) {
        logger()->error(QStringLiteral("Erro ao salvar configuração de idioma: %1").arg(config_path));
        return;
    }

    QJsonObject config;
    config.insert(QStringLiteral("idioma"), idioma_atual_);
    const QByteArray payload = QJsonDocument(config).toJson(QJsonDocument::Indented);
    if (file.write(payload) != payload.size() || !file.commit()) {
        logger()->error(QStringLiteral("Erro ao confirmar configuração de idioma: %1").arg(config_path));
    }
}

QString GerenciadorTraducao::obter_caminho_configuracao() const
{
    return QDir(app_data_dir()).filePath(QStringLiteral("config/language.json"));
}

void GerenciadorTraducao::remover_tradutor_instalado()
{
    auto *app = qobject_cast<QCoreApplication *>(app_ ? app_ : QCoreApplication::instance());
    if (app == nullptr) {
        return;
    }

    if (tradutor_ != nullptr) {
        app->removeTranslator(tradutor_);
        tradutor_->deleteLater();
        tradutor_ = nullptr;
    }

    if (qt_translator_ != nullptr) {
        app->removeTranslator(qt_translator_);
        qt_translator_->deleteLater();
        qt_translator_ = nullptr;
    }
}

bool GerenciadorTraducao::load_qt_translator_for_locale(const QString &codigo_idioma)
{
    auto *app = qobject_cast<QCoreApplication *>(app_ ? app_ : QCoreApplication::instance());
    if (app == nullptr) {
        return false;
    }

    const QString trans_dir = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    if (!QDir(trans_dir).exists()) {
        return false;
    }

    const QLocale locale(codigo_idioma);
    const QStringList preferred_catalogs = {
        QStringLiteral("qtbase"),
        QStringLiteral("qt"),
    };
    for (const QString &catalog : preferred_catalogs) {
        auto *translator = new QTranslator(this);
        if (translator->load(locale, catalog, QStringLiteral("_"), trans_dir)) {
            qt_translator_ = translator;
            app->installTranslator(qt_translator_);
            return true;
        }
        delete translator;
    }

    return false;
}

}  // namespace source::language
