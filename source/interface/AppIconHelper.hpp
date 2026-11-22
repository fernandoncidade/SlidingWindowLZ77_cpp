#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QString>
#include <QStringList>

namespace source::interface {

inline QString find_app_icon_path(const QString &fileName) {
    const QStringList candidate_paths = {
        QCoreApplication::applicationDirPath() + QStringLiteral("/assets/icons/%1").arg(fileName),
        QCoreApplication::applicationDirPath() + QStringLiteral("/source/assets/icons/%1").arg(fileName),
        QCoreApplication::applicationDirPath() + QStringLiteral("/icones/%1").arg(fileName),
        QDir::current().filePath(QStringLiteral("assets/icons/%1").arg(fileName)),
        QDir::current().filePath(QStringLiteral("source/assets/icons/%1").arg(fileName)),
        QDir::current().filePath(QStringLiteral("icones/%1").arg(fileName)),
        QStringLiteral("C:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/source/assets/icons/%1").arg(fileName)
    };

    for (const QString &path : candidate_paths) {
        if (QFileInfo::exists(path)) {
            return QFileInfo(path).absoluteFilePath();
        }
    }

    return QString();
}

inline QIcon get_app_window_icon() {
    const QString icoPath = find_app_icon_path(QStringLiteral("lz77.ico"));
    if (!icoPath.isEmpty()) {
        return QIcon(icoPath);
    }
    const QString pngPath = find_app_icon_path(QStringLiteral("lz77.png"));
    if (!pngPath.isEmpty()) {
        return QIcon(pngPath);
    }
    return QIcon();
}

inline QIcon get_app_button_icon() {
    const QString pngPath = find_app_icon_path(QStringLiteral("lz77.png"));
    if (!pngPath.isEmpty()) {
        return QIcon(pngPath);
    }
    const QString icoPath = find_app_icon_path(QStringLiteral("lz77.ico"));
    if (!icoPath.isEmpty()) {
        return QIcon(icoPath);
    }
    return QIcon();
}

} // namespace source::interface
