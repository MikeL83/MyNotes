import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: flaggednotesviewpage
    width: Screen.width
    height: Screen.height

    property var notes: []

    function updateListContent() {
        if (listModel.count !== 0) listModel.clear()
        var notes_ = [];
        notes_ = mynotesdb.getFlaggedNotes();
        if (notes_.length > 0) {
            var i = 0;
            var len = notes_.length;
            for (i; i < len; ++i) {
                listModel.append({"name": notes_[i][0],"datenumber": notes_[i][2].substring(0,notes_[i][2].indexOf(",")),
                                  "imagesource1": "qrc:/images/images/notepad.png",
                                  "imagesource2": "qrc:/images/images/red_flag.png"});
            }
        }
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
            title: qsTr("Flagged notes")
        }
        spacing: Theme.paddingLarge
        delegate: folderDelegate

        ViewPlaceholder {
            id: viewHolder
            enabled: listModel.count === 0
            text: qsTr("No flagged notes found")
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
                }
                Row {
                    id: row
                    spacing: 0
                    Image {
                        id: img1
                        anchors.verticalCenter: label.verticalCenter
                        source: imagesource1
                        scale: 0.9
                        smooth: true
                    }
                    Label {
                        id: label
                        text: name
                        font.pixelSize: Theme.fontSizeMedium
                        transform: Translate{x:20}
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
                            mynotesdb.deleteFlagFromNote(name,notes[index][2],String(0));
                            img2.source = "";
                            updateListContent();
                        }
                    }
                }
            }
            onClicked: {
                if (notes !== []) {
                    var list = [];
                    list = mynotesdb.getNoteData(name,notes[index][2]);
                    if (list.length > 0)
                        pageStack.push(Qt.resolvedUrl("NoteViewDialog.qml"),{"notedata": list, "date": notes[index][2]});
                }
            }
            onPressAndHold: {
                remorse.execute(bgItem,qsTr("Deleting..."), function() {mynotesdb.deleteNote("",name,notes[index][2]);
                    listModel.remove(index)});
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
