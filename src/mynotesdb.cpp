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
#include <QDebug>
#include "mynotesdb.h"
#include "error.h"

MyNotesDB::MyNotesDB(QObject *parent) : QObject(parent), MagicNumber(0xFA832109)
{
    appSettings = QSharedPointer<AppSettings>(new AppSettings);
    createConnection();
}

void MyNotesDB::createConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString databaseDir =
        QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    const QString dbFileName = QString("_mynotes.db");
    QDir dir(databaseDir);
    if (!QDir().mkpath(databaseDir)) {
        qWarning("Cannot create directory %s",
                 qPrintable(QStandardPaths::writableLocation(
                     QStandardPaths::DataLocation)));
        return;
    }
    //db.setDatabaseName(dir.absoluteFilePath(dbFileName));
    db.setDatabaseName(":memory:");
    if (!db.open())
        qFatal("Error while opening the database: %s",
               qPrintable(db.lastError().text()));

    createInitialData();
}

QSqlQuery MyNotesDB::prepare(const QString& statement) const
{
    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query.prepare(statement)) {
        qWarning() << Q_FUNC_INFO << "failed to prepare query";
        qWarning() << query.lastQuery();
        qWarning() << query.lastError().text();
        return QSqlQuery();
    }
    return query;
}

bool MyNotesDB::execute(QSqlQuery &query) const
{
    if (!query.exec()) {
        qWarning() << Q_FUNC_INFO << "failed execute query";
        qWarning() << query.lastQuery();
        qWarning() << query.lastError().text();
        return false;
    }
    return true;
}

void MyNotesDB::createInitialData() const
{
    QSqlQuery query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                              "Allnotes (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "title TEXT NOT NULL, "
                              "note CLOB NOT NULL, "
                              "date TEXT NOT NULL, "
                              "flagged INTEGER NOT NULL, "
                              "duedate TEXT NOT NULL);"));
    execute(query);
    if (db.tables().indexOf("Folders_" + QString::number(MagicNumber)) == -1) {
        QSqlQuery query =
            prepare(QString("CREATE TABLE IF NOT EXISTS "
                    "Hobbies (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "title TEXT NOT NULL, "
                    "note CLOB NOT NULL, "
                    "date TEXT NOT NULL, "
                    "flagged INTEGER NOT NULL, "
                    "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Home (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Miscellaneous (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Personal (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Shopping (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Travel (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Work (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);"));
        execute(query);
        query = prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Folders_") +
                        QString::number(MagicNumber) +
                        QString(" (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "name TEXT NOT NULL, "
                        "issubfolder INTEGER NOT NULL, "
                        "parent TEXT NOT NULL);"));
        execute(query);
        QSqlQueryModel model;
        model.setQuery("SELECT * FROM Folders_" + QString::number(MagicNumber) +
                       ";");
        if (model.rowCount() == 0) {
            QStringList tmp = { "Hobbies",  "Home",     "Miscellaneous",
                                "Personal", "Shopping", "Tasks",
                                "Travel",   "Work" };
            QSqlQuery query;
            for (auto &value : tmp) {
                query = prepare(QString("INSERT INTO Folders_") +
                                QString::number(MagicNumber) +
                                QString(" (name, issubfolder, parent) "
                                "VALUES (:name, :issubfolder, :parent);"));
                query.bindValue(":name", value);
                query.bindValue(":issubfolder", 0);
                query.bindValue(":parent", "root");
                execute(query);
            }
        }
    }
}

QStringList MyNotesDB::getMainFolders() const
{
    QSqlQuery query;
    query = prepare(QString("SELECT name, issubfolder FROM Folders_%1;")
                    .arg(QString::number(MagicNumber)));
    QStringList db_tables;
    if (!execute(query))
        return db_tables;
    while (query.next()) {
        if (query.value(0) != Invalid && query.value(1) != Invalid &&
            query.value(1).toInt() == 0) {
            db_tables.append(query.value(0).toString());
        }
    }
    db_tables.sort();
    return db_tables;
}

bool MyNotesDB::addFolder(const QString &foldername) const
{
    if (!db.tables().contains(foldername) &&
        !foldername.startsWith("sqlite_")) {
        QSqlQuery query;
        query = prepare("CREATE TABLE IF NOT EXISTS'" + foldername +
                        "' (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);");
        execute(query);
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
                             const QString &parent) const
{
    if (!db.tables().contains(foldername) &&
        !foldername.startsWith("sqlite_")) {
        QSqlQuery query;
        query = prepare("CREATE TABLE IF NOT EXISTS'" + foldername +
                        "' (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "note CLOB NOT NULL, "
                        "date TEXT NOT NULL, "
                        "flagged INTEGER NOT NULL, "
                        "duedate TEXT NOT NULL);");
        if (execute(query)) {
            updateFoldersTable(foldername, 1, parent);
        } else {
            return false;
        }
    } else {
        qWarning("There already exists a folder named %s. Please choose "
                 "another name for the subfolder.",
                 qPrintable(foldername));
        return false;
    }
    return true;
}

void MyNotesDB::deleteFolderContent(const QString &foldername,
                                    const QString &parent) const
{
    if (!getSubfolders(foldername).isEmpty()) {
        for (auto &item : getSubfolders(foldername)) {
            deleteFolderContent(item, foldername);
        }
    }
    QSqlQuery query;
    query = prepare("DELETE FROM '" + foldername + "';");
    execute(query);

    if (!parent.isEmpty()) {
        QSqlQuery query;
        query = prepare("DELETE FROM Folders_" + QString::number(MagicNumber) +
                        " WHERE name = ? AND parent = ?;");
        query.bindValue(0, foldername);
        query.bindValue(1, parent);
        execute(query);
        query = prepare("DROP TABLE IF EXISTS '" + foldername + "';");
        execute(query);
    }
}

void MyNotesDB::removeFolder(const QString &foldername,
                             const QString &parent) const
{
    if (!getSubfolders(foldername).isEmpty()) {
        for (auto &item : getSubfolders(foldername)) {
            deleteFolderContent(item, foldername);
        }
    }
    QSqlQuery query;
    query = prepare("DELETE FROM '" + foldername + "';");
    execute(query);
    query = prepare("DELETE FROM Folders_" + QString::number(MagicNumber) +
                    " WHERE name = ? AND parent = ?;");
    query.bindValue(0, foldername);
    query.bindValue(1, parent);
    execute(query);
    query = prepare("DROP TABLE IF EXISTS '" + foldername + "';");
    execute(query);
}

void MyNotesDB::updateFoldersTable(const QString &foldername, int issubfolder,
                                   QString parent, QString action) const
{
    if (action.compare("add") == 0) {
        QSqlQuery query;
        query = prepare("INSERT INTO Folders_" + QString::number(MagicNumber) +
                        " (name, issubfolder, parent) "
                        "VALUES (:name, :issubfolder, :parent);");
        query.bindValue(":name", foldername);
        query.bindValue(":issubfolder", issubfolder);
        query.bindValue(":parent", parent);
        execute(query);
    }
}

void MyNotesDB::addNewNote(const QString &foldername, const QString &title,
                           const QString &note, int flagged,
                           const QString &duedate) const
{
    QSqlQuery query = prepare(QString("INSERT INTO %1 "
                    "(title, note, date, flagged, duedate) "
                    "VALUES (?,?,?,?,?);").arg(foldername));
    query.bindValue(0, title);
    query.bindValue(1, note);
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();
    query.bindValue(2, d.toString("ddd d MMM yyyy") + QString(", ") +
                                 t.toString("hh:mm:ss"));
    query.bindValue(3, flagged);
    query.bindValue(4, duedate);
    execute(query);
}

void MyNotesDB::deleteNote(const QString &foldername, const QString &title,
                           const QString &date) const
{
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query = prepare("DELETE FROM '" + foldername + "' WHERE title = ? "
                                                       "AND date = ?;");
        query.bindValue(0, title);
        query.bindValue(1, date);
        execute(query);
    } else {
        QSqlQuery query;
        query = prepare("SELECT name FROM Folders_" +
                        QString::number(MagicNumber) + ";");
        if (!execute(query))
            return;
        QSqlQuery queryForNote;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                queryForNote =
                    prepare("DELETE FROM '" + query.value(0).toString() +
                            "' WHERE title = ? AND date = ?;");
                queryForNote.bindValue(0, title);
                queryForNote.bindValue(1, date);
                execute(queryForNote);
            }
        }
    }
}

QStringList MyNotesDB::getSubfolders(const QString &foldername) const
{
    QSqlQuery query;
    query = prepare(QString("SELECT name FROM Folders_%1 WHERE parent = ?;")
                    .arg(QString::number(MagicNumber)));
    query.bindValue(0, foldername);
    QStringList subfolders;
    if (!execute(query))
        return subfolders;
    while (query.next()) {
        if (query.value(0) != Invalid)
            subfolders.append(query.value(0).toString());
    }
    subfolders.sort();
    return subfolders;
}

QVariantList MyNotesDB::getFolderNotes(const QString &foldername) const
{
    QSqlQuery query = prepare(QString("SELECT title, date, flagged FROM %1;").arg(foldername));
    QVariantList notes;
    if (!execute(query))
        return notes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        if (query.value(0) != Invalid && query.value(1) != Invalid &&
            query.value(2) != Invalid) {
            if (appSettings->readSortingOrder().compare("name") == 0) {
                map.insert(query.value(0).toString(),
                           QVariant(QStringList()
                                    << query.value(0).toString()
                                    << query.value(1).toString()
                                    << query.value(2).toString()));
            } else {
                if (notes.count() == 0) {
                    notes.append(QVariant(QStringList()
                                          << query.value(0).toString()
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

QVariantList MyNotesDB::getAllNotes() const
{
    QSqlQuery query;
    query = prepare(QString("SELECT name FROM Folders_%1;").arg(QString::number(MagicNumber)));
    QVariantList notes;
    if (!execute(query))
        return notes;
    QMap<QString, QVariant> map;
    QSqlQuery queryForNotes;
    while (query.next()) {
        queryForNotes = prepare(QString("SELECT title, date, flagged FROM %1;").arg(query.value(0).toString()));
        if (!execute(queryForNotes))
            return notes;
        while (queryForNotes.next()) {
            if (queryForNotes.value(0) != Invalid &&
                queryForNotes.value(1) != Invalid &&
                queryForNotes.value(2) != Invalid) {
                if (appSettings->readSortingOrder().compare("name") == 0) {
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
                                    QString foldername) const
{
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query = prepare("SELECT title, note, date, flagged FROM '" +
                        foldername + "' WHERE title = ? AND date = ?;");
        query.bindValue(0, title);
        query.bindValue(1, date);
        QVariantList notedata;
        if (!execute(query))
            return notedata;
        query.next();
        if (query.value(0) != Invalid)
            notedata << query.value(0).toString() << query.value(1).toString()
                     << query.value(2).toString() << query.value(3).toInt();
        return notedata;
    } else {
        QSqlQuery query;
        query = prepare("SELECT name FROM Folders_" +
                        QString::number(MagicNumber) + ";");
        QVariantList notedata;
        if (!execute(query))
            return notedata;
        QSqlQuery queryForNoteData;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                queryForNoteData =
                    prepare("SELECT title, note, date, flagged FROM '" +
                            query.value(0).toString() +
                            "' WHERE title = ? AND date = ?;");
                queryForNoteData.bindValue(0, title);
                queryForNoteData.bindValue(1, date);
                if (execute(queryForNoteData)) {
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
                                 const QString &date_before) const
{
    QSqlQuery query;
    query = prepare("SELECT name FROM Folders_" + QString::number(MagicNumber) +
                    ";");
    QVariantList foundNotes;
    if (!execute(query))
        return foundNotes;
    QMap<QString, QVariant> map;
    while (query.next()) {
        QSqlQuery queryForNotes;
        if (query.value(0) != Invalid) {
            queryForNotes = prepare("SELECT title, note, date, flagged FROM '" +
                                    query.value(0).toString() + "';");
            if (!execute(queryForNotes))
                return foundNotes;
            while (queryForNotes.next()) {
                if (queryForNotes.value(0) != Invalid) {
                    if (appSettings->readSortingOrder().compare("name") == 0) {
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
                                    if (!map.contains(
                                             queryForNotes.value(0).toString()))
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

QVariantList MyNotesDB::getFlaggedNotes() const
{
    QSqlQuery query;
    query = prepare("SELECT name FROM Folders_" + QString::number(MagicNumber) +
                    ";");
    QVariantList foundFlaggedNotes;
    if (!execute(query))
        return foundFlaggedNotes;
    QMap<QString, QVariant> map;
    QSqlQuery queryForFlaggedNotes;
    while (query.next()) {
        if (query.value(0) != Invalid) {
            queryForFlaggedNotes =
                prepare("SELECT title, note, date, flagged FROM '" +
                        query.value(0).toString() + "';");
            if (!execute(queryForFlaggedNotes))
                return foundFlaggedNotes;
            while (queryForFlaggedNotes.next()) {
                if (queryForFlaggedNotes.value(3).toInt() != Invalid &&
                    queryForFlaggedNotes.value(3).toInt()) {
                    if (appSettings->readSortingOrder().compare("name") == 0) {
                        map.insert(
                            queryForFlaggedNotes.value(0).toString(),
                            QVariant(
                                QStringList()
                                << queryForFlaggedNotes.value(0).toString()
                                << queryForFlaggedNotes.value(1).toString()
                                << queryForFlaggedNotes.value(2).toString()));
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
                           const QString &duedate, QString foldername) const
{
    Q_UNUSED(date)
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        QDate d = QDate::currentDate();
        QTime t = QTime::currentTime();
        query = prepare("UPDATE '" + foldername + "' SET title = ?, note = ?,"
                                                  " date = ?, flagged = ?, "
                                                  "duedate = ? WHERE title = ? "
                                                  "AND date = ?;");
        query.bindValue(0, title);
        query.bindValue(1, note);
        query.bindValue(2, d.toString("ddd d MMM yyyy") + ", " +
                               t.toString("hh:mm:ss"));
        query.bindValue(3, flagged);
        query.bindValue(4, duedate);
        query.bindValue(5, old_title);
        query.bindValue(6, old_date);
        execute(query);
    } else {
        QSqlQuery query;
        query = prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber) + ";");
        if (!execute(query))
            return;
        QSqlQuery queryToUpdateNoteData;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                QDate d = QDate::currentDate();
                QTime t = QTime::currentTime();
                queryToUpdateNoteData =
                    prepare("UPDATE '" + query.value(0).toString() +
                            "' SET title = ?, note = ?,"
                            " date = ?, flagged = ?, duedate = ? WHERE title = "
                            "? AND date = ?;");
                queryToUpdateNoteData.bindValue(0, title);
                queryToUpdateNoteData.bindValue(1, note);
                queryToUpdateNoteData.bindValue(
                    2, d.toString("ddd d MMM yyyy") + ", " +
                           t.toString("hh:mm:ss"));
                queryToUpdateNoteData.bindValue(3, flagged);
                queryToUpdateNoteData.bindValue(4, duedate);
                queryToUpdateNoteData.bindValue(5, old_title);
                queryToUpdateNoteData.bindValue(6, old_date);
                execute(queryToUpdateNoteData);
            }
        }
    }
}
/*
bool MyNotesDB::renameFolder(const QString &new_foldername,
                             const QString &old_foldername) const
{
    if (new_foldername.compare("") != 0 && old_foldername.compare("") != 0 &&
        new_foldername.compare(old_foldername) != 0 &&
        db.tables().contains(old_foldername)) {
        if (!db.tables().contains(new_foldername)) {
            QSqlQuery query;
            query = prepare("ALTER TABLE '" + old_foldername + "' RENAME TO '" +
                            new_foldername + "';");
            if (!execute(query))
                return false;
            query = prepare("UPDATE Folders_" + QString::number(MagicNumber) +
                            " SET name = ? WHERE name = ?;");
            query.bindValue(0, new_foldername);
            query.bindValue(1, old_foldername);
            if (!execute(query))
                return false;
            if (!query.exec("ALTER TABLE '" + old_foldername + "' RENAME TO '" +
                            new_foldername + "';"))
                qWarning("Could not rename folder %s to %s\n%s",
                         qPrintable(old_foldername), qPrintable(new_foldername),
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
*/
void MyNotesDB::deleteFlagFromNote(const QString &title, const QString &date,
                                   const QString &flagged,
                                   QString foldername) const
{
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query = prepare("UPDATE '" + foldername + "' SET flagged = ?'"
                        " WHERE title = ? AND date = ?;");
        query.bindValue(0, title);
        query.bindValue(1, date);
        execute(query);
        /*
        if (!query.exec("UPDATE '" + foldername + "' SET flagged='" + flagged +
                        "' WHERE title='" + title + "' AND date='" + date +
                        "'"))
            qWarning("Could not update note's %s flag value to %s",
                     qPrintable(title), qPrintable(flagged));
        */
    } else {
        QSqlQuery query;
        query = prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber) + ";");
        if (!execute(query))
            return;
        QSqlQuery queryToUpdateNoteFlagValue;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                queryToUpdateNoteFlagValue = prepare("UPDATE '" + query.value(0).toString() +
                                                     "' SET flagged = ? WHERE title = ?'"
                                                     " AND date = ?;");
                queryToUpdateNoteFlagValue.bindValue(0, flagged);
                queryToUpdateNoteFlagValue.bindValue(1, title);
                queryToUpdateNoteFlagValue.bindValue(2, date);
                execute(queryToUpdateNoteFlagValue);
                /*
                if (!queryToUpdateNoteFlagValue.exec(
                         "UPDATE '" + query.value(0).toString() +
                         "' SET flagged = " + flagged + " WHERE title = '" +
                         title + "' AND date = '" + date + "'"))
                    qWarning("Could not update note's %s flag value to %s",
                             qPrintable(title), qPrintable(flagged));
                */
            }
        }
    }
}

QStringList MyNotesDB::getNoteDueDate(const QString &title, const QString &date,
                                      QString foldername) const
{
    if (foldername.compare("") != 0) {
        QSqlQuery query;
        query = prepare("SELECT duedate FROM '" + foldername + "' WHERE title = ?"
                        " AND date = ?;");
        query.bindValue(0, title);
        query.bindValue(1, date);
        QStringList duedate;
        if (!execute(query))
            return duedate;
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
        query = prepare("SELECT name FROM Folders_" +
                      QString::number(MagicNumber) + ";");
        QStringList duedate;
        if (!execute(query))
            return duedate;
        QSqlQuery queryForDueDate;
        while (query.next()) {
            if (query.value(0) != Invalid) {
                queryForDueDate = prepare(
                    "SELECT duedate FROM '" + query.value(0).toString() +
                    "' WHERE title = ? AND date = ?;");
                queryForDueDate.bindValue(0, title);
                queryForDueDate.bindValue(1, date);
                if (execute(queryForDueDate)) {
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

QVariantList MyNotesDB::checkDueDates() const
{
    QSqlQuery query;
    query = prepare("SELECT name FROM Folders_" + QString::number(MagicNumber) + ";");
    QVariantList duedates;
    if (!execute(query))
        return duedates;
    QSqlQuery queryForDueDates;
    while (query.next()) {
        if (query.value(0) != Invalid) {
            queryForDueDates = prepare("SELECT title, date, duedate FROM '" +
                                       query.value(0).toString() + "';");
            if (!execute(queryForDueDates))
                return duedates;
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
                    if ((date == today && time > QTime().currentTime()) ||
                        (date.addDays(-1) == today &&
                         time < QTime().currentTime())) {
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
                                         << QString("%1 %2").arg(dateStr).arg(
                                                timeStr)));
                        }
                    } else if ((date == today && time < QTime().currentTime()) ||
                               date < today) {
                        QSqlQuery queryToUpdateNoteDueDateValue;
                        queryToUpdateNoteDueDateValue = prepare("UPDATE '" + query.value(0).toString() +
                                                                "' SET duedate = ?"
                                                                " WHERE title = ? AND date = ?;");
                        queryToUpdateNoteDueDateValue.bindValue(0, "");
                        queryToUpdateNoteDueDateValue.bindValue(1, queryForDueDates.value(0).toString());
                        queryToUpdateNoteDueDateValue.bindValue(2, queryForDueDates.value(1).toString());
                        execute(queryToUpdateNoteDueDateValue);

                        /*
                        if (!queryToUpdateNoteDueDateValue.exec(
                                 "UPDATE '" + query.value(0).toString() +
                                 "' SET duedate = '" + "" +
                                 "' WHERE title = '" +
                                 queryForDueDates.value(0).toString() +
                                 "' AND date = '" +
                                 queryForDueDates.value(1).toString() + "'"))
                            qWarning(
                                "Could not set note's %s duedate value to %s",
                                qPrintable(
                                    queryForDueDates.value(0).toString()),
                                qPrintable("an empty value"));
                        */
                    }
                }
            }
        }
    }
    return duedates;
}
