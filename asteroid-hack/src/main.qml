import QtQuick 2.9
import org.asteroid.controls 1.0
import org.asteroid.utils 1.0

Application {
    id: app

    centerColor: "#0044A6"
    outerColor: "#00010C"

    Component { id: keyboardLayer;                  KeyboardPage       { } }
    Component { id: scriptsLayer;                  ScriptsPage       { } }

    LayerStack {
        id: layerStack
        firstPage: firstPageComponent
    }

    Component {
        id: firstPageComponent

        Flickable {
            contentHeight: settingsColumn.implicitHeight
            contentWidth: width
            boundsBehavior: Flickable.DragOverBounds
            flickableDirection: Flickable.VerticalFlick

            Column {
                id: settingsColumn
                anchors.fill: parent

                Item { width: parent.width; height: DeviceSpecs.hasRoundScreen ? Dims.h(6) : Dims.h(2) }
                ListItem {
                    title: qsTrId("Scripts")
                    iconName: "ios-launcher-outline"
                    onClicked: layerStack.push(scriptsLayer)
                }

                ListItem {
                    title: qsTrId("Keyboard")
                    iconName: "ios-usb"
                    onClicked: layerStack.push(keyboardLayer)
                }


                Item { width: parent.width; height: DeviceSpecs.hasRoundScreen ? Dims.h(6) : Dims.h(2) }
            }
        }
    }
    function backToMainMenu() {
        while (layerStack.layers.length > 0) {
            layerStack.pop(layerStack.currentLayer)
        }
    }
}
