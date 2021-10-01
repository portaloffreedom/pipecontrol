import QtQuick 2.12
import QtQuick.Layouts 1.11
import Pipewire 1.0
import QtQuick.Controls 2.12
//import org.kde.plasma.core 2.0 as PlasmaCore
//import org.kde.kirigami 2.12 as Kirigami


ColumnLayout {
    width: 400
    height: 200

    RowLayout {
        id: header
        height: 30
        Layout.fillWidth: true

        //Label { text: index }
//        Label {
//            Layout.preferredWidth: PlasmaCore.Units.iconSizes.smallMedium +30
//            text: "ACTIVE"
//        }
        Label {
            Layout.leftMargin: 10
            Layout.preferredWidth: 40
            Layout.alignment: Qt.AlignRight
            text: "ID"
        }
        Rectangle {
            Layout.preferredWidth: 50
            Label {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: "RATE"
            }
        }
        Rectangle {
            Layout.preferredWidth: 70
            Label {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: "QUANT"
            }
        }
        Rectangle {
            Layout.preferredWidth: 70
            Label {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: "WAIT"
            }
        }
        Rectangle {
            Layout.preferredWidth: 70
            Label {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: "BUSY"
            }
        }
        Label {
            Layout.leftMargin: 15
            Layout.fillWidth: true
            text: "NAME"
        }
    }

    ListView {
        id: topList
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: Pipewire.nodes
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
            required property int id
            required property bool active
            required property int rate
            required property int quantum
            required property double wait
            required property double busy
            required property string name
            required property int driverID

            //required property var display
            //property var node: Pipewire.nodeList[index]

            RowLayout {
                id: nodeRow
                height: 30
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignLeft
                Layout.margins: 10
                Layout.leftMargin: 10
                Layout.rightMargin: 10

//                Label { text: active }
//                Kirigami.Icon {
//                    visible: active
//                    Layout.leftMargin: 10
//                    Layout.rightMargin: 10
//                    width: PlasmaCore.Units.iconSizes.smallMedium
//                    height: PlasmaCore.Units.iconSizes.smallMedium
////                    source: active ? "media-playback-start" : "media-playback-pause"
//                    source: "media-playback-start"
//                }
//                Kirigami.Icon {
//                    visible: !active
//                    Layout.leftMargin: 10
//                    Layout.rightMargin: 10
//                    width: PlasmaCore.Units.iconSizes.smallMedium
//                    height: PlasmaCore.Units.iconSizes.smallMedium
//                    source: "media-playback-pause"
//                }
                Label {
                    Layout.leftMargin: 10
                    Layout.preferredWidth: 40;
                    Layout.alignment: Qt.AlignRight;
                    text: id
                }
                Rectangle {
                    Layout.preferredWidth: 50
                    Layout.alignment: Qt.AlignRight;
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: rate
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Layout.alignment: Qt.AlignRight;
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: quantum
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: Pipewire.formatTime(wait)
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: Pipewire.formatTime(busy)
                    }
                }
                Label {
                    Layout.fillWidth: true
                    Layout.leftMargin: 15
                    text: (driverID > 0 ? " + " : "") + name
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: topList.currentIndex = index
            }
        }

    }
}
