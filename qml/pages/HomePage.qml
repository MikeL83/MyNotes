import QtQuick 2.0
import Sailfish.Silica 1.0
//import io.thp.pyotherside 1.0

Page {
    id: mainpage
    width: Screen.width
    height: Screen.height

    SilicaListView {
        id: listview
        anchors.fill: parent
        clip: true
        focus: false
        model: ListModel{id: listModel}
        header: PageHeader {
            id: pageheader
            title: qsTr("My Notes")
        }
        spacing: Theme.paddingSmall
        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Create folder")
                onClicked: pageStack.push(Qt.resolvedUrl("AddFolderDialog.qml"))
            }
            MenuItem {
                text: qsTr("Find note")
                onClicked: pageStack.push(Qt.resolvedUrl("FindNoteDialog.qml"))
            }
            MenuItem {
                text: qsTr("Options")
                onClicked: pageStack.push(Qt.resolvedUrl("OptionsDialog.qml"))
            }
            MenuItem {
                text: qsTr("Rename folder")
                onClicked: pageStack.push(Qt.resolvedUrl("RenameFolderDialog.qml"))
            }
            MenuItem {
                text: qsTr("Show flagged")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("FoundFlaggedNotesPage.qml"))
                }
            }
        }
        delegate: folderDelegate   
    }
    Component {
        id: folderDelegate
        BackgroundItem {
            id: bgItem
            width: listview.width
            height: row.childrenRect.height
            Row {
                id: row
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingMedium
                }
                spacing: Theme.paddingSmall
                Image {
                    id: img
                    anchors.margins: 0
                    anchors.verticalCenter: label.verticalCenter
                    source: "image://theme/icon-m-folder"
                    scale: 0.6
                    smooth: true
                }
                Label {
                    id: label
                    text: name
                    font.pixelSize: Theme.fontSizeLarge
                }
            }
            onClicked: {
                if (name === qsTr("All notes")) {
                    pageStack.push(Qt.resolvedUrl("AllNotesPage.qml"))
                } else {
                    pageStack.push(Qt.resolvedUrl("NotesViewPage.qml"),{"foldername": name})
                }
            }
            Component.onCompleted: {
                if (name === qsTr("All notes")) {
                    img.opacity = 0;
                }
            }
            onPressAndHold: {
                if (name !== qsTr("All notes")) {
                    if (index > 0) {
                        remorse.execute(bgItem,qsTr("Deleting..."), function() {mynotesdb.removeFolder(name,"root");
                            listModel.remove(index)})
                    }
                }
            }
            RemorseItem { id: remorse;}
        }
    }
    onStatusChanged: {
        if (status === PageStatus.Activating) {
            if (listview.model.count !== 0) listview.model.clear()
            listview.model.append({"name": qsTr("All notes")})
            var tablenames = []
            tablenames = mynotesdb.getMainFolders();
            for (var i = 0; i < tablenames.length; ++i) {
                listview.model.append({"name": tablenames[i]})
            }
        }
    }
    VerticalScrollDecorator {
        flickable: listview
    }
}


