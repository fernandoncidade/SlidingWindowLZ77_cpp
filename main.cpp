#include <QApplication>
#include "source/interface/mainwindow.hpp"
#include "source/interface/AppIconHelper.hpp"
#include "source/locale/tr_01_gerenciadorTraducao.hpp"
#include "source/logs/LogManager.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize Application Icon
    app.setWindowIcon(source::interface::get_app_window_icon());

    // Initialize Logger
    auto *logger = source::logs::LogManager::get_instance();
    logger->info(QStringLiteral("=== Iniciando SlidingWindowLZ77 Visualizer & 7z Extractor ==="));

    // Initialize Translation Manager
    source::language::GerenciadorTraducao tradutor;
    tradutor.aplicar_traducao();

    // Create and show Main Window
    MainWindow window(&tradutor);
    window.show();

    int exitCode = app.exec();
    logger->info(QStringLiteral("=== Encerrando aplicativo (código %1) ===").arg(exitCode));
    return exitCode;
}