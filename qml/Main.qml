import QtQuick
import QtQuick.Controls
import Qt_basics

Window
{
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
        HabitGauge
        {
            anchors.horizontalCenter: parent.horizontalCenter
            progress: controller.progress
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
            Button
            {
                text:"Post Task"
                onClicked: taskModel.postTask(newTaskField.text)
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
        Row{
            Button
            {
                text:"Import big file"
                onClicked: taskModel.importButton()
            }
            Button
            {
                text:"Fetch from network"
                onClicked: taskModel.fetchFromNetwork()
            }

        }
        ProgressBar
        {
            from:0
            to: 100
            value: taskModel.importProgress
        }
        ListView
        {
            id: taskListView
            model: proxyModel.filterMode === TaskFilterProxyModel.All ? taskModel : proxyModel

            width: parent.width
            height: 200
            clip: true
            delegate: TaskDelegate
            {
                listView: taskListView
                reorderEnabled: proxyModel.filterMode === TaskFilterProxyModel.All
                width: taskListView.width
                onTaskToggled : proxyModel.toggleTask(model.index)
                onTaskRemoved : proxyModel.removeTask(model.index)
                onTaskReordered: (toIndex) => proxyModel.moveTask(index, toIndex)
            }
            add: Transition
            {
                NumberAnimation
                {
                    properties: "x"
                    from: -taskListView.width
                    to: 0
                    duration: 300
                    easing.type: Easing.OutQuad
                }
                NumberAnimation
                {
                    properties: "opacity"
                    from: 0
                    to: 1
                    duration: 300
                }
            }
            remove: Transition
            {
                NumberAnimation
                {
                    properties: "opacity"
                    to: 0
                    duration: 350
                }
                NumberAnimation
                {
                    properties: "scale"
                    to: 0.5
                    duration: 250
                }
            }
            displaced: Transition
            {
                NumberAnimation
                {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }
            move: Transition {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }
            moveDisplaced: Transition {
                NumberAnimation {
                    properties: "y"
                    duration: 250
                    easing.type: Easing.OutCubic
                }
            }
        }
    }
    DropArea
    {
        anchors.fill: parent
        onDropped: (drop) => {
            if(drop.hasUrls)
                taskModel.importFromFile(drop.urls[0])
            drop.acceptProposedAction()
        }
    }
    Text
    {
        text:taskModel.importMessage;
        color: "#f38ba8"
    }

}
