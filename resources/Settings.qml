import QtQuick 2.12
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami
import Pipewire 1.0

Kirigami.ScrollablePage {
    id: settingsPage
    title: i18nc("@title","Settings")
    property var root
    property bool show_advanced: false


    actions {
        main: Kirigami.Action {
            text: i18n("Top")
            icon.name: "source-playlist"
            onTriggered: root.pageStack.replace("qrc:/resources/Top.qml", {
                root: root
            })
        }
        left: Kirigami.Action {
            id: advanced_settings
            text: i18nc("@settings","advanced")
            icon.name: "adjustrow"
            checkable: true
            checked: settingsPage.show_advanced
            onCheckedChanged: settingsPage.show_advanced = advanced_settings.checked
        }
    }

    Item {
        width: layout.implicitWidth
        height: layout.implicitHeight
        implicitWidth: layout.implicitWidth
        implicitHeight: layout.implicitHeight
        Kirigami.FormLayout {
            anchors.fill: parent
            id: layout

            ComboBox {
                Kirigami.FormData.label: i18n("Force sample rate")

                id: forceRateCombo
                textRole: "text"
                valueRole: "value"
                onActivated: Pipewire.settings.force_sampleRate = currentValue
                Component.onCompleted: {
                    update();
                    Pipewire.settings.force_sampleRateChanged.connect(update);
                }

                function update() {
                    currentIndex = indexOfValue(Pipewire.settings.force_sampleRate)
                }

                model: [
                    { value: 0, text: qsTr("Do not force") },
                    { value: 44100, text: "44100" },
                    { value: 48000, text: "48000" },
                    { value: 88200, text: "88200" },
                    { value: 96000, text: "96000" },
                    { value: 176400, text: "176400" },
                    { value: 192000, text: "192000" }
                ]
            }

            ComboBox {
                Kirigami.FormData.label: i18n("Minimum buffer")
                id: minRateCombo
                textRole: "text"
                valueRole: "value"
                onActivated: Pipewire.settings.minBuffer = currentValue
                Component.onCompleted: {
                    update();
                    Pipewire.settings.minBufferChanged.connect(update);
                }

                function update() {
                    currentIndex = indexOfValue(Pipewire.settings.minBuffer)
                }

                model: [
                    { value: 8, text: "8" },
                    { value: 16, text: "16" },
                    { value: 32, text: "32" },
                    { value: 64, text: "64" },
                    { value: 128, text: "128" },
                    { value: 256, text: "256" },
                    { value: 512, text: "512" },
                    { value: 1024, text: "1024" },
                    { value: 2048, text: "2048" },
                    { value: 4096, text: "4096" },
                    { value: 8192, text: "8192" },
                ]
            }

            ComboBox {
                Kirigami.FormData.label: i18n("Max buffer")
                id: maxBufferCombo
                textRole: "text"
                valueRole: "value"
                onActivated: Pipewire.settings.maxBuffer = currentValue
                Component.onCompleted: {
                    update();
                    Pipewire.settings.maxBufferChanged.connect(update);
                }

                function update() {
                    currentIndex = indexOfValue(Pipewire.settings.maxBuffer)
                }

                model: [
                    { value: 8, text: "8" },
                    { value: 16, text: "16" },
                    { value: 32, text: "32" },
                    { value: 64, text: "64" },
                    { value: 128, text: "128" },
                    { value: 256, text: "256" },
                    { value: 512, text: "512" },
                    { value: 1024, text: "1024" },
                    { value: 2048, text: "2048" },
                    { value: 4096, text: "4096" },
                    { value: 8192, text: "8192" },
                ]
            }

            ComboBox {
                Kirigami.FormData.label: i18n("Force buffer size")
                id: forceBufferCombo
                textRole: "text"
                valueRole: "value"
                onActivated: Pipewire.settings.force_buffer = currentValue
                Component.onCompleted: {
                    update();
                    Pipewire.settings.force_bufferChanged.connect(update);
                }

                function update() {
                    currentIndex = indexOfValue(Pipewire.settings.force_buffer)
                }

                model: [
                    { value: 0, text: qsTr("Do not force") },
                    { value: 8, text: "8" },
                    { value: 16, text: "16" },
                    { value: 32, text: "32" },
                    { value: 64, text: "64" },
                    { value: 128, text: "128" },
                    { value: 256, text: "256" },
                    { value: 512, text: "512" },
                    { value: 1024, text: "1024" },
                    { value: 2048, text: "2048" },
                    { value: 4096, text: "4096" },
                    { value: 8192, text: "8192" },
                ]
            }

            ComboBox {
                visible: settingsPage.show_advanced
                Kirigami.FormData.label: i18n("Log Level")
                id: logLevelCombo
                textRole: "text"
                valueRole: "value"
                onActivated: Pipewire.settings.logLevel = currentValue
                Component.onCompleted: {
                    update();
                    Pipewire.settings.logLevelChanged.connect(update);
                }

                function update() {
                    currentIndex = indexOfValue(Pipewire.settings.logLevel)
                }

                model: [
                    { value: 0, text: qsTr("No log") },
                    { value: 1, text: qsTr("Error")  },
                    { value: 2, text: qsTr("Warning")},
                    { value: 3, text: qsTr("Info")   },
                    { value: 4, text: qsTr("Debug")  },
                    { value: 5, text: qsTr("Trace")  },
                ]
            }

            Kirigami.Separator {
                visible: settingsPage.show_advanced
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("ALSA driver settings")
            }

            Switch {
                visible: settingsPage.show_advanced
                Kirigami.FormData.label: i18n("Alsa Batching (USB Device) extra buffer")
                text: checked ? "enabled" : "disabled"
                checked: !Pipewire.alsaProperties.batchDisabled
                onCheckedChanged: Pipewire.alsaProperties.batchDisabled = !checked
            }

            TextField {
                visible: settingsPage.show_advanced
                Kirigami.FormData.label: i18n("\"Alsa Batch\" extra buffer")
                text: Pipewire.alsaProperties.periodSize
                onTextChanged: Pipewire.alsaProperties.periodSize = text
            }

            Button {
                visible: settingsPage.show_advanced
                Kirigami.FormData.label: i18n("Pipewire Media Session")
                text: "Restart"
                onClicked: Pipewire.pipewireMediaSession.restart()
            }

            Kirigami.Separator {
                visible: settingsPage.show_advanced
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: i18n("Pipewire client properties")
            }

            ComboBox {
                visible: settingsPage.show_advanced
                Kirigami.FormData.label: i18n("Client Properties")
                id: clientBox
                textRole: "name"
                valueRole: "value"
                Component.onCompleted: {
                    currentIndex = 0
                }

                model: Pipewire.client.propertiesList
            }

            Label {
                visible: settingsPage.show_advanced
                text: clientBox.currentValue
            }
        }
    }
}
