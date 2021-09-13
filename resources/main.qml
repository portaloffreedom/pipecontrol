import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.15 as Kirigami
import Pipewire 1.0


Kirigami.ApplicationWindow {
    id: window
//    height: 600
//    width: 800
    minimumWidth: page.implicitWidth + page.margins*2
    minimumHeight: page.implicitHeight + page.margins*2 + tabs.height*2
    //maximumWidth: minimumWidth
    //maximumHeight: minimumHeight
    visible: true
    title: "PipeControl "
//    icon: "qrc:/resources/pipecontrol.png"

    ColumnLayout {
        anchors.fill: parent

        TabBar {
            id: tabs
            width: parent.width
            Layout.fillWidth: true
            TabButton {
                text: qsTr("Top")
            }
            TabButton {
                text: qsTr("Settings")
            }
            TabButton {
                text: qsTr("About")
            }
        }

        StackLayout {
            id: page
            property int margins: 5
            width: parent.width
            height: parent.height - tabs.height
            currentIndex: tabs.currentIndex
            Layout.margins: margins

            Top {
                id: top
//                anchors.fill: parent
                Layout.minimumWidth: 300
            }

            Item {
                Layout.minimumWidth: 300
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Settings {
                    id: layout
                    anchors.fill: parent
                }
            }

            Text {
                text: "Work in progress by @portaloffreedom. License: GPLv3"
            }
        }
    }
}
