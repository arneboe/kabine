import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0


Window
{
    id: main_window
    visible: true
    width: 800
    height: 600
    ColumnLayout {
        spacing: 2
        anchors.fill: parent
        Image {
            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectFit
            scale: Qt.KeepAspectRatio
            
            cache: false
            objectName: "image_viewer"
            id: image_viewer
            Layout.fillWidth: true
            Layout.fillHeight: true
            property int currentFrameNumber: 0
            source: "image://capture_provider/image" + currentFrameNumber
            signal imageClicked()
            
            MouseArea {
                anchors.fill: parent
                onClicked: image_viewer.imageClicked()
            }
            
            NumberAnimation on currentFrameNumber {
                from: 0;
                to: 25;
                loops: Animation.Infinite;
                duration: 1000
            }
        }
        RowLayout {
            spacing: 200
            Layout.alignment: Qt.AlignCenter
        Button {
            id: control
            text: qsTr("Take Picture")

            contentItem: Text {
                text: control.text
                font.pointSize: 20
                opacity: enabled ? 1.0 : 0.3
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

//             background: Rectangle {
//                 implicitWidth: 100
//                 implicitHeight: 40
//                 opacity: enabled ? 1 : 0.3
//                 border.color: control.down ? "#17a81a" : "#21be2b"
//                 border.width: 1
//                 radius: 2
//             }
}
            Button {
                text: "Delete Picture"
            }
            Button {
                text: "Delete & Print Picture"
            }
        }
    }
}
