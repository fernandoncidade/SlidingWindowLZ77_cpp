#pragma once

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>

namespace source::logs {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class LogManager : public QObject {
    Q_OBJECT

public:
    static LogManager* get_instance();
    static LogManager* get_logger() { return get_instance(); }

    void log(LogLevel level, const QString &message);
    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

    QString get_log_file_path() const;

signals:
    void message_logged(const QString &formatted_message, LogLevel level);

private:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager() override;

    void ensure_log_file_open();
    QString level_to_string(LogLevel level) const;

    mutable QMutex mutex_;
    QFile log_file_;
    QTextStream log_stream_;
    QString log_dir_path_;
    QString current_date_str_;
};

}  // namespace source::logs

namespace source::utils {
using LogManager = source::logs::LogManager;
using LogLevel = source::logs::LogLevel;
}
