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
                    id: nodeIcon
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: textSource(model.node)


                    source: mediaIconSource(model.node)
                    fallback: "script-error"


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

                    function textSource(node) {
                        switch(node.mediaType) {
                        case Node.MediaTypeAudio:
                            switch(node.nodeType) {
                                case Node.NodeTypeInput:
                                    return i18nc("node_type", "Audio Input");
                                case Node.NodeTypeSource:
                                    return i18nc("node_type", "Audio Source");
                                case Node.NodeTypeOutput:
                                    return i18nc("node_type", "Audio Output");
                                case Node.NodeTypeSink:
                                    return i18nc("node_type", "Audio Sink");
                                default:
                                    return i18nc("node_type", "Error");
                            }
                        case Node.MediaTypeVideo:
                            switch(node.nodeType) {
                                case Node.NodeTypeInput:
                                    return i18nc("node_type", "Video Input");
                                case Node.NodeTypeSource:
                                    return i18nc("node_type", "Video Source");
                                case Node.NodeTypeOutput:
                                    return i18nc("node_type", "Video Output");
                                case Node.NodeTypeSink:
                                    return i18nc("node_type", "Video Sink");
                                default:
                                    return i18nc("node_type", "Error");
                            }
                        case Node.MediaTypeMidi:
                            return i18nc("node_type", "Midi");
                        case Node.MediaTypeNone:
                            return i18nc("node_type", "None");
                        default:
                            return i18nc("node_type", "Error");
                        }
                    }
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

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Controls.Label {
                            //Controls.ToolTip.visible: model.node.isAlsa() && hovered
                            //Controls.ToolTip.text: model.node.nodeDescription
                            Layout.fillWidth: true
                            height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                            text: model.name //+ "|" + model.display.category + "|" + model.display.mediaClass
                            //color: listItem.checked || (listItem.pressed && !listItem.checked && !listItem.sectionDelegate) ? listItem.activeTextColor : listItem.textColor
                        }
                        Controls.Label {
                            visible: model.node.nodeDescription && model.name != model.node.nodeDescription
                            Layout.fillWidth: true
                            height: Math.max(implicitHeight, Kirigami.Units.iconSizes.smallMedium)
                            text: model.node.nodeDescription
                            color: Kirigami.Theme.disabledTextColor
                        }
                    }
                }

                Kirigami.OverlaySheet {
                    id: infoAlsaNodeSheet
                    header: Kirigami.Heading {
                        text: model.name
                    }
                    Kirigami.FormLayout {
                        Controls.Label {
                            id: nameField
                            Kirigami.FormData.label: i18nc("@label:textbox", "Node Description:")
                            text: model.node.nodeDescription
                            enabled: false
                        }
                        Controls.Label {
                            id: descriptionField
                            Kirigami.FormData.label: i18nc("@label:textbox", "Node name:")
                            text: model.node.nodeName
                            enabled: false
                        }
                        Controls.Label {
                            id: classField
                            Kirigami.FormData.label: i18nc("@label:textbox", "Media Class:")
                            text: model.node.mediaClass
                            enabled: false
                        }
                    }
                }
            }
            actions: [
                Kirigami.Action {
                    visible: model.node.isAlsa()
                    iconName: "documentinfo"
                    text: i18nc("@audiostream","info")
                    onTriggered: {
                        infoAlsaNodeSheet.open()
                    }
                }
            ]
        }
    }

    ListView {
        id: mainList
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: -1

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
