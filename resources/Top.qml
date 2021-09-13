import QtQuick 2.15
import QtQuick.Layouts 1.12
import Pipewire 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.14 as Kirigami


ColumnLayout {

    RowLayout {
        id: header
        height: 30
        Layout.fillWidth: true

        //Text { text: index }
        Text {
            Layout.leftMargin: 10
            width: PlasmaCore.Units.iconSizes.smallMedium
            text: "ACTIVE"
        }
        Text {
            Layout.preferredWidth: 40
            Layout.alignment: Qt.AlignRight
            text: "ID"
        }
        Rectangle {
            Layout.preferredWidth: 50
            Text { anchors.centerIn: parent; text: "RATE" }
        }
        Rectangle {
            Layout.preferredWidth: 70
            Layout.alignment: Qt.AlignRight
            Text { anchors.centerIn: parent; text: "WAIT" }
        }
        Rectangle {
            Layout.preferredWidth: 70
            Text { anchors.centerIn: parent; text: "BUSY" }
        }
        Text {
            Layout.fillWidth: true
            text: "NAME"
        }
    }

    ListView {
        id: topList
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: Pipewire.nodeList
        delegate: rowDelegate
        clip: true
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
    }

    Component {
        id: rowDelegate
        Item {
            width: topList.width
            height: nodeRow.height

            required property int index
            property var node: topList.model[index]

            RowLayout {
                id: nodeRow
                height: 30
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft
                Layout.margins: 10
                Layout.leftMargin: 10
                Layout.rightMargin: 10

                //Text { text: index }
                Kirigami.Icon {
                    Layout.leftMargin: 20
                    width: PlasmaCore.Units.iconSizes.smallMedium
                    height: PlasmaCore.Units.iconSizes.smallMedium
                    source: node.active ? "media-playback-start" : "media-playback-pause"
                }
                Text {
                    Layout.preferredWidth: 40;
                    Layout.alignment: Qt.AlignRight;
                    text: node.id
                }
                Rectangle {
                    Layout.preferredWidth: 50
                    Text { anchors.centerIn: parent; text: node.rate === 0 ? "" : node.rate }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Layout.alignment: Qt.AlignRight;
                    Text { anchors.centerIn: parent; text: node.formatTime(node.waiting) }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Text { anchors.centerIn: parent; text: node.formatTime(node.busy) }
                }
                Text {
                    Layout.fillWidth: true
                    text: (node.driver ? " + " : "") + node.name
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: topList.currentIndex = index
            }
        }

    }
}
