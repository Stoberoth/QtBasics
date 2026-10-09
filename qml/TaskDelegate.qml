import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    required property int index
    required property var model
    property bool reorderEnabled: false
    property ListView listView

    height: 44
    radius: 8
    color: root.model.completed ? "#181825" : "#313244"
    signal taskToggled
    signal taskRemoved
    signal taskReordered(int toIndex)

    CheckBox {
        id: check
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        checked: root.model.completed
        onToggled: root.taskToggled()
    }

    Button {
        id: deleteBtn
        anchors.right: drag.left
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        text: "x"
        onClicked: root.taskRemoved()
    }

    Text {
        anchors.left: check.right
        anchors.right: deleteBtn.left
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        text: root.model.title
        color: root.model.completed ? "#6c7086" : "#cdd6f4"
        font.strikeout: root.model.completed
        font.pixelSize: 14
    }
    Rectangle {
        id: drag
        height: 20
        width: 20
        color: "blue"
        radius: 4
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        Text {
            anchors.centerIn: parent
            text: "≡"
        }
        MouseArea {
            anchors.fill: parent
            enabled: root.reorderEnabled
            preventStealing: true
            cursorShape: enabled ? Qt.OpenHandCursor : Qt.ArrowCursor

            onReleased: {
                if (!root.listView || root.listView.count === 0)
                    return;
                const pos = mapToItem(root.listView.contentItem, mouseX, mouseY);
                let to = root.listView.indexAt(pos.x, pos.y);
                if (to < 0)
                    to = pos.y <= 0 ? 0 : root.listView.count - 1;
                root.taskReordered(to);
            }
        }
    }
}
