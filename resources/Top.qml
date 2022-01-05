import QtQuick 2.12
import QtQuick.Layouts 1.11
import Pipewire 1.0
import Pipewire.Node 1.0
import QtQuick.Controls 2.12 as Controls
import org.kde.kirigami 2.12 as Kirigami

Kirigami.ScrollablePage {
    title: i18nc("@title","Top")
    property var root
    Layout.fillWidth: true

    actions.main: Kirigami.Action {
        id: addAction
        icon.name: "settings-configure"
        text: i18n("Settings")
        onTriggered: root.pageStack.replace("qrc:/resources/Settings.qml", {
            root: root
        })
    }

    Component {
        id: delegateComponent
        Kirigami.SwipeListItem {
            id: listItem
            contentItem: RowLayout {

                Kirigami.ListItemDragHandle {
                    visible: false //model.driverID > 0
                    listItem: listItem
                    listView: mainList
                    property int oldIndex: -1;
                    property int newIndex: -1;
                    onMoveRequested: function(p_oldIndex, p_newIndex) {
                        oldIndex = p_oldIndex
                        newIndex = p_newIndex
                    }
                    onDropped: {
                        console.log("element moved " + oldIndex + " -> " + newIndex + " dropped");
                        Pipewire.nodes.move(oldIndex, newIndex);
                        oldIndex = -1
                        newIndex = -1
                    }
                }

                Kirigami.Icon {
                    function mediaIconSource(node) {
                        switch(node.mediaType) {
                        case Node.MediaTypeAudio:
                            switch(node.nodeType) {
                                case Node.NodeTypeInput:
                                case Node.NodeTypeSource:
                                    return "audio-input-microphone-symbolic";
                                case Node.NodeTypeOutput:
                                case Node.NodeTypeSink:
                                    return "audio-speakers-symbolic";
                                default:
                                    return "error";
                            }
                        case Node.MediaTypeVideo:
                            return "camera-video-symbolic";
                        case Node.MediaTypeMidi:
                            return "music-note-16th";
                        default:
                        case Node.MediaTypeNone:
                            return "error";
                        }
                    }

                    source: mediaIconSource(model.node)
                    fallback: "script-error"
                }

                Controls.Label {
                    Layout.leftMargin: 10
                    Layout.preferredWidth: 40
                    Layout.alignment: Qt.AlignRight
                    text: model.id
                }

                Rectangle {
                    Layout.preferredWidth: 50
                    Layout.alignment: Qt.AlignRight;
                    Controls.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: rate
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Layout.alignment: Qt.AlignRight;
                    Controls.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: quantum
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Controls.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: Pipewire.formatTime(wait)
                    }
                }
                Rectangle {
                    Layout.preferredWidth: 70
                    Controls.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: Pipewire.formatTime(busy)
                    }
                }

                RowLayout {
                    Layout.leftMargin: 15
                    Layout.fillWidth: true
                    Kirigami.Icon {
                        visible: model.driverID > 0
                        source: "aggregation"
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                        text: model.name //+ "|" + model.display.category + "|" + model.display.mediaClass
                        //color: listItem.checked || (listItem.pressed && !listItem.checked && !listItem.sectionDelegate) ? listItem.activeTextColor : listItem.textColor
                    }
                }
            }
            //actions: [
                //Kirigami.Action {
                    //iconName: "document-decrypt"
                    //text: "Action 1"
                    //onTriggered: showPassiveNotification(model.text + " Action 1 clicked")
                //},
                //Kirigami.Action {
                    //iconName: "mail-reply-sender"
                    //text: "Action 2"
                    //onTriggered: showPassiveNotification(model.text + " Action 2 clicked")
                //}]
        }
    }

    ListView {
        id: mainList
        Layout.fillWidth: true
        Layout.fillHeight: true

        header:  RowLayout {
            id: header
            height: 30
            Layout.fillWidth: true

            Kirigami.Icon {}

            Controls.Label {
                Layout.leftMargin: 20
                Layout.preferredWidth: 40
                Layout.alignment: Qt.AlignRight
                text: "ID"
            }
            Rectangle {
                Layout.preferredWidth: 50
                Controls.Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    text: "RATE"
                }
            }
            Rectangle {
                Layout.preferredWidth: 70
                Controls.Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    text: "QUANT"
                }
            }
            Rectangle {
                Layout.preferredWidth: 70
                Controls.Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    text: "WAIT"
                }
            }
            Rectangle {
                Layout.preferredWidth: 70
                Controls.Label {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    text: "BUSY"
                }
            }

            Controls.Label {
                Layout.leftMargin: 15
                Layout.fillWidth: true
                text: "NAME"
            }
        }

        model: Pipewire.nodes
        Component.onCompleted: Pipewire.nodes.sortList()
        property var old_model: ListModel {
            id: listModel

            Component.onCompleted: {
                for (var i = 0; i < 200; ++i) {
                    listModel.append({"title": "Item " + i,
                        "actions": [{text: "Action 1", icon: "document-decrypt"},
                                    {text: "Action 2", icon: "mail-reply-sender"}],
                        //"sec": Math.floor(i/10)
                    })
                }
            }
        }
        moveDisplaced: Transition {
            YAnimator {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
        delegate: Kirigami.DelegateRecycler {
            id: delegate
            width: parent ? parent.width : implicitWidth
            sourceComponent: delegateComponent
        }
        //section {
            //property: "sec"
            //delegate: Kirigami.ListSectionHeader {
                //text: "Section " + (parseInt(section) + 1)
            //}
        //}
    }


}
