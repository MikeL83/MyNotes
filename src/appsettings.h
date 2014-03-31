#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSharedPointer>

class QString;
class QSettings;

class AppSettings : public QObject
{
    Q_OBJECT
  public:
    explicit AppSettings(QObject *parent = 0);

    Q_INVOKABLE QString readSortingOrder() const;
    Q_INVOKABLE int readFontSize() const;

    Q_INVOKABLE void writeSortOrder(const QString &);
    Q_INVOKABLE void writeFontSize(const int);

    Q_INVOKABLE void callSync();

  private:
    QSharedPointer<QSettings> settings;
};

#endif // APPSETTINGS_H
