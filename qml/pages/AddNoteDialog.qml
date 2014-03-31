import QtQuick 2.0
import Sailfish.Silica 1.0

//AddNoteDialog.qml

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

    property string foldername: ""
    property string _duetime: ""
    property string _duedate: ""

    SilicaFlickable {
        id: view
        anchors.fill: parent
        clip: true

        PullDownMenu {
            MenuItem {
                text: qsTr("Set notification")
                onClicked: pageStack.push(Qt.resolvedUrl("SetNotificationDialog.qml"), {"container": dialog})
            }
        }

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
            spacing: Theme.paddingSmall
            TextField {
                 id: titleField
                 focus: true
                 width: dialog.width
                 placeholderText: qsTr("Add title...")
                 label: qsTr("Note title")
                 validator: RegExpValidator { regExp: /^[0-9\_\#\-A-Za-z\s]+$/ }
                 errorHighlight: text ? !acceptableInput : false
                 inputMethodHints: Qt.ImhNoPredictiveText
                 font.pixelSize: Theme.fontSizeLarge
            }

            Slider {
                 id: flagged
                 anchors.margins: 0
                 anchors.horizontalCenter: parent.horizontalCenter
                 width: parent.width/2
                 minimumValue: 0
                 maximumValue: 1
                 value: 0
                 stepSize: 1
                 valueText: value ? "|" : "O"
                 label: qsTr("Set flag")
            }

            Separator {
                anchors.margins: 0
                width: parent.width
                height: 2
                color: Theme.secondaryHighlightColor
            }

            TextArea {
                id: textarea
                font.family: "Verdana"
                width: dialog.width
                color: Theme.primaryColor
                wrapMode: TextEdit.Wrap
                placeholderText: qsTr("Note...")
                font.pixelSize: Theme.fontSizeMedium
            }
        }
    }
    onAccepted: {
        if (foldername !== "") {
            if (_duetime !== "" && _duedate !== "") {
                mynotesdb.addNewNote(foldername,titleField.text,textarea.text,flagged.value,_duedate + ";" + _duetime)
            } else {
                mynotesdb.addNewNote(foldername,titleField.text,textarea.text,flagged.value,"")
            }
        }
    }
    onOpened: {
        textarea.font.pixelSize = myappsettings.readFontSize()
    }
}
