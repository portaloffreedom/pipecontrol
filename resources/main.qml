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
    visible: true
    title: "PipeControl "

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
                Layout.minimumWidth: 300
            }

            Item {
                Layout.fillHeight: false
                Layout.fillWidth: true
                implicitHeight: settings.implicitHeight
                implicitWidth: settings.implicitWidth
                Settings {
                    id: settings
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                text: "Pipecontrol v" + Pipewire.appVersion +
                      "\nWork in progress by @portaloffreedom." +
                      "\nLicense: GPLv3" +
                      "\n" +
                      "\n Pipewire compiled version: " + Pipewire.pipewireCompiledVersion() +
                      "\n Pipewire linked version: " + Pipewire.pipewireLinkedVersion() +
                      "\n Qt compiled version: " + Pipewire.qtCompiledVersion() +
                      "\n Qt linked version: " + Pipewire.qtLinkedVersion() +
                      "\n Platform: " + Pipewire.platformName()
            }
        }
    }
}
