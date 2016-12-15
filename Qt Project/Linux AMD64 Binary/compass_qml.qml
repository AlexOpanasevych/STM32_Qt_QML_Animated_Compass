import QtQuick 2.0

Item {
    Image {
        source: "compass.svg"
    }
    Image {
        x: 189
        y: 70
        width: 32
        height: 200
        source: "arrow.svg"
        transform: Rotation {
            origin.x: 16
            origin.y: 100
            angle: 0
            objectName: "rect"
        }
    }

}
