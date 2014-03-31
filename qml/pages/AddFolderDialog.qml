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
        dialogType: "folder"

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
            TextField {
                 id: foldername
                 focus: true
                 width: parent.width
                 font.pixelSize: Theme.fontSizeLarge
                 anchors.margins: Theme.paddingSmall
                 color: "steelblue"
                 placeholderText: qsTr("Add folder name...")
                 label: qsTr("Folder name")
                 validator: RegExpValidator { regExp: /^[0-9\_\#\-A-Za-z\s]+$/ }
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

    onDone: {
        if (foldername.text !== "")
            if (!mynotesdb.addFolder(foldername.text)) {
                banner.active = true;
                banner.text = qsTr("Folder %1 already exists.\n" +
                                   "Please choose another name for the folder.").arg(foldername.text)
            } else {
                banner.active = false;
            }

    }
}
