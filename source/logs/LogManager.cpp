#include "source/logs/LogManager.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QThread>
#include <iostream>

namespace source::logs {
namespace {

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

}  // namespace

LogManager* LogManager::get_instance()
{
    static LogManager instance;
    return &instance;
}

LogManager::LogManager(QObject *parent)
    : QObject(parent)
{
    log_dir_path_ = QDir(app_data_dir()).filePath(QStringLiteral("logs"));
    QDir().mkpath(log_dir_path_);
    ensure_log_file_open();
}

LogManager::~LogManager()
{
    QMutexLocker locker(&mutex_);
    if (log_file_.isOpen()) {
        log_stream_.flush();
        log_file_.close();
    }
}

void LogManager::ensure_log_file_open()
{
    const QString today = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd"));
    if (log_file_.isOpen() && current_date_str_ == today) {
        return;
    }

    if (log_file_.isOpen()) {
        log_stream_.flush();
        log_file_.close();
    }

    current_date_str_ = today;
    const QString log_file_name = QStringLiteral("slidingwindowlz77_%1.log").arg(today);
    log_file_.setFileName(QDir(log_dir_path_).filePath(log_file_name));

    if (log_file_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        log_stream_.setDevice(&log_file_);
    }
}

QString LogManager::level_to_string(LogLevel level) const
{
    switch (level) {
        case LogLevel::Debug:   return QStringLiteral("DEBUG");
        case LogLevel::Info:    return QStringLiteral("INFO ");
        case LogLevel::Warning: return QStringLiteral("WARN ");
        case LogLevel::Error:   return QStringLiteral("ERROR");
    }
    return QStringLiteral("INFO ");
}

void LogManager::log(LogLevel level, const QString &message)
{
    QMutexLocker locker(&mutex_);
    ensure_log_file_open();

    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
    const QString thread_id = QString::asprintf("0x%p", QThread::currentThreadId());
    const QString formatted = QStringLiteral("[%1] [%2] [%3] %4")
                                  .arg(timestamp, level_to_string(level), thread_id, message);

    // Write to console
    std::cout << formatted.toLocal8Bit().constData() << std::endl;

    // Write to file
    if (log_file_.isOpen()) {
        log_stream_ << formatted << Qt::endl;
        log_stream_.flush();
    }

    emit message_logged(formatted, level);
}

void LogManager::debug(const QString &message)
{
    log(LogLevel::Debug, message);
}

void LogManager::info(const QString &message)
{
    log(LogLevel::Info, message);
}

void LogManager::warning(const QString &message)
{
    log(LogLevel::Warning, message);
}

void LogManager::error(const QString &message)
{
    log(LogLevel::Error, message);
}

QString LogManager::get_log_file_path() const
{
    QMutexLocker locker(&mutex_);
    return log_file_.fileName();
}

}  // namespace source::logs
