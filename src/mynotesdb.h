#ifndef MYNOTESDB_H
#define MYNOTESDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QSharedPointer>
#include <QStringList>
#include <QVariantList>
#include "appsettings.h"

class QString;
class QSqlQuery;

class MyNotesDB : public QObject
{
    Q_OBJECT public : explicit MyNotesDB(QObject *parent = 0);

    Q_INVOKABLE QStringList getMainFolders() const;

    Q_INVOKABLE bool addFolder(const QString &) const;

    Q_INVOKABLE bool addSubFolder(const QString &, const QString &) const;

    Q_INVOKABLE void removeFolder(const QString &, const QString &) const;

    Q_INVOKABLE void deleteFolderContent(const QString &,
                                         const QString &) const;

    Q_INVOKABLE void addNewNote(const QString &, const QString &,
                                const QString &, int, const QString &) const;

    Q_INVOKABLE void deleteNote(const QString &, const QString &,
                                const QString &) const;
    Q_INVOKABLE void updateNote(const QString &, const QString &,
                                const QString &, const QString &,
                                const QString &, const QString &,
                                const QString &, QString foldername = "") const;
    //Q_INVOKABLE bool renameFolder(const QString &, const QString &) const;

    Q_INVOKABLE QStringList getSubfolders(const QString &foldername) const;

    Q_INVOKABLE QVariantList getFolderNotes(const QString &foldername) const;

    Q_INVOKABLE QVariantList getAllNotes() const;

    Q_INVOKABLE QVariantList getNoteData(const QString &, const QString &,
                                         QString foldername = "") const;

    Q_INVOKABLE QVariantList
    findNote(const QString &, const QString &, const QString &) const;

    Q_INVOKABLE QVariantList getFlaggedNotes() const;

    Q_INVOKABLE void deleteFlagFromNote(const QString &, const QString &,
                                        const QString &,
                                        QString foldername = "") const;
    Q_INVOKABLE QStringList getNoteDueDate(const QString &, const QString &,
                                           QString foldername = "") const;

    Q_INVOKABLE QVariantList checkDueDates() const;

  protected:
    QSqlQuery prepare(const QString& statement) const;
    bool execute(QSqlQuery &query) const;
    void createConnection();
    void createInitialData() const;
    void updateFoldersTable(const QString &, int issubfolder = 0,
                            QString parent = "root",
                            QString action = "add") const;
    QSqlDatabase db;
    const u_int32_t MagicNumber;
    const QVariant Invalid;
    QSharedPointer<AppSettings> appSettings;
};

#endif // MYNOTESDB_H
