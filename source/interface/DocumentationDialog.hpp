#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QPushButton;
class QLabel;
class QPlainTextEdit;
QT_END_NAMESPACE

namespace source::language {
class GerenciadorTraducao;
}

class DocumentationDialog : public QDialog {
    Q_OBJECT

public:
    explicit DocumentationDialog(source::language::GerenciadorTraducao *tradutor = nullptr,
                                QWidget *parent = nullptr);
    ~DocumentationDialog() override = default;

public slots:
    void retranslateUi();

protected:
    void changeEvent(QEvent *event) override;

private:
    void setupUi();
    void loadLicenses();
    QString findLicensePath(const QString &fileName) const;

    source::language::GerenciadorTraducao *tradutor_{nullptr};
    QLabel *lbl_titulo_{nullptr};
    QTabWidget *tab_widget_{nullptr};
    QPushButton *btn_fechar_{nullptr};

    struct LicenseTabEntry {
        QString fileName;
        QString tabKey;
        QPlainTextEdit *textEdit{nullptr};
    };

    QList<LicenseTabEntry> tabs_;
};
