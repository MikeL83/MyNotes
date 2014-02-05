#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QSettings>
#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) : QObject(parent) {
    QCoreApplication::setOrganizationName("MyNotesSoftware");
    QCoreApplication::setApplicationName("MyNotes");

    settings = QSharedPointer<QSettings>(new QSettings);

    if (!settings->contains("sort/sortOrder"))
        settings->setValue("sort/sortOrder", "name");
    if (!settings->contains("font/fontSize"))
        settings->setValue("font/fontSize", 30);
}

QString AppSettings::readSortingOrder() const {
    return settings->value("sort/sortOrder").toString();
}

void AppSettings::writeSortOrder(const QString &order) {
    settings->setValue("sort/sortOrder", order);
}

int AppSettings::readFontSize() const {
    return settings->value("font/fontSize").toInt();
}

void AppSettings::writeFontSize(const int size) {
    settings->setValue("font/fontSize", size);
}

void AppSettings::callSync() {
    settings->sync();
}
