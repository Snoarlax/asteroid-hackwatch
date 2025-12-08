import QtQuick 2.9
import org.asteroid.controls 1.0
import org.asteroid.utils 1.0

Item {
    id: root

    Column {
        anchors.centerIn: parent
        width: parent.width * 0.9
        spacing: root.height * 0.02

        // Output display
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
                contentHeight: outputText.height
                clip: true

                Label {
                    id: outputText
                    text: commandRunner.output || "Select a script to run"
                    font.pixelSize: root.height * 0.04
                    wrapMode: Text.WordWrap
                    width: parent.width
                    color: commandRunner.output ? "#ffffff" : "#666666"
                }
            }
        }
    }
}
