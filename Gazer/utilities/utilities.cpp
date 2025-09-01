#include "utilities.h"

#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QtGlobal>

QString Utilities::getDataPath()
{
    QString user_movie_path = QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)[0];
    QDir movie_dir(user_movie_path);
    movie_dir.mkpath("Gazer");
    return movie_dir.absoluteFilePath("Gazer");
}

QString Utilities::newSavedVideoName()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd HH-mm-ss"); // HH:mm:ss改为HH-mm-ss，文件名中不允许:符号
}

QString Utilities::getSavedVideoPath(QString name, QString postfix)
{
    QString finalPath = QString("%1/%2.%3").arg(Utilities::getDataPath(), name, postfix);

#ifdef Q_OS_WIN
    finalPath = finalPath.replace("/", "\\");
#endif

    return finalPath;
}
