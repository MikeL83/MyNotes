import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: messageBox
    property alias text: message_label.text
    property bool active: false
    TextArea {
        id: message_label
        horizontalAlignment: TextEdit.AlignHCenter
        font.underline: false
        color: Theme.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeExtraSmall
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
        onTriggered: {active = false;}
    }
}
