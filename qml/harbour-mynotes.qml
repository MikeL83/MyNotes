import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import "scripts/mynotesJS.js" as AppJS

ApplicationWindow
{

    initialPage: Component {
        HomePage {}
    }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    Component.onDestruction: myappsettings.callSync();

    onApplicationActiveChanged: {
        if (applicationActive && AppJS.NUMBEROFNOTIFICATIONS > 0) {
            console.log("jee")
        }
    }
}


