import QtQuick
import QtQuick.Controls
import Qt_basics

Window{
    width:640
    height: 480
    visible:true
    title: "Task & Habit Tracker"
    color: "#1e1e2e"

    TaskListModel
    {
        id: taskModel
    }
    TaskFilterProxyModel
    {
        id: proxyModel
        sourceModel: taskModel
    }
    DashboardController
    {
        id: controller
        taskModel: taskModel
    }

    Column
    {
        anchors.centerIn: parent
        spacing: 16

        Text
        {
            text: controller.title
            color: "#cdd6f4"
        }
        TextField
        {
            anchors.horizontalCenter: parent.horizontalCenter
            text: controller.title
            onTextChanged: controller.title = text
        }
        ProgressBar
        {
            from: 0
            to: 100
            value: controller.progress
        }
        Text
        {
            text: controller.progress + "%"
            color: "#a6adc8"
        }

        Row
        {
            spacing: 8
            TextField
            {
                id: newTaskField
                placeholderText:"Nouvelle tache"
            }
            Button
            {
                text:"Ajouter"
                onClicked: {
                    taskModel.addTask(newTaskField.text)
                    newTaskField.text = ""
                }
            }

        }
        Row
        {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter

            ButtonGroup
            {
                id: filterGroup
            }
            Button
            {
                text:"Toutes"
                checkable: true
                checked: proxyModel.filterMode === TaskFilterProxyModel.All
                ButtonGroup.group: filterGroup
                onClicked: proxyModel.filterMode = TaskFilterProxyModel.All
            }
            Button {
                text: "Actives"
                checkable: true
                checked: proxyModel.filterMode === TaskFilterProxyModel.Active
                ButtonGroup.group: filterGroup
                onClicked: proxyModel.filterMode = TaskFilterProxyModel.Active
            }
            Button {
                text: "Terminées"
                checkable: true
                checked: proxyModel.filterMode === TaskFilterProxyModel.Completed
                ButtonGroup.group: filterGroup
                onClicked: proxyModel.filterMode = TaskFilterProxyModel.Completed
            }
        }
        ListView
        {
            model: proxyModel
            width: parent.width
            height: 200
            clip: true
            delegate: Row
            {
                spacing: 8
                CheckBox
                {
                    checked: model.completed
                    onToggled: proxyModel.toggleTask(model.index)
                }
                Text
                {
                    text: model.title
                    color: model.completed ? "#6c7086" : "#cdd6f4"
                    font.strikeout : model.completed
                }
                Button
                {
                    text:"x"
                    onClicked: proxyModel.removeTask(model.index)
                }
            }
        }
    }
}
