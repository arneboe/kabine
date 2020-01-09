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
    color: "black"

    Text {
        objectName: "take_pic_text"
        id: take_pic_text
        text: "Taking Picture"
        font.family: "Helvetica"
        font.pointSize: 55
        font.bold: true
        color: "white"
        anchors.centerIn: parent
        state: "invisible"

        states: [
            State {
                name: "invisible"
                PropertyChanges { target: take_pic_text; opacity: 0 }
            },

            State {
                name: "visible"
                PropertyChanges { target: take_pic_text; opacity: 1.0 }
            }
        ]

        transitions: Transition {
            NumberAnimation { properties: "opacity"; duration: 600 }
        }

        function show() {
            state = "visible";
        }
        function hide() {
            state = "invisible";
        }
    }
    
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

            state: "visible"
            
            states: [
                State {
                    name: "invisible"
                    PropertyChanges { target: image_viewer; opacity: 0 }
                },

                State {
                    name: "visible"
                    PropertyChanges { target: image_viewer; opacity: 1.0 }
                }
            ]    
            
            transitions: Transition {
                NumberAnimation { properties: "opacity"; duration: 600 }
            }
            
            function show() {
                state = "visible";
            }
            function hide() {
                state = "invisible";
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
                enabled: false
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
                enabled: false
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
