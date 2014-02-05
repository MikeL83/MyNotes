# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-mynotes

CONFIG += sailfishapp

QMAKE_CXXFLAGS += -std=c++0x

QT += sql

SOURCES += src/harbour-mynotes.cpp \
    src/mynotesdb.cpp \
    src/appsettings.cpp

OTHER_FILES += qml/harbour-mynotes.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-mynotes.spec \
    rpm/harbour-mynotes.yaml \
    harbour-mynotes.desktop \
    qml/pages/SetNotificationDialog.qml \
    qml/pages/RenameFolderDialog.qml \
    qml/pages/OptionsDialog.qml \
    qml/pages/NoteViewDialog.qml \
    qml/pages/NotesViewPage.qml \
    qml/pages/HomePage.qml \
    qml/pages/FoundNotesPage.qml \
    qml/pages/FoundFlaggedNotesPage.qml \
    qml/pages/FindNoteDialog.qml \
    qml/pages/AllNotesPage.qml \
    qml/pages/AddSubFolderDialog.qml \
    qml/pages/AddNoteDialog.qml \
    qml/pages/AddFolderDialog.qml \
    qml/pages/AboutPage.qml \
    qml/scripts/mynotesJS.js \
    qml/pages/components/Notification.qml

RESOURCES += \
    MyResource.qrc

HEADERS += \
    src/mynotesdb.h \
    src/error.h \
    src/appsettings.h

