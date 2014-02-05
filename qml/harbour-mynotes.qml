import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    initialPage: Component {
        HomePage {}
    }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    Component.onDestruction: myappsettings.callSync();
}

