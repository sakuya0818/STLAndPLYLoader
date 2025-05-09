#ifndef SPACECOORDINATESGLWIGET_H
#define SPACECOORDINATESGLWIGET_H

#include "Camera.h"

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QFileInfo>
#include <QMouseEvent>
#include <QDateTime>
#include <QPainter>
#include <QDebug>

// 显示模型的OpenGL基类，提供了鼠标旋转、方法缩小、平移、网格、坐标系等基本功能
class SpaceCoordinatesGLWiget : public QOpenGLWidget, public QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    SpaceCoordinatesGLWiget(QWidget *parent = nullptr);

    ~SpaceCoordinatesGLWiget();

    // 画网格
    void drawGrid();

    // 画坐标系
    void drawCoordinates();

    // 规格化点数据，映射到-1.0 ~ 1.0的范围之内
    void normalizePointData(QVector<QVector3D> data);

    float getFactor();
    void getMaxCoordinate();// 分别计算出 x y z 的最大值 组合成一个点；
    void getMinCoordinate();//  分别计算 x y z 的最小值 组合成一个；
    void getCenterPoint(QVector3D &vec); // 获取中心点

signals:
    // 信号：x轴旋转角度变化
    void xRotationChanged(int angle);

    // 信号：y轴旋转角度变化
    void yRotationChanged(int angle);

    // 信号：z轴旋转角度变化
    void zRotationChanged(int angle);

protected:
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int w, int h) override;

protected:
    // 创建VAO和VBO对象
    QOpenGLVertexArrayObject m_vaoGrid;
    QOpenGLVertexArrayObject m_vaoCoordinates;
    QOpenGLVertexArrayObject m_vaoSTL;
    QOpenGLVertexArrayObject m_vaoPLY;
    // 创建VBO对象
    GLuint m_vbo[4];
    // 创建QOpenGLShaderProgram对象
    QOpenGLShaderProgram m_stlShaderProgram;
    QOpenGLShaderProgram m_plyShaderProgram;
    // 相机对象
    Camera *m_camera;
    // 点数据
    QVector<QVector3D> m_pointData;
    // 最大坐标点
    QVector3D m_maxCoordinate;
    // 最小坐标大
    QVector3D m_minCoordinate;
};

#endif // SPACECOORDINATESGLWIGET_H
