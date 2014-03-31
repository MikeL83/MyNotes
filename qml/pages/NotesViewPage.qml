import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: notesviewpage
    width: Screen.width
    height: Screen.height

    property string foldername: ""
    property var notes: []
    property var subfolders: []

    function updateListContent() {
        if (listModel.count !== 0) listModel.clear()
        var subfolders_ = [];
        subfolders_ = mynotesdb.getSubfolders(foldername);
        var notes_ = [];
        notes_ = mynotesdb.getFolderNotes(foldername);
        if (subfolders_.length !== 0) {
            var i = 0
            for (; i < subfolders_.length; ++i) {
                listModel.append({"name": subfolders_[i], "datenumber": "", "imagesource1": "image://theme/icon-m-folder",
                                  "imagesource2": "", "scale_factor": 0.5})
            }
        }
        var k = 0;
        var count = notes_.length
        if (count !== 0)
            for (; k < count; ++k) {
                if (parseInt(notes_[k][2])) {
                    listModel.append({"name": notes_[k][0], "datenumber": notes_[k][1].substring(0,notes_[k][1].indexOf(",")), "imagesource1": "qrc:/images/images/notepad.png",
                                      "imagesource2": "qrc:/images/images/red_flag.png", "scale_factor": 0.9})
                } else {
                    listModel.append({"name": notes_[k][0], "datenumber": notes_[k][1].substring(0,notes_[k][1].indexOf(",")),
                                      "imagesource1": "qrc:/images/images/notepad.png", "imagesource2": "", "scale_factor": 0.9})
                }
            }
        subfolders = subfolders_;
        notes = notes_;
    }

    SilicaListView {
        id: listview
        anchors.fill: parent
        clip: true
        focus: false
        model: ListModel{id: listModel}
        header: PageHeader {
            id: pageheader
            title: foldername
        }
        spacing: Theme.paddingLarge
        PullDownMenu {
            /*
            MenuItem {
                text: qsTr("Rename folder")
                onClicked: pageStack.push(Qt.resolvedUrl("RenameFolderDialog.qml"))
            }
            */
            MenuItem {
                text: qsTr("Delete folder content")
                onClicked:{
                    mynotesdb.deleteFolderContent(foldername,"");
                    pageStack.replace(Qt.resolvedUrl("NotesViewPage.qml"),{"foldername": foldername})
                }
            }
            MenuItem {
                text: qsTr("Create subfolder")
                onClicked: pageStack.push(Qt.resolvedUrl("AddSubFolderDialog.qml"),{"parentFolder": foldername})
            }
            MenuItem {
                text: qsTr("Create note")
                onClicked: pageStack.push(Qt.resolvedUrl("AddNoteDialog.qml"),{"foldername": foldername})
            }
        }
        delegate: folderDelegate

        ViewPlaceholder {
            id: viewHolder
            enabled: listModel.count === 0;
            text: qsTr("No notes available")
        }

        VerticalScrollDecorator {
            flickable: listview
        }
    }
    Component {
        id: folderDelegate
        BackgroundItem {
            id: bgItem
            width: listview.width
            height: Theme.itemSizeMedium
            Column {
                id: col
                spacing: 0
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    verticalCenter: parent.verticalCenter
                }
                Row {
                    id: row
                    spacing: 0
                    Image {
                        id: img1
                        anchors.margins: 0
                        anchors.verticalCenter: label.verticalCenter
                        source: imagesource1
                        scale: scale_factor
                        smooth: true

                        Component.onCompleted: {
                            if (scale < 0.7) {
                                img1.x -= 15;
                                label.x -= 10;
                                row.height -= date.height - Theme.paddingLarge;
                            } else {
                                label.x += 20;
                            }
                        }
                    }
                    Label {
                        id: label
                        text: name
                        font.pixelSize: Theme.fontSizeMedium
                        color: bgItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }
                }
                Label {
                    id: date
                    text: datenumber
                    font.pixelSize: Theme.fontSizeSmall
                    color: "#0066CC"
                    transform: Translate{x: img1.width + 20}
                }
            }
            Image {
                id: img2
                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                    bottom: col.bottom
                    bottomMargin: 5
                }
                source: imagesource2
                scale: 1.2
                smooth: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (datenumber !== "") {
                            mynotesdb.deleteFlagFromNote(name,notes[index - subfolders.length][1],String(0),foldername);
                            img2.source = "";
                            updateListContent();
                        }
                    }
                }
            }
            onClicked: {
                if (notes.length !== 0 && datenumber !== "") {
                    var idx = index - subfolders.length;
                    var list = []
                    list = mynotesdb.getNoteData(name,notes[idx][1],foldername)
                    if (list.length > 0)
                        pageStack.push(Qt.resolvedUrl("NoteViewDialog.qml"),{"notedata": list, "date": notes[idx][1],
                                           "foldername": foldername})
                } else if (datenumber === "") {
                    pageStack.push(Qt.resolvedUrl("NotesViewPage.qml"),{"foldername": name})
                }

            }
            onPressAndHold: {
                var idx = index - subfolders.length;
                if (datenumber === "") {
                    remorse.execute(bgItem,qsTr("Deleting..."), function() {mynotesdb.removeFolder(name,foldername);
                        listModel.remove(index)})
                } else {
                    remorse.execute(bgItem,qsTr("Deleting..."), function() {mynotesdb.deleteNote(foldername,name,notes[idx][1]);
                        listModel.remove(index)})
                }
            }
            RemorseItem { id: remorse;}
        }
    }
    onStatusChanged: {
        if (status === PageStatus.Activating) {
            updateListContent();
        }
    }
}
