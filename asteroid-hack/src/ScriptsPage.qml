import QtQuick 2.9
import org.asteroid.controls 1.0
import org.asteroid.utils 1.0

Item {
    id: root

    Component { id: outputLayer;                  OutputPage       { } }

    PageHeader {
        id: title
        text: qsTrId("Keyboard")
    }

    Column {
        anchors.centerIn: parent
        width: parent.width * 0.9
        spacing: root.height * 0.02

        // Scripts list
        Rectangle {
            width: parent.width
            height: root.height
            color: "#2a2a2a"
            radius: width * 0.05
            border.color: "#555555"
            border.width: 1

            Flickable {
                anchors.fill: parent
                anchors.margins: parent.width * 0.05
                contentHeight: scriptsColumn.height
                clip: true

                Column {
                    id: scriptsColumn
                    width: parent.width
                    spacing: parent.height * 0.02

                    Repeater {
                        model: commandRunner.scripts

                        Rectangle {
                            width: scriptsColumn.width
                            height: root.height * 0.15
                            color: scriptMouseArea.pressed ? "#4a4a4a" : "#3a3a3a"
                            radius: height * 0.2
                            border.color: "#666666"
                            border.width: 1

                            Label {
                                text: modelData
                                anchors.centerIn: parent
                                font.pixelSize: parent.height * 0.4
                                color: "#ffffff"
                            }

                            MouseArea {
                                id: scriptMouseArea
                                anchors.fill: parent
                                enabled: !commandRunner.isRunning
                                onClicked: {
                                    layerStack.push(outputLayer)
                                    commandRunner.runScript(modelData)
                                }
                            }
                        }
                    }

                    Label {
                        visible: commandRunner.scripts.length === 0
                        text: "No scripts found in\n~/hack_scripts/"
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: parent.height * 0.08
                        color: "#666666"
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }

    Connections {
        target: commandRunner
        function onCommandFinished(success) {
            console.log("Script finished. Success:", success)
        }
    }
}
