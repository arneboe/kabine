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
        
//         SequentialAnimation on currentFrameNumber {
//             loops: Animation.Infinite
//             PropertyAnimation { to: 50 }
//             PropertyAnimation { to: 0 }
//         }
//         

        NumberAnimation on currentFrameNumber {
            from: 0;
            to: 50;
            loops: Animation.Infinite;
            duration: 50 * Math.abs(to - from)
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
