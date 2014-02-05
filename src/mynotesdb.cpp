#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QSqlDriver>
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlQueryModel>
#include <QtGlobal>
#include <QString>
#include <QMessageLogger>
#include <QFile>
#include <QDate>
#include <QTime>
#include <QPair>
#include <QList>
#include <QMap>
#include <QStandardPaths>
#include <QDir>
#include "mynotesdb.h"
#include "error.h"

MyNotesDB::MyNotesDB(QObject *parent)
    : QObject(parent), MagicNumber(0xFA832109) {
    createConnection();
}

void MyNotesDB::createConnection() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString pathToDatabase = "";
    if (!QDir().exists(
            QStandardPaths::writableLocation(QStandardPaths::DataLocation))) {
        if (!QDir().mkpath(QStandardPaths::writableLocation(
                QStandardPaths::DataLocation))) {
            qWarning("Cannot create directory %s",
                     qPrintable(QStandardPaths::writableLocation(
                         QStandardPaths::DataLocation)));
        }
    } else {
        pathToDatabase =
            QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
            "/" + "_mynotes.db";
    }

    if (!pathToDatabase.isEmpty())
        db.setDatabaseName(pathToDatabase);

    if (!db.open())
        qFatal("Error while opening the database: %s", qPrintable(db.lastError().text()));

    createInitialData();
}

void MyNotesDB::createInitialData() const {
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS "
                  "Allnotes (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "title TEXT NOT NULL, "
                  "note CLOB NOT NULL, "
                  "date TEXT NOT NULL, "
                  "flagged INTEGER NOT NULL, "
                  "duedate TEXT NOT NULL)");
    if (!query.exec())
        qFatal("Failed to create table Allnotes\n%s",
               qPrintable(query.lastError().text()));
    if (db.tables().indexOf("Folders_" + QString::number(MagicNumber)) == -1) {
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Hobbies (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Hobbies\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Home (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Home\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Miscellaneous (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Miscellaneous\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Personal (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Personal\%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Shopping (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Shopping\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Tasks\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Travel (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Travel\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Work (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table Work\n%s",
                   qPrintable(query.lastError().text()));
        query.prepare("CREATE TABLE IF NOT EXISTS "
                      "Folders_" +
                      QString::number(MagicNumber) +
                      " (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT NOT NULL, "
                      "issubfolder INTEGER NOT NULL, "
                      "parent TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table folders\n%s",
                   qPrintable(query.lastError().text()));
        QSqlQueryModel model;
        model.setQuery("SELECT * FROM Folders_" + QString::number(MagicNumber));
        if (model.rowCount() == 0) {
            QStringList tmp = { "Hobbies", "Home", "Miscellaneous", "Personal",
                                "Shopping", "Tasks", "Travel", "Work" };
            QSqlQuery query;
            for (auto &value : tmp) {
                query.prepare("INSERT INTO Folders_" +
                              QString::number(MagicNumber) +
                              " (name, issubfolder, parent) "
                              "VALUES (:name, :issubfolder, :parent)");
                query.bindValue(":name", value);
                query.bindValue(":issubfolder", 0);
                query.bindValue(":parent", "root");
                if(!query.exec()) {
                    qWarning("Failed to insert table %s\n%s",
                             qPrintable(value),
                             qPrintable(query.lastError().text()));
                }
            }
        }
    }
}

QStringList MyNotesDB::getMainFolders() const {
    QSqlQuery query;
    query.prepare("SELECT name, issubfolder FROM Folders_" +
                  QString::number(MagicNumber));
    QStringList db_tables;
    if (!query.exec())
        qWarning("%s", qPrintable(query.lastError().text()));
    while (query.next()) {
        if (query.value(0) != Invalid && query.value(1) != Invalid &&
            query.value(1).toInt() == 0) {
            db_tables.append(query.value(0).toString());
        }
    }
    db_tables.sort();
    return db_tables;
}

bool MyNotesDB::addFolder(const QString &foldername) const {
    if (!db.tables().contains(foldername) && !foldername.startsWith("sqlite_")) {
        QSqlQuery query;
        query.prepare("CREATE TABLE IF NOT EXISTS'" + foldername +
                      "' (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table %s\n%s", qPrintable(foldername),
                     qPrintable(query.lastError().text()));
        updateFoldersTable(foldername);
    } else {
        qWarning(
            "Folder %s already exists. Choose another name for the folder.",
            qPrintable(foldername));
        return false;
    }
    return true;
}

bool MyNotesDB::addSubFolder(const QString &foldername,
                             const QString &parent) const {
    if (!db.tables().contains(foldername) && !foldername.startsWith("sqlite_")) {
        QSqlQuery query;
        query.prepare("CREATE TABLE IF NOT EXISTS'" + foldername +
                      "' (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "title TEXT NOT NULL, "
                      "note CLOB NOT NULL, "
                      "date TEXT NOT NULL, "
                      "flagged INTEGER NOT NULL, "
                      "duedate TEXT NOT NULL)");
        if (!query.exec())
            qFatal("Failed to create table %s", qPrintable(foldername));
        updateFoldersTable(foldername, 1, parent);
    } else {
        qWarning("There already exists a folder named %s. Please choose "
                 "another name for the subfolder.",
                 qPrintable(foldername));
        return false;
    }
    return true;
}

void MyNotesDB::deleteFolderContent(const QString &foldername,
                                    const QString &parent) const {
    if (!getSubfolders(foldername).isEmpty()) {
        for (auto &item : getSubfolders(foldername)) {
            deleteFolderContent(item, foldername);
        }
    }
    QSqlQuery query;
    query.prepare("DELETE FROM '" + foldername + "'");
    if (!query.exec()) {
        qWarning("Could not delete data from table %s.",
                 qPrintable(foldername));
    }

    if (!parent.isEmpty()) {
        QSqlQuery query;
        query.prepare("DELETE FROM Folders_" + QString::number(MagicNumber) +
                      " WHERE name ='" + foldername + "' AND parent ='" +
                      parent + "'");
        if (!query.exec()) {
            qWarning("Could not delete content from folder %s.",
                     qPrintable(foldername));
        }
        query.prepare("DROP TABLE IF EXISTS '" + foldername + "'");
        if (!query.exec()) {
            qWarning("Could not delete table %s.",
                     qPrintable(foldername));
        }
    }
}

void MyNotesDB::removeFolder(const QString &foldername,
                             const QString &parent) const {
    if (!getSubfolders(foldername).isEmpty()) {
        for (auto &item : getSubfolders(foldername)) {
            deleteFolderContent(item, foldername);
        }
    }
    QSqlQuery query;
    query.prepare("DELETE FROM '" + foldername + "'");
    if (!query.exec()) {
        qWarning("Could not delete data from table %s.",
                 qPrintable(foldername));
    }
    query.prepare("DELETE FROM Folders_" + QString::number(MagicNumber) +
                  " WHERE name ='" + foldername + "' AND parent ='" +
                  parent + "'");
    if (!query.exec()) {
        qWarning("Could not delete content from folder %s.",
                 qPrintable(foldername));
    }
    query.prepare("DROP TABLE IF EXISTS '" + foldername + "'");
    if (!query.exec()) {
        qWarning("Could not delete table %s.",
                 qPrintable(foldername));
    }

}

void MyNotesDB::updateFoldersTable(const QString &foldername, int issubfolder,
                                   QString parent, QString action) const {
    if (action.compare("add") == 0) {
        QSqlQuery query;
        query.prepare("INSERT INTO Folders_" + QString::number(MagicNumber) +
                      " (name, issubfolder, parent) "
                      "VALUES (:name, :issubfolder, :parent)");
        query.bindValue(":name", foldername);
        query.bindValue(":issubfolder", issubfolder);
        query.bindValue(":parent", parent);
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
    }
}

void MyNotesDB::addNewNote(const QString &foldername, const QString &title,
                           const QString &note, int flagged,
                           const QString &duedate) const {
    QSqlQuery query;
    query.prepare("INSERT INTO '" + foldername +
                  "' (title, note, date, flagged, duedate) "
                  "VALUES (:title, :note, :date, :flagged, :duedate)");
    query.bindValue(":title", title);
    query.bindValue(":note", note);
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();
    query.bindValue(
        ":date", d.toString("ddd d MMM yyyy") + ", " + t.toString("hh:mm:ss"));
    query.bindValue(":flagged", flagged);
    query.bindValue(":duedate", duedate);
    if (!query.exec())
        qWarning("Could not add a new note %s into folder %s\n%s",
                 qPrintable(title),
                 qPrintable(foldername),
                 qPrintable(query.lastError().text()));
}

void MyNotesDB::deleteNote(const QString &foldername, const QString &title,
                           const QString &date) const {
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query.prepare("DELETE FROM '" + foldername + "' WHERE title='" + title +
                      "' AND date='" + date + "'");
        if (!query.exec())
            qWarning("Could not delete note %s from folder %s\n%s",
                     qPrintable(title),
                     qPrintable(foldername),
                     qPrintable(query.lastError().text()));
    } else {
        QSqlQuery query;
        query.prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber));
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        while (query.next()) {
            QSqlQuery queryForNote;
            if (query.value(0) != Invalid) {
                queryForNote.prepare("DELETE FROM '" +
                                     query.value(0).toString() +
                                     "' WHERE title='" + title + "' AND " +
                                     " date='" + date + "'");
                if (!queryForNote.exec())
                    qWarning("Could not delete note %s from folder %s\n%s",
                             qPrintable(title),
                             qPrintable(foldername),
                             qPrintable(queryForNote.lastError().text()));
            }
        }
    }
}

QStringList MyNotesDB::getSubfolders(const QString &foldername) const {
    QSqlQuery query;
    query.prepare("SELECT name FROM Folders_" + QString::number(MagicNumber) +
                  " WHERE parent='" + foldername + "'");
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QStringList subfolders;
    while (query.next()) {
        if (query.value(0) != Invalid)
            subfolders.append(query.value(0).toString());
    }
    subfolders.sort();
    return subfolders;
}

QVariantList MyNotesDB::getFolderNotes(const QString &foldername) const {
    QSqlQuery query;
    query.prepare("SELECT title, date, flagged FROM '" + foldername + "'");
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QVariantList notes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        if (query.value(0) != Invalid && query.value(1) != Invalid &&
            query.value(2) != Invalid) {
            if (appSettings.readSortingOrder().compare("name") == 0) {
                map.insert(
                    query.value(0).toString(),
                    QVariant(QStringList() << query.value(0).toString()
                                           << query.value(1).toString()
                                           << query.value(2).toString()));
            } else {
                if (notes.count() == 0) {
                    notes.append(
                        QVariant(QStringList() << query.value(0).toString()
                                               << query.value(1).toString()
                                               << query.value(2).toString()));
                } else {
                    QString str1 = query.value(1).toString();
                    str1.truncate(str1.indexOf(","));
                    QDate dateNext = QDate::fromString(str1, "ddd d MMM yyyy");
                    QString str2 = query.value(1).toString();
                    QTime timeNext = QTime::fromString(
                        str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                    int idx = 0;
                    for (QVariant &item : notes) {
                        QString str1 = item.value<QStringList>().at(1);
                        str1.truncate(str1.indexOf(","));
                        QDate datePrev =
                            QDate::fromString(str1, "ddd d MMM yyyy");
                        QString str2 = item.value<QStringList>().at(1);
                        QTime timePrev = QTime::fromString(
                            str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                        if (dateNext > datePrev) {
                            break;
                        } else if (dateNext < datePrev) {
                            idx++;
                            continue;
                        } else if (dateNext == datePrev &&
                                   timeNext > timePrev) {
                            break;
                        } else if (dateNext == datePrev &&
                                   timeNext < timePrev) {
                            idx++;
                            continue;
                        }
                    }
                    notes.insert(idx, QVariant(QStringList()
                                               << query.value(0).toString()
                                               << query.value(1).toString()
                                               << query.value(2).toString()));
                }
            }
        }
    }
    if (map.values().count() > 0) {
        return map.values();
    } else {
        return notes;
    }
}

QVariantList MyNotesDB::getAllNotes() const {
    QSqlQuery query;
    query.prepare("SELECT name FROM Folders_" + QString::number(MagicNumber));
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QVariantList notes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        QSqlQuery queryForNotes;
        queryForNotes.prepare("SELECT title, date, flagged FROM '" +
                              query.value(0).toString() + "'");
        if (!queryForNotes.exec())
            throw MyNotes::Error(queryForNotes.lastError().text());
        while (queryForNotes.next()) {
            if (queryForNotes.value(0) != Invalid &&
                queryForNotes.value(1) != Invalid &&
                queryForNotes.value(2) != Invalid) {
                if (appSettings.readSortingOrder().compare("name") == 0) {
                    map.insert(queryForNotes.value(0).toString(),
                               QVariant(QStringList()
                                        << queryForNotes.value(0).toString()
                                        << queryForNotes.value(1).toString()
                                        << queryForNotes.value(2).toString()));
                } else {
                    if (notes.count() == 0) {
                        notes.append(
                            QVariant(QStringList()
                                     << queryForNotes.value(0).toString()
                                     << queryForNotes.value(1).toString()
                                     << queryForNotes.value(2).toString()));
                    } else {
                        QString str1 = queryForNotes.value(1).toString();
                        str1.truncate(str1.indexOf(","));
                        QDate dateNext =
                            QDate::fromString(str1, "ddd d MMM yyyy");
                        QString str2 = queryForNotes.value(1).toString();
                        QTime timeNext = QTime::fromString(
                            str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                        int idx = 0;
                        for (QVariant &item : notes) {
                            QString str1 = item.value<QStringList>().at(1);
                            str1.truncate(str1.indexOf(","));
                            QDate datePrev =
                                QDate::fromString(str1, "ddd d MMM yyyy");
                            QString str2 = item.value<QStringList>().at(1);
                            QTime timePrev = QTime::fromString(
                                str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                            if (dateNext > datePrev) {
                                break;
                            } else if (dateNext < datePrev) {
                                ++idx;
                                continue;
                            } else if (dateNext == datePrev &&
                                       timeNext > timePrev) {
                                break;
                            } else if (dateNext == datePrev &&
                                       timeNext < timePrev) {
                                ++idx;
                                continue;
                            }
                        }
                        notes.insert(
                            idx,
                            QVariant(QStringList()
                                     << queryForNotes.value(0).toString()
                                     << queryForNotes.value(1).toString()
                                     << queryForNotes.value(2).toString()));
                    }
                }
            }
        }
    }
    if (map.values().count() > 0) {
        return map.values();
    } else {
        return notes;
    }
}

QVariantList MyNotesDB::getNoteData(const QString &title, const QString &date,
                                    QString foldername) const {
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query.prepare("SELECT title, note, date, flagged FROM '" + foldername +
                      "' WHERE title='" + title + "' AND date='" + date + "'");
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        QVariantList notedata;
        query.next();
        if (query.value(0) != Invalid)
            notedata << query.value(0).toString() << query.value(1).toString()
                     << query.value(2).toString() << query.value(3).toInt();
        return notedata;
    } else {
        QSqlQuery query;
        query.prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber));
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        QVariantList notedata;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                QSqlQuery queryForNoteData;
                queryForNoteData.prepare(
                    "SELECT title, note, date, flagged FROM '" +
                    query.value(0).toString() + "' WHERE title='" + title +
                    "' AND date='" + date + "'");
                if (queryForNoteData.exec()) {
                    queryForNoteData.next();
                    if (queryForNoteData.isValid()) {
                        notedata << queryForNoteData.value(0).toString()
                                 << queryForNoteData.value(1).toString()
                                 << queryForNoteData.value(2).toString()
                                 << queryForNoteData.value(3).toString();
                        return notedata;
                    }
                }
            }
        }
        return notedata;
    }
}

QVariantList MyNotesDB::findNote(const QString &searchText,
                                 const QString &date_after,
                                 const QString &date_before) const {
    QSqlQuery query;
    query.prepare("SELECT name FROM Folders_" + QString::number(MagicNumber));
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QVariantList foundNotes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        QSqlQuery queryForNotes;
        if (query.value(0) != Invalid) {
            queryForNotes.prepare("SELECT title, note, date, flagged FROM '" +
                                  query.value(0).toString() + "'");
            if (!queryForNotes.exec())
                throw MyNotes::Error(queryForNotes.lastError().text());
            while (queryForNotes.next()) {
                if (queryForNotes.value(0) != Invalid) {
                    if (appSettings.readSortingOrder().compare("name") == 0) {
                        QString dateStr = queryForNotes.value(2).toString();
                        dateStr.truncate(dateStr.indexOf(","));
                        QVariant notes = QVariant(
                            QStringList() << queryForNotes.value(0).toString()
                                          << queryForNotes.value(1).toString()
                                          << queryForNotes.value(2).toString()
                                          << queryForNotes.value(3).toString());
                        if (date_after.compare("") != 0 &&
                            date_before.compare("") != 0) {
                            if (QDate::fromString(dateStr) >
                                    QDate::fromString(date_after,
                                                      "d MMM yyyy") &&
                                QDate::fromString(dateStr) <
                                    QDate::fromString(date_before,
                                                      "d MMM yyyy")) {
                                map.insert(queryForNotes.value(0).toString(),
                                           notes);
                            }
                        } else if (date_after.compare("") != 0 &&
                                   date_before.compare("") == 0) {
                            if (QDate::fromString(dateStr) >
                                QDate::fromString(date_after, "d MMM yyyy")) {
                                map.insert(queryForNotes.value(0).toString(),
                                           notes);
                            }
                        } else if (date_after.compare("") == 0 &&
                                   date_before.compare("") != 0) {
                            if (QDate::fromString(dateStr) <
                                QDate::fromString(date_before, "d MMM yyyy")) {
                                map.insert(queryForNotes.value(0).toString(),
                                           notes);
                            }
                        }
                        QString title = queryForNotes.value(0).toString();
                        if (!searchText.isEmpty()) {
                            if (date_after.compare("") == 0 &&
                                date_before.compare("") == 0) {
                                if (title.contains(QRegExp(
                                        searchText, Qt::CaseInsensitive))) {
                                    map.insert(
                                        queryForNotes.value(0).toString(),
                                        notes);
                                }
                            } else {
                                if (title.contains(QRegExp(
                                        searchText, Qt::CaseInsensitive))) {
                                    if (!map.contains(queryForNotes.value(0)
                                                          .toString()))
                                        map.insert(
                                            queryForNotes.value(0).toString(),
                                            notes);
                                } else {
                                    map.erase(--map.end());
                                }
                            }
                        }
                    } else {
                        int idx = 0;
                        if (foundNotes.count() > 0) {
                            QString str1 = queryForNotes.value(2).toString();
                            str1.truncate(str1.indexOf(","));
                            QDate dateNext =
                                QDate::fromString(str1, "ddd d MMM yyyy");
                            QString str2 = queryForNotes.value(2).toString();
                            QTime timeNext = QTime::fromString(
                                str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                            for (QVariant &item : foundNotes) {
                                QString str1 = item.value<QStringList>().at(2);
                                str1.truncate(str1.indexOf(","));
                                QDate datePrev =
                                    QDate::fromString(str1, "ddd d MMM yyyy");
                                QString str2 = item.value<QStringList>().at(2);
                                QTime timePrev = QTime::fromString(
                                    str2.mid(str2.indexOf(",") + 2),
                                    "hh:mm:ss");
                                if (dateNext > datePrev) {
                                    break;
                                } else if (dateNext < datePrev) {
                                    ++idx;
                                    continue;
                                } else if (dateNext == datePrev &&
                                           timeNext > timePrev) {
                                    break;
                                } else if (dateNext == datePrev &&
                                           timeNext < timePrev) {
                                    ++idx;
                                    continue;
                                }
                            }
                        }
                        QString dateStr = queryForNotes.value(2).toString();
                        dateStr.truncate(dateStr.indexOf(","));
                        QVariant notes = QVariant(
                            QStringList() << queryForNotes.value(0).toString()
                                          << queryForNotes.value(1).toString()
                                          << queryForNotes.value(2).toString()
                                          << queryForNotes.value(3).toString());
                        if (date_after.compare("") != 0 &&
                            date_before.compare("") != 0) {
                            if (QDate::fromString(dateStr) >
                                    QDate::fromString(date_after,
                                                      "d MMM yyyy") &&
                                QDate::fromString(dateStr) <
                                    QDate::fromString(date_before,
                                                      "d MMM yyyy")) {
                                foundNotes.insert(idx, notes);
                            }
                        } else if (date_after.compare("") != 0 &&
                                   date_before.compare("") == 0) {
                            if (QDate::fromString(dateStr) >
                                QDate::fromString(date_after, "d MMM yyyy")) {
                                foundNotes.insert(idx, notes);
                            }
                        } else if (date_after.compare("") == 0 &&
                                   date_before.compare("") != 0) {
                            if (QDate::fromString(dateStr) <
                                QDate::fromString(date_before, "d MMM yyyy")) {
                                foundNotes.insert(idx, notes);
                            }
                        }
                        QString title = queryForNotes.value(0).toString();
                        if (!searchText.isEmpty()) {
                            if (date_after.compare("") == 0 &&
                                date_before.compare("") == 0) {
                                if (title.contains(QRegExp(
                                        searchText, Qt::CaseInsensitive))) {
                                    foundNotes.insert(idx, notes);
                                }
                            } else {
                                if (title.contains(QRegExp(
                                        searchText, Qt::CaseInsensitive))) {
                                    if (!foundNotes.contains(notes))
                                        foundNotes.insert(idx, notes);
                                } else {
                                    foundNotes.removeLast();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (map.values().count() > 0) {
        return map.values();
    } else {
        return foundNotes;
    }
}

QVariantList MyNotesDB::getFlaggedNotes() const {
    QSqlQuery query;
    query.prepare("SELECT name FROM Folders_" + QString::number(MagicNumber));
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QVariantList foundFlaggedNotes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        QSqlQuery queryForFlaggedNotes;
        if (query.value(0) != Invalid) {
            queryForFlaggedNotes.prepare(
                "SELECT title, note, date, flagged FROM '" +
                query.value(0).toString() + "'");
            if (!queryForFlaggedNotes.exec())
                throw MyNotes::Error(queryForFlaggedNotes.lastError().text());
            while (queryForFlaggedNotes.next()) {
                if (queryForFlaggedNotes.value(3).toInt() != Invalid &&
                    queryForFlaggedNotes.value(3).toInt()) {
                    if (appSettings.readSortingOrder().compare("name") == 0) {
                        map.insert(
                            queryForFlaggedNotes.value(0).toString(),
                            QVariant(QStringList()
                                     << queryForFlaggedNotes.value(0).toString()
                                     << queryForFlaggedNotes.value(1).toString()
                                     << queryForFlaggedNotes.value(2)
                                            .toString()));
                    } else {
                        if (foundFlaggedNotes.count() == 0) {
                            foundFlaggedNotes.append(QVariant(
                                QStringList()
                                << queryForFlaggedNotes.value(0).toString()
                                << queryForFlaggedNotes.value(1).toString()
                                << queryForFlaggedNotes.value(2).toString()));
                        } else {
                            QString str1 =
                                queryForFlaggedNotes.value(2).toString();
                            str1.truncate(str1.indexOf(","));
                            QDate dateNext =
                                QDate::fromString(str1, "ddd d MMM yyyy");
                            QString str2 =
                                queryForFlaggedNotes.value(2).toString();
                            QTime timeNext = QTime::fromString(
                                str2.mid(str2.indexOf(",") + 2), "hh:mm:ss");
                            int idx = 0;
                            for (QVariant &item : foundFlaggedNotes) {
                                QString str1 = item.value<QStringList>().at(2);
                                str1.truncate(str1.indexOf(","));
                                QDate datePrev =
                                    QDate::fromString(str1, "ddd d MMM yyyy");
                                QString str2 = item.value<QStringList>().at(2);
                                QTime timePrev = QTime::fromString(
                                    str2.mid(str2.indexOf(",") + 2),
                                    "hh:mm:ss");
                                if (dateNext > datePrev) {
                                    break;
                                } else if (dateNext < datePrev) {
                                    ++idx;
                                    continue;
                                } else if (dateNext == datePrev &&
                                           timeNext > timePrev) {
                                    break;
                                } else if (dateNext == datePrev &&
                                           timeNext < timePrev) {
                                    ++idx;
                                    continue;
                                }
                            }
                            foundFlaggedNotes.insert(
                                idx,
                                QVariant(
                                    QStringList()
                                    << queryForFlaggedNotes.value(0).toString()
                                    << queryForFlaggedNotes.value(1).toString()
                                    << queryForFlaggedNotes.value(2)
                                           .toString()));
                        }
                    }
                }
            }
        }
    }
    if (map.values().count() > 0) {
        return map.values();
    } else {
        return foundFlaggedNotes;
    }
}

void MyNotesDB::updateNote(const QString &title, const QString &note,
                           const QString &date, const QString &flagged,
                           const QString &old_title, const QString &old_date,
                           const QString &duedate, QString foldername) const {
    Q_UNUSED(date)
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        QDate d = QDate::currentDate();
        QTime t = QTime::currentTime();
        if (!query.exec("UPDATE '" + foldername + "' SET title='" + title +
                        "', note='" + note + "', date='" +
                        d.toString("ddd d MMM yyyy") + ", " +
                        t.toString("hh:mm:ss") + "', flagged=" + flagged +
                        ", duedate='" + duedate + "' WHERE title='" +
                        old_title + "' AND date='" + old_date + "'"))
            qWarning("Could not update note %s in folder %s\n%s",
                     qPrintable(title),
                     qPrintable(foldername),
                     qPrintable(query.lastError().text()));
    } else {
        QSqlQuery query;
        query.prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber));
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        while (query.next()) {
            QSqlQuery queryToUpdateNoteData;
            if (query.value(0) != Invalid) {
                QDate d = QDate::currentDate();
                QTime t = QTime::currentTime();
                if (!queryToUpdateNoteData.exec(
                        "UPDATE '" + query.value(0).toString() +
                        "' SET title='" + title + "', note= '" + note +
                            "', date='" + d.toString("ddd d MMM yyyy") + ", " +
                        t.toString("hh:mm:ss") + "', flagged=" + flagged +
                        ", duedate='" + duedate + "' WHERE title='" +
                        old_title + "' AND date='" + old_date + "'"))
                    qWarning("Could not update note %s in folder %s\n%s",
                             qPrintable(title),
                             qPrintable(query.value(0).toString()),
                             qPrintable(queryToUpdateNoteData.lastError().text()));
            }
        }
    }
}

bool MyNotesDB::renameFolder(const QString &new_foldername,
                             const QString &old_foldername) const {
    if (new_foldername.compare("") != 0 && old_foldername.compare("") != 0 &&
        new_foldername.compare(old_foldername) != 0 &&
        db.tables().contains(old_foldername)) {
        if (!db.tables().contains(new_foldername)) {
            QSqlQuery query;
            if (!query.exec("ALTER TABLE '" + old_foldername + "' RENAME TO '" +
                            new_foldername + "'"))
                qWarning("Could not rename folder %s to %s\n%s",
                         qPrintable(old_foldername),
                         qPrintable(new_foldername),
                         qPrintable(query.lastError().text()));
            if (!query.exec("UPDATE Folders_" + QString::number(MagicNumber) +
                            " SET name = '" + new_foldername +
                            "' WHERE name = '" + old_foldername + "'"))
                qWarning("Could not update table after renaming folder %s\n%s",
                         qPrintable(old_foldername),
                         qPrintable(query.lastError().text()));
        } else {
            qWarning("There already exists a folder named %s. "
                     "Please choose another name for the folder.",
                 qPrintable(new_foldername));
            return false;
        }
    }
    return true;
}

void MyNotesDB::deleteFlagFromNote(const QString &title, const QString &date,
                                   const QString &flagged,
                                   QString foldername) const {
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        if (!query.exec("UPDATE '" + foldername + "' SET flagged='" + flagged +
                        "' WHERE title='" + title + "' AND date='" + date +
                        "'"))
            qWarning("Could not update note's %s flag value to %s",
                     qPrintable(title), qPrintable(flagged));
    } else {
        QSqlQuery query;
        query.prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber));
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        while (query.next()) {
            QSqlQuery queryToUpdateNoteFlagValue;
            if (query.value(0) != Invalid) {
                if (!queryToUpdateNoteFlagValue.exec(
                        "UPDATE '" + query.value(0).toString() +
                        "' SET flagged = " + flagged + " WHERE title = '" +
                        title + "' AND date = '" + date + "'"))
                    qWarning("Could not update note's %s flag value to %s",
                             qPrintable(title), qPrintable(flagged));
            }
        }
    }
}

QStringList MyNotesDB::getNoteDueDate(const QString &title, const QString &date,
                                      QString foldername) const {
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query.prepare("SELECT duedate FROM '" + foldername + "' WHERE title='" +
                      title + "' AND date='" + date + "'");
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        QStringList duedate;
        query.next();
        if (query.value(0) != Invalid && query.value(0).toString() != "") {
            QString date_ = query.value(0).toString();
            date_.truncate(date_.indexOf(";"));
            QString time_ = query.value(0).toString();
            time_ = time_.mid(time_.indexOf(";") + 1);
            duedate << date_ << time_;
        }

        return duedate;
    } else {
        QSqlQuery query;
        query.prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber));
        if (!query.exec())
            throw MyNotes::Error(query.lastError().text());
        QStringList duedate;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                QSqlQuery queryForDueDate;
                queryForDueDate.prepare(
                    "SELECT duedate FROM '" + query.value(0).toString() +
                    "' WHERE title='" + title + "' AND date='" + date + "'");
                if (queryForDueDate.exec()) {
                    queryForDueDate.next();
                    if (queryForDueDate.isValid() &&
                        queryForDueDate.value(0).toString() != "") {
                        QString date_ = queryForDueDate.value(0).toString();
                        date_.truncate(date_.indexOf(";"));
                        QString time_ = queryForDueDate.value(0).toString();
                        time_ = time_.mid(time_.indexOf(";") + 1);
                        duedate << date_ << time_;
                        return duedate;
                    }
                }
            }
        }
        return duedate;
    }
}

QVariantList MyNotesDB::checkDueDates() const {
    QSqlQuery query;
    query.prepare("SELECT name FROM Folders_" + QString::number(MagicNumber));
    if (!query.exec())
        throw MyNotes::Error(query.lastError().text());
    QVariantList duedates;
    while (query.next()) {
        if (query.value(0) != Invalid) {
            QSqlQuery queryForDueDates;
            queryForDueDates.prepare("SELECT title, date, duedate FROM '" +
                                     query.value(0).toString() + "'");
            if (!queryForDueDates.exec())
                throw MyNotes::Error(queryForDueDates.lastError().text());
            while (queryForDueDates.next()) {
                if (queryForDueDates.value(2) != Invalid &&
                    queryForDueDates.value(2).toString() != "") {
                    QString dateStr = queryForDueDates.value(2).toString();
                    dateStr.truncate(dateStr.indexOf(";"));
                    QString timeStr = queryForDueDates.value(2).toString();
                    timeStr = timeStr.mid(timeStr.indexOf(";") + 1);
                    QTime time = QTime().fromString(timeStr, "hh:mm");
                    QDate date = QDate().fromString(dateStr, "d MMM yyyy");
                    QDate today = QDate().currentDate();
                    if ((date == today && time >= QTime().currentTime()) ||
                        (date.addDays(-1) == today &&
                         time <= QTime().currentTime())) {
                        if (duedates.isEmpty()) {
                            duedates.append(QVariant(
                                QStringList()
                                << queryForDueDates.value(0).toString()
                                << QString("%1 %2").arg(dateStr).arg(timeStr)));
                        } else {
                            int k = 0;
                            for (QVariant &item : duedates) {
                                QString d = item.value<QStringList>().at(1);
                                d.truncate(
                                    d.indexOf(QRegExp("\\d{2}[:]{1}\\d{2}$")));
                                QDate dateInCont = QDate().fromString(
                                    d.trimmed(), "d MMM yyyy");
                                QString t = item.value<QStringList>().at(1);
                                QTime timeInCont = QTime().fromString(
                                    t.mid(t.indexOf(
                                        QRegExp("\\d{2}[:]{1}\\d{2}$"))),
                                    "hh:mm");
                                if (dateInCont > date) {
                                    break;
                                } else if (dateInCont < date) {
                                    k += 1;
                                    continue;
                                } else if (dateInCont == date) {
                                    if (timeInCont > time) {
                                        break;
                                    } else if (timeInCont < time) {
                                        k += 1;
                                        continue;
                                    }
                                }
                            }
                            duedates.insert(
                                k,
                                QVariant(QStringList()
                                         << queryForDueDates.value(0).toString()
                                         << QString("%1 %2").arg(dateStr)
                                                .arg(timeStr)));
                        }
                    } else if (date == today && time < QTime().currentTime()) {
                        QSqlQuery queryToUpdateNoteDueDateValue;
                        if (!queryToUpdateNoteDueDateValue.exec(
                                "UPDATE '" + query.value(0).toString() +
                                "' SET duedate = '" + "" + "' WHERE title = '" +
                                queryForDueDates.value(0).toString() +
                                "' AND date = '" +
                                queryForDueDates.value(1).toString() + "'"))
                            qWarning(
                                "Could not set note's %s duedate value to %s",
                                qPrintable(queryForDueDates.value(0)
                                               .toString()),
                                qPrintable("an empty value"));
                    }
                }
            }
        }
    }
    return duedates;
}
