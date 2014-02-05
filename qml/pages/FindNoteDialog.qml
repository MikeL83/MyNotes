import QtQuick 2.0
import Sailfish.Silica 1.0
import "../scripts/mynotesJS.js" as JS

//FindNoteDialog.qml

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

    property string date_after: ""
    property string date_before: ""


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
            horizontalCenter: parent.horizontalCenter
        }
        width: dialog.width
        spacing: Theme.paddingLarge
        TextField {
             id: searchField
             focus: true
             anchors.margins: 0
             width: dialog.width
             placeholderText: qsTr("Search text...")
             validator: RegExpValidator { regExp: /^[0-9\_\#\-A-Za-z\s]+$/ }
             errorHighlight: text ? !acceptableInput : false
             inputMethodHints: Qt.ImhNoPredictiveText
             font.pixelSize: Theme.fontSizeLarge
        }
        Button {
            id: button1
            anchors.margins: 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Choose a date to search before:")
            onClicked: {
                var dialog = pageStack.push("Sailfish.Silica.DatePickerDialog", {})
                dialog.accepted.connect(function() {
                    button1.text = qsTr("You selected: ") + dialog.dateText
                    date_before = dialog.dateText;

                })
                dialog.rejected.connect(function() {
                    button1.text = qsTr("Choose a date to search before:")
                    date_before = "";
                })
            }
        }
        Button {
            id: button2
            anchors.margins: 0
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Choose a date to search after:")
            onClicked: {
                var dialog = pageStack.push("Sailfish.Silica.DatePickerDialog", {})
                dialog.accepted.connect(function() {
                    button2.text = qsTr("You selected: ") + dialog.dateText
                    date_after = dialog.dateText
                })
                dialog.rejected.connect(function() {
                    button2.text = qsTr("Choose a date to search after:");
                    date_after = "";
                })
            }
        }
    }
    onAccepted: {
        searchField.text = searchField.text !== "" ? searchField.text : "";
        JS.search_text = searchField.text;
        JS.date_after_value = date_after;
        JS.date_before_value = date_before;
        pageStack.push(Qt.resolvedUrl("FoundNotesPage.qml"));
    }
    onRejected: pageStack.navigateBack()
}
