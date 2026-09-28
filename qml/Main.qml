import QtQuick
import QtQuick.Controls

Window{
    width:640
    height: 480
    visible:true
    title: "Task & Habit Tracker"

    color: "#1e1e2e"

    Column{
        anchors.centerIn: parent
        Text{
            text: "QML Rocks"
            color: "white"
        }
        Text{
            text:"Mais pas autant que moi !!!!!"
            color:"red"
        }
    }
}
