import QtQuick 2.0
import Sailfish.Silica 1.0
import "components"

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

    Notification {
        id: banner
        width: parent.width
        anchors.centerIn: parent

        onActiveChanged: {
            if (active === true) {
                canAccept = false;
            } else {
                canAccept = true;
            }
        }
    }

    SilicaFlickable {
        id: view
        anchors.fill: parent
        clip: true
        focus: true

        DialogHeader {
            id: dialogHeader
            width: parent.width
            anchors {
                topMargin: Theme.paddingMedium
                left: parent.left
            }
        }
        Column {
            anchors {
                top: dialogHeader.bottom
                topMargin: Theme.paddingMedium
            }
            width: dialog.width
            spacing: Theme.paddingLarge
            TextField {
                 id: currentFolder
                 focus: true
                 width: parent.width
                 font.pixelSize: Theme.fontSizeLarge
                 anchors.margins: Theme.paddingSmall
                 color: Theme.secondaryHighlightColor
                 placeholderText: qsTr("Current folder...")
                 label: qsTr("Old folder")
                 validator: RegExpValidator { regExp: /^[0-9\_\#A-Za-z\s]+$/ }
                 errorHighlight: text ? !acceptableInput : false
                 inputMethodHints: Qt.ImhNoPredictiveText
            }
            TextField {
                 id: newFolder
                 width: parent.width
                 font.pixelSize: Theme.fontSizeLarge
                 anchors.margins: Theme.paddingSmall
                 color: Theme.secondaryHighlightColor
                 placeholderText: qsTr("Rename folder...")
                 label: qsTr("New folder")
                 validator: RegExpValidator { regExp: /^[0-9\_\#A-Za-z\s]+$/ }
                 errorHighlight: text ? !acceptableInput : false
                 inputMethodHints: Qt.ImhNoPredictiveText
            }
        }
        states: [
            State {
                name: "show view"
                when: banner.active
                PropertyChanges {
                    target: view; opacity: 0.25
                }
            },
            State {
                name: "hide view"
                when: !banner.active
                PropertyChanges {
                    target: view; opacity: 1
                }
            }
        ]
    }
    onAccepted: {
        if (currentFolder.text.trim() !== "" && currentFolder.text !== qsTr("All notes")) {
            if (newFolder.text.trim() === "") {
                mynotesdb.renameFolder(qsTr("New Folder"), currentFolder.text)
            } else {
                if (!mynotesdb.renameFolder(newFolder.text, currentFolder.text)) {
                    banner.active = true;
                    banner.text = qsTr("Folder " + newFolder.text + " already exists.\n" +
                                  "Please choose another name for the folder.");
                }
            }
        }
    }
}
