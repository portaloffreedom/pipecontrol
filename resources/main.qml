import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import Pipewire 1.0


Kirigami.ApplicationWindow {
    id: root
    visible: true
    title: "PipeControl"

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("Top")
                icon.name: "source-playlist"
                onTriggered: root.pageStack.replace("qrc:/resources/Top.qml", {
                    root: root
                })
            },
            Kirigami.Action {
                text: i18n("Settings")
                icon.name: "settings-configure"
                onTriggered: root.pageStack.replace("qrc:/resources/Settings.qml", {
                    root: root
                })
            },
            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-about-symbolic"
                onTriggered: aboutSheet.open()
            },
            Kirigami.Action {
                text: i18n("Quit")
                icon.name: "window-close"
                shortcut: StandardKey.Quit
                onTriggered: Qt.quit()
            }
        ]
    }

    Kirigami.OverlaySheet {
        id: aboutSheet
        width: aboutlabel.leftPadding + aboutlabel.contentWidth + aboutlabel.rightPadding + 40 // 40 extra necessary, unknown reason
        parent: applicationWindow().overlay
        header: Kirigami.Heading {
            text: "About PipeControl"
        }
        RowLayout {
            Controls.Label {
                id: aboutlabel
                text: "Pipecontrol v" + Pipewire.appVersion +
                    "\nWork in progress by @portaloffreedom" +
                    "\nLicense: GPLv3" +
                    "\n" +
                    "\n Pipewire compiled version: " + Pipewire.pipewireCompiledVersion() +
                    "\n Pipewire linked version: " + Pipewire.pipewireLinkedVersion() +
                    "\n Qt compiled version: " + Pipewire.qtCompiledVersion() +
                    "\n Qt linked version: " + Pipewire.qtLinkedVersion() +
                    "\n KF5 compiled version: " + Pipewire.kframeworksCompiledVersion() +
                    "\n Platform: " + Pipewire.platformName()
            }
        }
    }


    // Initial page to be loaded on app load
    pageStack.initialPage: Top {
        root: root
    }
}
