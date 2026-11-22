#include "source/interface/mainwindow.hpp"
#include "source/interface/LzmaMatchesTableModel.hpp"
#include "source/interface/AppIconHelper.hpp"
#include "source/interface/DocumentViewerDialog.hpp"
#include "source/interface/DocumentationDialog.hpp"
#include "source/locale/tr_01_gerenciadorTraducao.hpp"
#include "source/logs/LogManager.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>

MainWindow::MainWindow(source::language::GerenciadorTraducao *tradutor, QWidget *parent)
    : QMainWindow(parent),
      gerenciador_traducao_(tradutor)
{
    setupUi();
    setupMenus();
    setupConnections();
    retranslateUi();

    auto *logger = source::logs::LogManager::get_instance();
    logger->info(QStringLiteral("Janela principal inicializada com sucesso."));
}

MainWindow::~MainWindow()
{
    if (extractor_thread_) {
        extractor_thread_->request_cancel();
        extractor_thread_->wait();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event && event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::setupUi()
{
    setWindowIcon(source::interface::get_app_window_icon());
    resize(1080, 720);
    setMinimumSize(850, 550);

    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);

    auto *mainLayout = new QVBoxLayout(central_widget_);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    const QIcon buttonIcon = source::interface::get_app_button_icon();

    // --- 1. Archive Selection Group ---
    auto *groupArquivo = new QGroupBox(this);
    groupArquivo->setObjectName(QStringLiteral("groupArquivo"));
    auto *layoutArquivo = new QHBoxLayout(groupArquivo);

    edit_caminho_arquivo_ = new QLineEdit(this);
    edit_caminho_arquivo_->setObjectName(QStringLiteral("editCaminhoArquivo"));
    edit_caminho_arquivo_->setPlaceholderText(QStringLiteral("Selecione um arquivo (.7z, .rar, .zip, .tar, .wim, .xz, .gz, etc.)..."));

    btn_selecionar_arquivo_ = new QPushButton(this);
    btn_selecionar_arquivo_->setObjectName(QStringLiteral("btnSelecionarArquivo"));
    btn_selecionar_arquivo_->setIcon(buttonIcon);

    btn_iniciar_extracao_ = new QPushButton(this);
    btn_iniciar_extracao_->setObjectName(QStringLiteral("btnIniciarExtracao"));
    btn_iniciar_extracao_->setIcon(buttonIcon);
    btn_iniciar_extracao_->setStyleSheet(QStringLiteral("font-weight: bold; background-color: #0066cc; color: white; padding: 6px 14px;"));

    btn_cancelar_extracao_ = new QPushButton(this);
    btn_cancelar_extracao_->setObjectName(QStringLiteral("btnCancelarExtracao"));
    btn_cancelar_extracao_->setIcon(buttonIcon);
    btn_cancelar_extracao_->setEnabled(false);

    layoutArquivo->addWidget(edit_caminho_arquivo_, 1);
    layoutArquivo->addWidget(btn_selecionar_arquivo_);
    layoutArquivo->addWidget(btn_iniciar_extracao_);
    layoutArquivo->addWidget(btn_cancelar_extracao_);
    mainLayout->addWidget(groupArquivo);

    // --- 2. Progress & Status ---
    auto *groupProgresso = new QGroupBox(this);
    groupProgresso->setObjectName(QStringLiteral("groupProgresso"));
    auto *layoutProgresso = new QVBoxLayout(groupProgresso);

    barra_progresso_ = new QProgressBar(this);
    barra_progresso_->setRange(0, 100);
    barra_progresso_->setValue(0);
    barra_progresso_->setTextVisible(true);

    lbl_status_ = new QLabel(QStringLiteral("Pronto."), this);
    lbl_status_->setObjectName(QStringLiteral("lblStatus"));

    layoutProgresso->addWidget(barra_progresso_);
    layoutProgresso->addWidget(lbl_status_);
    mainLayout->addWidget(groupProgresso);

    // --- 3. Metrics Summary Group ---
    auto *groupMetricas = new QGroupBox(this);
    groupMetricas->setObjectName(QStringLiteral("groupMetricas"));
    auto *gridMetricas = new QGridLayout(groupMetricas);

    lbl_total_matches_ = new QLabel(QStringLiteral("0"), this);
    lbl_total_matches_->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: bold; color: #0088ff;"));
    lbl_total_literals_ = new QLabel(QStringLiteral("0"), this);
    lbl_total_literals_->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: bold; color: #28a745;"));
    lbl_uncompressed_size_ = new QLabel(QStringLiteral("0 bytes"), this);
    lbl_uncompressed_size_->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: bold; color: #e67e22;"));
    lbl_tempo_decorrido_ = new QLabel(QStringLiteral("0 ms"), this);
    lbl_tempo_decorrido_->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: bold; color: #9b59b6;"));

    auto *lblTitleMatches = new QLabel(this);
    lblTitleMatches->setObjectName(QStringLiteral("lblTitleMatches"));
    auto *lblTitleLiterals = new QLabel(this);
    lblTitleLiterals->setObjectName(QStringLiteral("lblTitleLiterals"));
    auto *lblTitleSize = new QLabel(this);
    lblTitleSize->setObjectName(QStringLiteral("lblTitleSize"));
    auto *lblTitleTime = new QLabel(this);
    lblTitleTime->setObjectName(QStringLiteral("lblTitleTime"));

    gridMetricas->addWidget(lblTitleMatches, 0, 0);
    gridMetricas->addWidget(lbl_total_matches_, 0, 1);
    gridMetricas->addWidget(lblTitleLiterals, 0, 2);
    gridMetricas->addWidget(lbl_total_literals_, 0, 3);
    gridMetricas->addWidget(lblTitleSize, 1, 0);
    gridMetricas->addWidget(lbl_uncompressed_size_, 1, 1);
    gridMetricas->addWidget(lblTitleTime, 1, 2);
    gridMetricas->addWidget(lbl_tempo_decorrido_, 1, 3);
    mainLayout->addWidget(groupMetricas);

    // --- 4. Matches Table ---
    auto *groupTabela = new QGroupBox(this);
    groupTabela->setObjectName(QStringLiteral("groupTabela"));
    auto *layoutTabela = new QVBoxLayout(groupTabela);

    modelo_matches_ = new LzmaMatchesTableModel(this);

    tabela_matches_ = new QTableView(this);
    tabela_matches_->setModel(modelo_matches_);
    tabela_matches_->setWordWrap(false);
    tabela_matches_->setAlternatingRowColors(true);
    tabela_matches_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela_matches_->setSelectionMode(QAbstractItemView::SingleSelection);
    tabela_matches_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela_matches_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tabela_matches_->horizontalHeader()->setStretchLastSection(true);
    tabela_matches_->horizontalHeader()->setHighlightSections(false);
    tabela_matches_->verticalHeader()->setVisible(false);
    tabela_matches_->verticalHeader()->setDefaultSectionSize(24);
    tabela_matches_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    layoutTabela->addWidget(tabela_matches_);
    mainLayout->addWidget(groupTabela, 1);

    statusBar()->showMessage(tr("Pronto"));
}

void MainWindow::setupMenus()
{
    const QIcon actionIcon = source::interface::get_app_button_icon();

    // Menu Arquivo
    menu_arquivo_ = menuBar()->addMenu(QStringLiteral("Arquivo"));
    menu_arquivo_->setObjectName(QStringLiteral("menuArquivo"));

    acao_abrir_ = menu_arquivo_->addAction(actionIcon, QStringLiteral("Abrir Arquivo 7z..."));
    acao_abrir_->setObjectName(QStringLiteral("acaoAbrir"));

    menu_arquivo_->addSeparator();

    acao_sair_ = menu_arquivo_->addAction(actionIcon, QStringLiteral("Sair"));
    acao_sair_->setObjectName(QStringLiteral("acaoSair"));

    // Menu Configurações
    menu_configuracoes_ = menuBar()->addMenu(QStringLiteral("Configurações"));
    menu_configuracoes_->setObjectName(QStringLiteral("menuConfiguracoes"));

    menu_idioma_ = menu_configuracoes_->addMenu(QStringLiteral("Idioma"));
    menu_idioma_->setObjectName(QStringLiteral("menuIdioma"));

    grupo_idiomas_ = new QActionGroup(this);
    grupo_idiomas_->setExclusive(true);

    acao_pt_br_ = new QAction(QStringLiteral("Português-BR"), this);
    acao_pt_br_->setCheckable(true);
    acao_pt_br_->setData(QStringLiteral("pt_BR"));

    acao_en_us_ = new QAction(QStringLiteral("English-US"), this);
    acao_en_us_->setCheckable(true);
    acao_en_us_->setData(QStringLiteral("en_US"));

    grupo_idiomas_->addAction(acao_pt_br_);
    grupo_idiomas_->addAction(acao_en_us_);

    menu_idioma_->addAction(acao_pt_br_);
    menu_idioma_->addAction(acao_en_us_);

    // Menu Sobre
    menu_sobre_ = menuBar()->addMenu(QStringLiteral("Sobre"));
    menu_sobre_->setObjectName(QStringLiteral("menuSobre"));

    acao_sobre_sobre_ = menu_sobre_->addAction(actionIcon, QStringLiteral("Sobre"));
    acao_sobre_sobre_->setObjectName(QStringLiteral("acaoSobreSobre"));

    acao_sobre_leiame_ = menu_sobre_->addAction(actionIcon, QStringLiteral("Leia-me"));
    acao_sobre_leiame_->setObjectName(QStringLiteral("acaoSobreLeiame"));

    acao_sobre_manual_ = menu_sobre_->addAction(actionIcon, QStringLiteral("Manual"));
    acao_sobre_manual_->setObjectName(QStringLiteral("acaoSobreManual"));

    acao_sobre_nota_lancamento_ = menu_sobre_->addAction(actionIcon, QStringLiteral("Nota de Lançamento"));
    acao_sobre_nota_lancamento_->setObjectName(QStringLiteral("acaoSobreNotaLancamento"));

    acao_sobre_documentacao_ = menu_sobre_->addAction(actionIcon, QStringLiteral("Documentação"));
    acao_sobre_documentacao_->setObjectName(QStringLiteral("acaoSobreDocumentacao"));

    atualizar_menu_idiomas();
}

void MainWindow::setupConnections()
{
    connect(btn_selecionar_arquivo_, &QPushButton::clicked, this, &MainWindow::on_selecionar_arquivo_clicked);
    connect(btn_iniciar_extracao_, &QPushButton::clicked, this, &MainWindow::on_iniciar_extracao_clicked);
    connect(btn_cancelar_extracao_, &QPushButton::clicked, this, &MainWindow::on_cancelar_extracao_clicked);

    connect(acao_abrir_, &QAction::triggered, this, &MainWindow::on_selecionar_arquivo_clicked);
    connect(acao_sair_, &QAction::triggered, this, &QWidget::close);

    connect(acao_sobre_sobre_, &QAction::triggered, this, &MainWindow::on_action_sobre_clicked);
    connect(acao_sobre_leiame_, &QAction::triggered, this, &MainWindow::on_action_leiame_clicked);
    connect(acao_sobre_manual_, &QAction::triggered, this, &MainWindow::on_action_manual_clicked);
    connect(acao_sobre_nota_lancamento_, &QAction::triggered, this, &MainWindow::on_action_nota_lancamento_clicked);
    connect(acao_sobre_documentacao_, &QAction::triggered, this, &MainWindow::on_action_documentacao_clicked);

    connect(acao_pt_br_, &QAction::triggered, this, [this]() {
        if (gerenciador_traducao_) {
            gerenciador_traducao_->definir_idioma(QStringLiteral("pt_BR"));
        }
    });

    connect(acao_en_us_, &QAction::triggered, this, [this]() {
        if (gerenciador_traducao_) {
            gerenciador_traducao_->definir_idioma(QStringLiteral("en_US"));
        }
    });

    if (gerenciador_traducao_) {
        connect(gerenciador_traducao_, &source::language::GerenciadorTraducao::idioma_alterado,
                this, &MainWindow::retranslateUi);
    }
}

void MainWindow::atualizar_menu_idiomas()
{
    const QString idiomaAtual = gerenciador_traducao_ ? gerenciador_traducao_->obter_idioma_atual() : QStringLiteral("pt_BR");
    if (idiomaAtual == QLatin1String("en_US")) {
        acao_en_us_->setChecked(true);
    } else {
        acao_pt_br_->setChecked(true);
    }
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("Sliding Window LZ77 & LZMA Visualizer - Descompressor Multi-formato com Hooks"));

    if (menu_arquivo_) {
        menu_arquivo_->setTitle(tr("&Arquivo"));
    }
    if (acao_abrir_) {
        acao_abrir_->setText(tr("&Abrir Arquivo Compactado..."));
    }
    if (acao_sair_) {
        acao_sair_->setText(tr("&Sair"));
    }

    if (menu_configuracoes_) {
        menu_configuracoes_->setTitle(tr("&Configurações"));
    }
    if (menu_idioma_) {
        menu_idioma_->setTitle(tr("&Idioma"));
    }
    if (acao_pt_br_) {
        acao_pt_br_->setText(tr("Português-BR"));
    }
    if (acao_en_us_) {
        acao_en_us_->setText(tr("English-US"));
    }

    if (menu_sobre_) {
        menu_sobre_->setTitle(tr("&Sobre"));
    }
    if (acao_sobre_sobre_) {
        acao_sobre_sobre_->setText(tr("&Sobre"));
    }
    if (acao_sobre_leiame_) {
        acao_sobre_leiame_->setText(tr("&Leia-me"));
    }
    if (acao_sobre_manual_) {
        acao_sobre_manual_->setText(tr("&Manual"));
    }
    if (acao_sobre_nota_lancamento_) {
        acao_sobre_nota_lancamento_->setText(tr("&Nota de Lançamento"));
    }
    if (acao_sobre_documentacao_) {
        acao_sobre_documentacao_->setText(tr("&Documentação"));
    }

    auto *groupArquivo = findChild<QGroupBox*>(QStringLiteral("groupArquivo"));
    if (groupArquivo) {
        groupArquivo->setTitle(tr("Arquivo de Entrada Compactado"));
    }
    if (edit_caminho_arquivo_) {
        edit_caminho_arquivo_->setPlaceholderText(tr("Selecione um arquivo (.7z, .rar, .zip, .tar, .wim, .xz, .gz, etc.)..."));
    }
    if (btn_selecionar_arquivo_) {
        btn_selecionar_arquivo_->setText(tr("Procurar..."));
    }
    if (btn_iniciar_extracao_) {
        btn_iniciar_extracao_->setText(tr("Iniciar Extração com Hook"));
    }
    if (btn_cancelar_extracao_) {
        btn_cancelar_extracao_->setText(tr("Cancelar"));
    }

    auto *groupProgresso = findChild<QGroupBox*>(QStringLiteral("groupProgresso"));
    if (groupProgresso) {
        groupProgresso->setTitle(tr("Progresso da Extração"));
    }

    if (lbl_status_ && (lbl_status_->text() == QStringLiteral("Pronto.") || lbl_status_->text() == QStringLiteral("Ready."))) {
        lbl_status_->setText(tr("Pronto."));
    }

    auto *groupMetricas = findChild<QGroupBox*>(QStringLiteral("groupMetricas"));
    if (groupMetricas) {
        groupMetricas->setTitle(tr("Métricas de Compressão LZMA / LZ77"));
    }

    auto *lblTitleMatches = findChild<QLabel*>(QStringLiteral("lblTitleMatches"));
    if (lblTitleMatches) {
        lblTitleMatches->setText(tr("Total de Repetições (Matches):"));
    }
    auto *lblTitleLiterals = findChild<QLabel*>(QStringLiteral("lblTitleLiterals"));
    if (lblTitleLiterals) {
        lblTitleLiterals->setText(tr("Total de Literais:"));
    }
    auto *lblTitleSize = findChild<QLabel*>(QStringLiteral("lblTitleSize"));
    if (lblTitleSize) {
        lblTitleSize->setText(tr("Tamanho Descomprimido:"));
    }
    auto *lblTitleTime = findChild<QLabel*>(QStringLiteral("lblTitleTime"));
    if (lblTitleTime) {
        lblTitleTime->setText(tr("Tempo Decorrido:"));
    }

    auto *groupTabela = findChild<QGroupBox*>(QStringLiteral("groupTabela"));
    if (groupTabela) {
        groupTabela->setTitle(tr("Fluxo de Repetições (Ponteiros Curtos: Distância e Comprimento)"));
    }

    if (modelo_matches_) {
        modelo_matches_->retranslate();
    }

    atualizar_menu_idiomas();
}

void MainWindow::on_selecionar_arquivo_clicked()
{
    const QString caminho = QFileDialog::getOpenFileName(
        this,
        tr("Selecionar Arquivo Compactado"),
        edit_caminho_arquivo_->text().isEmpty() ? QStringLiteral("D:/TESTE") : edit_caminho_arquivo_->text(),
        tr("Todos os Arquivos Suportados (*.7z *.zip *.rar *.tar *.wim *.xz *.gz *.bz2 *.cab *.iso *.lzma *.zst *.tgz *.tbz2 *.txz);;"
           "Arquivos 7-Zip (*.7z);;"
           "Arquivos ZIP (*.zip);;"
           "Arquivos RAR (*.rar);;"
           "Arquivos TAR (*.tar *.tgz *.tbz2 *.txz);;"
           "Arquivos WIM (*.wim);;"
           "Arquivos XZ (*.xz);;"
           "Arquivos GZIP (*.gz);;"
           "Arquivos BZIP2 (*.bz2);;"
           "Arquivos CAB (*.cab);;"
           "Arquivos ISO (*.iso);;"
           "Arquivos LZMA (*.lzma);;"
           "Todos os Arquivos (*.*)")
    );

    if (!caminho.isEmpty()) {
        edit_caminho_arquivo_->setText(caminho);
        auto *logger = source::logs::LogManager::get_instance();
        logger->info(QStringLiteral("Arquivo selecionado: %1").arg(caminho));
    }
}

void MainWindow::on_iniciar_extracao_clicked()
{
    const QString caminho = edit_caminho_arquivo_->text().trimmed();
    if (caminho.isEmpty()) {
        QMessageBox::warning(this, tr("Aviso"), tr("Por favor, selecione um arquivo compactado válido."));
        return;
    }

    if (!QFile::exists(caminho)) {
        QMessageBox::critical(this, tr("Erro"), tr("O arquivo informado não existe:\n%1").arg(caminho));
        return;
    }

    btn_iniciar_extracao_->setEnabled(false);
    btn_selecionar_arquivo_->setEnabled(false);
    btn_cancelar_extracao_->setEnabled(true);

    barra_progresso_->setValue(0);
    lbl_status_->setText(tr("Inicializando descompressão com gancho LZMA..."));

    if (modelo_matches_) {
        modelo_matches_->clear();
    }
    total_matches_count_ = 0;
    total_literals_count_ = 0;

    lbl_total_matches_->setText(QStringLiteral("0"));
    lbl_total_literals_->setText(QStringLiteral("0"));
    lbl_uncompressed_size_->setText(QStringLiteral("0 bytes"));
    lbl_tempo_decorrido_->setText(QStringLiteral("0 ms"));

    extractor_thread_ = new LzmaExtractor(caminho, this);

    connect(extractor_thread_, &LzmaExtractor::progress, this, &MainWindow::on_extracao_progresso);
    connect(extractor_thread_, &LzmaExtractor::matchesBatch, this, &MainWindow::on_matches_batch);
    connect(extractor_thread_, &LzmaExtractor::extractionFinished, this, &MainWindow::on_extracao_finalizada);

    extractor_thread_->start();
}

void MainWindow::on_cancelar_extracao_clicked()
{
    if (extractor_thread_) {
        extractor_thread_->request_cancel();
        btn_cancelar_extracao_->setEnabled(false);
        lbl_status_->setText(tr("Cancelando extração..."));
    }
}

void MainWindow::on_extracao_progresso(int pct, const QString &status)
{
    barra_progresso_->setValue(pct);
    lbl_status_->setText(status);
}

void MainWindow::on_matches_batch(const QVector<LzmaMatchInfo> &batch)
{
    if (batch.isEmpty()) {
        return;
    }

    if (modelo_matches_) {
        modelo_matches_->appendMatches(batch);
        total_matches_count_ = modelo_matches_->totalMatchesCount();
    } else {
        total_matches_count_ += batch.size();
    }

    lbl_total_matches_->setText(QLocale().toString(static_cast<qulonglong>(total_matches_count_)));
}

void MainWindow::on_extracao_finalizada(bool success, const QString &message,
                                        quint64 totalMatches, quint64 totalLiterals,
                                        quint64 uncompressedSize, qint64 elapsedMs)
{
    btn_iniciar_extracao_->setEnabled(true);
    btn_selecionar_arquivo_->setEnabled(true);
    btn_cancelar_extracao_->setEnabled(false);

    total_matches_count_ = totalMatches;
    total_literals_count_ = totalLiterals;

    lbl_total_matches_->setText(QLocale().toString(static_cast<qulonglong>(totalMatches)));
    lbl_total_literals_->setText(QLocale().toString(static_cast<qulonglong>(totalLiterals)));

    QString sizeStr;
    if (uncompressedSize >= 1024 * 1024 * 1024) {
        sizeStr = QStringLiteral("%1 GB (%2 bytes)").arg(double(uncompressedSize) / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2).arg(QLocale().toString(static_cast<qulonglong>(uncompressedSize)));
    } else if (uncompressedSize >= 1024 * 1024) {
        sizeStr = QStringLiteral("%1 MB (%2 bytes)").arg(double(uncompressedSize) / (1024.0 * 1024.0), 0, 'f', 2).arg(QLocale().toString(static_cast<qulonglong>(uncompressedSize)));
    } else if (uncompressedSize >= 1024) {
        sizeStr = QStringLiteral("%1 KB (%2 bytes)").arg(double(uncompressedSize) / 1024.0, 0, 'f', 2).arg(QLocale().toString(static_cast<qulonglong>(uncompressedSize)));
    } else {
        sizeStr = QStringLiteral("%1 bytes").arg(uncompressedSize);
    }
    lbl_uncompressed_size_->setText(sizeStr);

    lbl_tempo_decorrido_->setText(QStringLiteral("%1 ms").arg(elapsedMs));

    if (success) {
        barra_progresso_->setValue(100);
        lbl_status_->setText(message);
        QMessageBox::information(
            this,
            tr("Extração Concluída"),
            QStringLiteral("%1\n\n%2: %3\n%4: %5\n%6: %7\n%8: %9ms")
                .arg(message)
                .arg(tr("Total de Repetições (Matches)"), QLocale().toString(static_cast<qulonglong>(totalMatches)))
                .arg(tr("Total de Literais"), QLocale().toString(static_cast<qulonglong>(totalLiterals)))
                .arg(tr("Tamanho Descomprimido"), sizeStr)
                .arg(tr("Tempo"), QString::number(elapsedMs))
        );
    } else {
        lbl_status_->setText(tr("Falha na extração: %1").arg(message));
        QMessageBox::critical(this, tr("Erro na Extração"), message);
    }
}

void MainWindow::on_action_sobre_clicked()
{
    DocumentViewerDialog dlg(DocumentType::About, gerenciador_traducao_, this);
    dlg.exec();
}

void MainWindow::on_action_leiame_clicked()
{
    DocumentViewerDialog dlg(DocumentType::Readme, gerenciador_traducao_, this);
    dlg.exec();
}

void MainWindow::on_action_manual_clicked()
{
    DocumentViewerDialog dlg(DocumentType::Manual, gerenciador_traducao_, this);
    dlg.exec();
}

void MainWindow::on_action_nota_lancamento_clicked()
{
    DocumentViewerDialog dlg(DocumentType::ReleaseNotes, gerenciador_traducao_, this);
    dlg.exec();
}

void MainWindow::on_action_documentacao_clicked()
{
    DocumentationDialog dlg(gerenciador_traducao_, this);
    dlg.exec();
}

