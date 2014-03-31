import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: messageBox
    property alias text: message_label.text
    property bool active: false
    property var dialogType;
    property string parent_: ""

    Text {
        id: message_label
        width: parent.width*0.9
        anchors.centerIn: parent
        horizontalAlignment: TextEdit.AlignHCenter
        font.underline: false
        color: Theme.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeLarge
        font.bold: true
        wrapMode: Text.WordWrap
        z: 10
    }
    states: [
        State {
            name: "show notification"
            when: messageBox.active
            PropertyChanges {
                target: message_label; visible: true
            }
        },
        State {
            name: "hide notification"
            when: !messageBox.active
            PropertyChanges {
                target: message_label; visible: false
            }
        }
    ]
    transitions: Transition {
            from: "*"
            to: "*"
            NumberAnimation { property: "visible"
                              duration: 500; easing.type: Easing.InOutQuad }
            }
    Timer {
        id: timer
        interval: 5000
        repeat: false
        running: active
        onTriggered: {active = false;
            if (dialogType === "folder") {
                pageStack.replace(Qt.resolvedUrl("../AddFolderDialog.qml"))
            } else if (dialogType === "subfolder") {
                pageStack.replace(Qt.resolvedUrl("../AddSubFolderDialog.qml"),{"parentFolder": parent_})
            }
        }
    }
}
