#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <QKeyEvent>
#include <QOpenGLShader>

const float SENSITIVITY = 0.01f;

class Camera
{
public:
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 3.0f), QVector3D front = QVector3D(0.0, 0.0, 0.0), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f));

    // 获取lookAt矩阵
    QMatrix4x4 getViewMatrix();

    // 滚轮控制
    void processMouseScroll(float yoffset);

public:
    // Camera Attributes
    QVector3D m_position;
    QVector3D m_front;
    QVector3D m_up;
    QVector3D m_right;
    QVector3D m_worldUp;

    // 滚轮敏感度
    float scrollSensitivity;
};

#endif // CAMERA_H
