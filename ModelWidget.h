#ifndef MODELWIDGET_H
#define MODELWIDGET_H

#include "SpaceCoordinatesGLWiget.h"
#include "STLFileLoader.h"
#include "PLYFileLoader.h"

enum ShowType
{
    ShowType_STL,       // STL文件
    ShowType_PLY        // 点云文件
};

class ModelWidget : public SpaceCoordinatesGLWiget
{
    Q_OBJECT
public:
    ModelWidget(QWidget *parent = nullptr);

    ~ModelWidget();

    void translate_point(QPoint &p_ab);

    // 辅助函数：将3D点投影到屏幕坐标
    QPointF projectToScreen(const QVector3D &point, const QMatrix4x4 &mvp, const QRect &viewport);

protected:
    void paintGL() override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void showSTLModel(QString stlPath);

    void showMultiSTLModel(QStringList stlPaths);

    void showPLYModel(QString plyPath);

    void showGrid(bool bShow);

    void showCoordinates(bool bShow);

    void showHighLight(bool bShow);

private:
    QVector<GLfloat> vertices;
    QVector<GLfloat> pointVertices;

    ShowType m_ShowType;                // 绘制的文件类型
    bool m_bDrawGrid = false;           // 是否绘制网格
    bool m_bDrawCoordinates = false;    // 是否绘制坐标轴
    bool m_bDrawHighLight = false;      // 是否绘制高亮边框

    QMatrix4x4 modelMatrix;             // 模式矩阵
    QMatrix4x4 modelUse;                // 这相当于一个中间值
    QMatrix4x4 modelSave;               // 上一次鼠标点击移动过后的 模式矩阵
    QPoint press_position;              // 按下鼠标左键时的位置
};

#endif // MODELWIDGET_H
