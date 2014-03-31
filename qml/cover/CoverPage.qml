import QtQuick 2.0
import Sailfish.Silica 1.0
import "../scripts/mynotesJS.js" as AppJS

CoverBackground {
    id: cover
    property int number_of_nofications: 0
    property bool active: status === Cover.Active

    CoverActionList {
        id: coverAction
        CoverAction {
            iconSource: "image://theme/icon-cover-sync"
            onTriggered: {
                checkUpdates();
            }
        }
    }

    Timer {
        id: timer
        interval: 60000
        running: false
        repeat: true
        triggeredOnStart: false
        onTriggered: {
            checkUpdates();
        }
    }

    function checkUpdates() {
        if (listModel.count !== 0) listModel.clear()
        var duedates = [];
        duedates = mynotesdb.checkDueDates();
        if (duedates.length !== 0) {
            var i = 0, count = duedates.length;
            for (i; i < count; ++i) {
                listModel.append({"name": duedates[i][0], "duedate": duedates[i][1]})
            }
            number_of_nofications = count;

            AppJS.NUMBEROFNOTIFICATIONS = count;
        }
    }

    Image {
        id: icon
        horizontalAlignment: Image.AlignHCenter
        source: "qrc:/images/images/harbour-mynotes.png"
        smooth: true
        anchors {
            top: parent.top
            topMargin: Theme.paddingLarge
            horizontalCenter: parent.horizontalCenter
        }
    }

    Label {
        id: title
        anchors {
            centerIn: parent
        }
        text: "My Notes"
        color: Theme.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeLarge*1.25
    }


    SilicaListView {
        id: coverListView
        anchors.fill: parent
        clip: true
        focus: true
        model: ListModel{id: listModel}
        spacing: Theme.paddingMedium

        header: Row{
            id: header
            anchors {
                topMargin: Theme.paddingLarge
                left: parent.left
                leftMargin: Theme.paddingLarge
            }
            transform: Translate{y: 20}
            spacing: 10
                Label {
                id: label1
                text: String(number_of_nofications)
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSizeHuge
                font.bold: true
            }
                Label {
                id: label2
                text: "new\nnotifications\n\n"
                font.family: Theme.fontFamily
                font.pixelSize: Math.floor(label1.font.pixelSize/2.2)
                elide: Text.ElideRight
            }
            states: [
                State {
                    when: listModel.count !== 0
                    PropertyChanges {
                        target: header; visible: true
                    }
                    PropertyChanges {
                        target: title; opacity: 0.15
                    }
                    PropertyChanges {
                        target: icon; opacity: 0.15
                    }
                },
                State {
                    when: listModel.count === 0
                    PropertyChanges {
                        target: header; visible: false
                    }
                    PropertyChanges {
                        target: title; opacity: 1
                    }
                    PropertyChanges {
                        target: icon; opacity: 1
                    }
                }
            ]
        }

        delegate: coverDelegate

        VerticalScrollDecorator {
            flickable: listview
        }
    }
    Component {
        id: coverDelegate
        BackgroundItem {
            id: bgItem
            width: coverListView.width*0.9
            height: col.childrenRect.height + Theme.paddingSmall
            Column {
                id: col
                spacing: 5
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                }
                Text {
                    id: label
                    width: coverListView.width*0.9
                    text: name
                    font.pixelSize: Theme.fontSizeSmall
                    color: "#FF6633"
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
                Label {
                    id: date
                    text: duedate
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: "#FF6633"
                    font.bold: true
                }
            }
            Separator {
                id: separator
                anchors {
                    margins: 0
                    top: col.bottom
                    topMargin: 5
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }
                width: parent.width
                height: 1
                color: Theme.secondaryHighlightColor
            }
        }
    }
    Component.onCompleted: {
        timer.start();
    }

    onActiveChanged: {
        checkUpdates();
    }
}





