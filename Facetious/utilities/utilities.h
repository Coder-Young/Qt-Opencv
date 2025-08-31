#ifndef UTILITIES_H
#define UTILITIES_H

class QString;
class Utilities
{
public:
   static QString getDataPath();
   static QString newSavedPhotoName();
   static QString getSavedPhotoPath(QString name, QString postfix);
};

#endif // UTILITIES_H
