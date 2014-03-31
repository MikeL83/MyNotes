import QtQuick 2.0
import Sailfish.Silica 1.0

//OptionsDialog.qml

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

    property QtObject container
    property string duetime: ""
    property string duedate: ""

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
            Button {
                id: button1
                anchors.margins: 0
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Choose a time")

                onClicked: {
                    var dialog = pageStack.push("Sailfish.Silica.TimePickerDialog", {
                        hour: duetime !== "" ? parseInt(duetime.substring(0,duetime.indexOf(":"))) : 00,
                        minute: duetime !== "" ? parseInt(duetime.substring(duetime.indexOf(":")+1,duetime.length)) : 00,
                        hourMode: DateTime.TwentyFourHours
                    });
                    dialog.accepted.connect(function() {
                        button1.text = qsTr("Time set to: ") + dialog.timeText;
                        duetime = dialog.timeText;
                    });
                    dialog.rejected.connect(function() {
                        button1.text = qsTr("Choose a time");
                        duetime = "";
                    });
                }
            }

            Button {
                id: button2
                anchors.margins: 0
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Choose a date")
                onClicked: {
                    var dialog = pageStack.push("Sailfish.Silica.DatePickerDialog", {
                        date: duedate !== "" ? new Date(duedate) : new Date()
                    });
                    dialog.accepted.connect(function() {
                        button2.text = qsTr("Date set to: ") + dialog.dateText;
                        duedate = dialog.dateText;
                    })
                    dialog.rejected.connect(function() {
                        button2.text = qsTr("Choose a date")
                        duedate = "";
                    })
                }
            }
        }
    }
    onAccepted: {
        if (duetime !== "" && duedate !== "") {
            container._duetime = duetime;
            container._duedate = duedate;
        }
    }
    onOpened: {
        if (duetime !== "" && duedate !== ""){
            button1.text = qsTr("Time set to: ") + duetime;
            button2.text = qsTr("Date set to: ") + duedate;
        }
    }
}
