#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace mocks::version_editor {

class VersionEditorWidget : public QWidget {
public:
    explicit VersionEditorWidget(QWidget *parent = nullptr);

private:
    void selectBaseDirectory();
    void checkFiles();
    void saveChanges();
    void setStatusLines(const QStringList &lines);
    void setStatusGroups(const QList<QPair<QString, QStringList>> &groups);

    QString selected_base_dir_;

    QLineEdit *folder_path_ = nullptr;

    QLineEdit *pt_version_ = nullptr;
    QSpinBox *pt_day_ = nullptr;
    QComboBox *pt_month_ = nullptr;
    QSpinBox *pt_year_ = nullptr;

    QLineEdit *en_version_ = nullptr;
    QSpinBox *en_day_ = nullptr;
    QComboBox *en_month_ = nullptr;
    QSpinBox *en_year_ = nullptr;

    QPushButton *select_folder_btn_ = nullptr;
    QPushButton *check_btn_ = nullptr;
    QPushButton *save_btn_ = nullptr;
    QPushButton *refresh_btn_ = nullptr;

    QTabWidget *status_tabs_ = nullptr;
};

}  // namespace mocks::version_editor
