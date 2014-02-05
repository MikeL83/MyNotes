import QtQuick 2.0
import Sailfish.Silica 1.0

//OptionsDialog.qml

Dialog {
    id: dialog
    width: Screen.width
    height: Screen.height

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
                horizontalCenter: parent.horizontalCenter
            }
            width: dialog.width
            spacing: Theme.paddingLarge
            ComboBox {
                id: sortOrder
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
                width: parent.width
                label: "Sort notes by"
                value: ""
                menu: ContextMenu {
                            MenuItem { text: "name"}
                            MenuItem { text: "date"}

                            onActivated: sortOrder.value = sortOrder.currentItem.text
                            closeOnActivation: sortOrder.value = sortOrder.currentItem.text
                       }

            }

            Slider {
                 id: fontSize
                 anchors {
                     leftMargin: Theme.paddingLarge
                     rightMargin: Theme.paddingLarge
                 }

                 width: parent.width
                 minimumValue: 20
                 maximumValue: 40
                 stepSize: 2
                 value: myappsettings.readFontSize()
                 valueText: value
                 label: qsTr("Set font size for the note")
            }
        }
    }
    onAccepted: {
        myappsettings.writeSortOrder(sortOrder.value);
        myappsettings.writeFontSize(fontSize.value);
    }
    onOpened: {
        sortOrder.value = myappsettings.readSortingOrder()
    }

}
