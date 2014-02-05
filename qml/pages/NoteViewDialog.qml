import QtQuick 2.0
import Sailfish.Silica 1.0

//TitleAndNoteInputDialog.qml

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

    property var notedata: []
    property string date: ""
    property string foldername: ""
    property string _duetime: ""
    property string _duedate: ""
    property var duedate: [];

    SilicaFlickable {
        id: view
        anchors.fill: parent
        clip: true
        focus: true

        PullDownMenu {
            id: pulleymenu
            MenuItem {
                text: qsTr("Set notification")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("SetNotificationDialog.qml"),
                                   {"container": dialog, "duedate": duedate[0], "duetime": duedate[1]})
                }
                Component.onCompleted: {
                    duedate = mynotesdb.getNoteDueDate(notedata[0], date, foldername);
                    if (duedate.length !== 0) {
                        text = qsTr("Change notification");
                    }
                }
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
                topMargin: Theme.paddingLarge
            }
            spacing: 5
            TextField {
                 id: titleField
                 focus: true
                 width: dialog.width
                 font.pixelSize: Theme.fontSizeLarge
                 placeholderText: qsTr("Add title...")
                 validator: RegExpValidator { regExp: /^[0-9\_\#\-A-Za-z\s]+$/ }
                 errorHighlight: text ? !acceptableInput : false
                 inputMethodHints: Qt.ImhNoPredictiveText
            }

            Slider {
                 id: flagged
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
                focus: true
                font.family: "Verdana"
                width: parent.width
                color: Theme.primaryColor
                wrapMode: TextEdit.Wrap
                placeholderText: qsTr("Note...")
                font.pixelSize: Theme.fontSizeMedium
            }
        }
    }
    onOpened: {
        if (notedata !== []) {
            titleField.text = notedata[0]
            textarea.text = notedata[1]
            flagged.value = parseInt(notedata[3])
        }
        textarea.font.pixelSize = myappsettings.readFontSize();
    }
    onAccepted: {
        if (date !== "") {
            if (_duedate !== "" && _duetime !== "") {
                mynotesdb.updateNote(titleField.text,textarea.text,date,
                                     String(flagged.value),notedata[0],
                                     notedata[2],_duedate + ";" + _duetime,foldername);
            } else {
                if (duedate[0] !== undefined && duedate[1] !== undefined) {
                    mynotesdb.updateNote(titleField.text,textarea.text,date,
                                         String(flagged.value),notedata[0],
                                         notedata[2],duedate[0] + ";" + duedate[1],foldername);
                } else {
                    mynotesdb.updateNote(titleField.text,textarea.text,date,
                                        String(flagged.value),notedata[0],
                                         notedata[2],"",foldername);
                }
            }
        }
    }
}
