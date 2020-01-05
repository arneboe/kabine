import QtQuick 2.0
import QtQuick.Window 2.0

Window
{
    id: main_window
    visible: true
    width: 300
    height: 240

//     Rectangle {
//         anchors.fill: parent
//         color: "steelblue"
//     }
    
    Image {
        cache: false
        objectName: "image_viewer"
        id: image_viewer
        anchors.fill: parent
        property int currentFrameNumber: 0
        source: "image://capture_provider/image" + currentFrameNumber

        NumberAnimation on currentFrameNumber {
            from: 0;
            to: 25;
            loops: Animation.Infinite;
            duration: 1000
        }
    
        MouseArea {
            anchors.fill: parent
            onClicked: {
                parent.currentFrameNumber = parent.currentFrameNumber + 1
//                 parent.sourceChanged(parent.source);
            }
        }

    }

}
