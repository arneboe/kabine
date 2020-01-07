import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0


Window
{
    id: main_window
    visible: true
    width: 800
    height: 600
    visibility: Window.Maximized
    
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
                id: take_pic_button
                objectName: "take_pic_button"
                contentItem: Text {
                    text: qsTr("Take Picture")
                    font.pointSize: 20
                    opacity: enabled ? 1.0 : 0.3
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }

            Button {
                id: print_pic_button
                objectName: "print_pic_button"
                contentItem: Text {
                    text: qsTr("Print & Delete Picture")
                    font.pointSize: 20
                    opacity: enabled ? 1.0 : 0.3
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
            
            Button {
                id: delete_pic_button
                objectName: "delete_pic_button"
                contentItem: Text {
                    text: qsTr("Delete Picture")
                    font.pointSize: 20
                    opacity: enabled ? 1.0 : 0.3
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
        }
    }
}
