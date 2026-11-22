#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QVector>
#include "source/config/LzmaExtractor.hpp"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QPushButton;
class QProgressBar;
class QTableView;
class QLabel;
class QLineEdit;
class QTextEdit;
class QTabWidget;
QT_END_NAMESPACE

class LzmaMatchesTableModel;

namespace source::language {
class GerenciadorTraducao;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(source::language::GerenciadorTraducao *tradutor = nullptr, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void retranslateUi();
    void on_selecionar_arquivo_clicked();
    void on_iniciar_extracao_clicked();
    void on_cancelar_extracao_clicked();
    void on_extracao_progresso(int pct, const QString &status);
    void on_matches_batch(const QVector<LzmaMatchInfo> &batch);
    void on_extracao_finalizada(bool success, const QString &message,
                                quint64 totalMatches, quint64 totalLiterals,
                                quint64 uncompressedSize, qint64 elapsedMs);
    void on_action_sobre_clicked();
    void on_action_leiame_clicked();
    void on_action_manual_clicked();
    void on_action_nota_lancamento_clicked();
    void on_action_documentacao_clicked();

private:
    void setupUi();
    void setupMenus();
    void setupConnections();
    void atualizar_menu_idiomas();

    source::language::GerenciadorTraducao *gerenciador_traducao_{nullptr};
    QPointer<LzmaExtractor> extractor_thread_;

    // Menu Bar
    QMenu *menu_arquivo_{nullptr};
    QAction *acao_abrir_{nullptr};
    QAction *acao_sair_{nullptr};

    QMenu *menu_configuracoes_{nullptr};
    QMenu *menu_idioma_{nullptr};
    QActionGroup *grupo_idiomas_{nullptr};
    QAction *acao_pt_br_{nullptr};
    QAction *acao_en_us_{nullptr};

    QMenu *menu_sobre_{nullptr};
    QAction *acao_sobre_sobre_{nullptr};
    QAction *acao_sobre_leiame_{nullptr};
    QAction *acao_sobre_manual_{nullptr};
    QAction *acao_sobre_nota_lancamento_{nullptr};
    QAction *acao_sobre_documentacao_{nullptr};

    // UI Widgets
    QWidget *central_widget_{nullptr};
    QLineEdit *edit_caminho_arquivo_{nullptr};
    QPushButton *btn_selecionar_arquivo_{nullptr};
    QPushButton *btn_iniciar_extracao_{nullptr};
    QPushButton *btn_cancelar_extracao_{nullptr};

    QProgressBar *barra_progresso_{nullptr};
    QLabel *lbl_status_{nullptr};

    // Stats Labels
    QLabel *lbl_total_matches_{nullptr};
    QLabel *lbl_total_literals_{nullptr};
    QLabel *lbl_uncompressed_size_{nullptr};
    QLabel *lbl_tempo_decorrido_{nullptr};

    // Virtual Table
    QTableView *tabela_matches_{nullptr};
    LzmaMatchesTableModel *modelo_matches_{nullptr};

    quint64 total_matches_count_{0};
    quint64 total_literals_count_{0};
};
