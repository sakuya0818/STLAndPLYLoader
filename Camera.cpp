#include "Camera.h"

Camera::Camera(QVector3D position, QVector3D front, QVector3D up)
    : scrollSensitivity(SENSITIVITY)
    , m_position(position)
    , m_worldUp(up)
    , m_front(front)
{

}

QMatrix4x4 Camera::getViewMatrix()
{
    QMatrix4x4 matrix;
    matrix.lookAt(m_position, m_front, m_worldUp);
    return matrix;
}

void Camera::processMouseScroll(float yoffset)
{
    yoffset *= scrollSensitivity;

    if (m_position.z() < 0.00001f)
    {
        m_position.setZ(0.00001f);
    }
    else
    {
        m_position.setZ(m_position.z() + yoffset);
    }
}
