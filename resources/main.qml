import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import PipewireMetadata 1.0


ApplicationWindow {
    height: 200
    width: 350
    minimumHeight: 200
    minimumWidth: 350
    visible: true
    title: "PipeControl"
//    icon: "qrc:/resources/pipecontrol.png"

    GridLayout {
        columns: 2
        anchors.fill: parent
        anchors.margins: 20


        Text {
            text: "Force sample rate"
        }

        ComboBox {
            id: forceRateCombo
            textRole: "text"
            valueRole: "value"
            onActivated: PipewireMetadata.force_sampleRate = currentValue
            Component.onCompleted: {
                update();
                PipewireMetadata.force_sampleRateChanged.connect(update);
            }

            function update() {
                currentIndex = indexOfValue(PipewireMetadata.force_sampleRate)
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

        Text {
            text: "Minimum buffer"
        }

        ComboBox {
            id: minRateCombo
            textRole: "text"
            valueRole: "value"
            onActivated: PipewireMetadata.minBuffer = currentValue
            Component.onCompleted: {
                update();
                PipewireMetadata.minBufferChanged.connect(update);
            }

            function update() {
                currentIndex = indexOfValue(PipewireMetadata.minBuffer)
            }

            model: [
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

        Text {
            text: "Max buffer"
        }

        ComboBox {
            id: maxRateCombo
            textRole: "text"
            valueRole: "value"
            onActivated: PipewireMetadata.maxBuffer = currentValue
            Component.onCompleted: {
                update();
                PipewireMetadata.maxBufferChanged.connect(update);
            }

            function update() {
                currentIndex = indexOfValue(PipewireMetadata.maxBuffer)
            }

            model: [
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

        Text {
            text: "Force buffer size"
        }

        ComboBox {
            id: forceBufferCombo
            textRole: "text"
            valueRole: "value"
            onActivated: PipewireMetadata.force_buffer = currentValue
            Component.onCompleted: {
                update();
                PipewireMetadata.force_bufferChanged.connect(update);
            }

            function update() {
                currentIndex = indexOfValue(PipewireMetadata.force_buffer)
            }

            model: [
                { value: 0, text: qsTr("Do not force") },
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
    }
}
