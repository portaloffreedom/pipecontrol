import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import Pipewire 1.0

GridLayout {
    id: layout
    columns: 2
//    columnSpacing: 5
//    rowSpacing: 5


//    Rectangle {
//        anchors.fill: parent
//        radius: 10
//        color: "red"
//    }

    Text {
        text: qsTr("Force sample rate")
    }

    ComboBox {
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

    Text {
        text: qsTr("Minimum buffer")
    }

    ComboBox {
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
        text: qsTr("Max buffer")
    }

    ComboBox {
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
        text: qsTr("Force buffer size")
    }

    ComboBox {
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
        text: qsTr("Log Level")
    }

    ComboBox {
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

    Text {
        text: "Client Properties"
    }

    Text {}

    ComboBox {
        id: clientBox
        textRole: "name"
        valueRole: "value"
        Component.onCompleted: {
            currentIndex = 0
        }

        model: Pipewire.client.propertiesList
    }

    Text {
        text: clientBox.currentValue
    }

}
