#pragma once

#include <QDialog>
#include <QString>

class QTextBrowser;
class QPushButton;
class QLabel;

namespace source::language {
class GerenciadorTraducao;
}

enum class DocumentType {
    About,
    Readme,
    Manual,
    ReleaseNotes
};

class DocumentViewerDialog : public QDialog {
    Q_OBJECT

public:
    explicit DocumentViewerDialog(DocumentType type,
                                 source::language::GerenciadorTraducao *tradutor,
                                 QWidget *parent = nullptr);
    ~DocumentViewerDialog() override = default;

    void setDocumentType(DocumentType type);

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void retranslateUi();
    void loadDocumentContent();

private:
    void scrollToChapter(const QString &anchor);
    QString findDocumentFilePath() const;

    DocumentType document_type_;
    source::language::GerenciadorTraducao *tradutor_{nullptr};

    QLabel *label_title_{nullptr};
    QTextBrowser *text_browser_{nullptr};
    QPushButton *btn_close_{nullptr};
};
