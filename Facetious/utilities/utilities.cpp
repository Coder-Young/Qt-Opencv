#include "utilities.h"

#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QtGlobal>

QString Utilities::getDataPath()
{
    QString user_pictures_path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    QDir movie_dir(user_pictures_path);
    movie_dir.mkpath("Facetious");
    return movie_dir.absoluteFilePath("Facetious");
}

QString Utilities::newSavedPhotoName()
{
    QDateTime time = QDateTime::currentDateTime();
    return time.toString("yyyy-MM-dd HH-mm-ss");
}

QString Utilities::getSavedPhotoPath(QString name, QString postfix)
{
    QString finalPath = QString("%1/%2.%3").arg(Utilities::getDataPath(), name, postfix);

#ifdef Q_OS_WIN
    finalPath = finalPath.replace("/", "\\");
#endif

    return finalPath;
}
