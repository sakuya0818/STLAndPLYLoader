#include "SpaceCoordinatesGLWiget.h"

SpaceCoordinatesGLWiget::SpaceCoordinatesGLWiget(QWidget *parent)
    :QOpenGLWidget(parent)
{

}

SpaceCoordinatesGLWiget::~SpaceCoordinatesGLWiget() {

}

void SpaceCoordinatesGLWiget::drawGrid()
{
    makeCurrent();

    QVector<GLfloat> vertices;
    float step = 0.1;
    int num = 10;
    for (int i = -num; i < num + 1; i++)
    {
        vertices.push_back(i * step);
        vertices.push_back(-0.5);
        vertices.push_back(-num * step);
        vertices.push_back(i * step);
        vertices.push_back(-0.5);
        vertices.push_back(num * step);
        vertices.push_back(-num * step);
        vertices.push_back(-0.5);
        vertices.push_back(i * step);
        vertices.push_back(num * step);
        vertices.push_back(-0.5);
        vertices.push_back(i * step);
    }

    m_vaoGrid.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // 将顶点数据传递给着色器程序
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    m_vaoGrid.release();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpaceCoordinatesGLWiget::drawCoordinates() {
    makeCurrent();

    QVector<GLfloat> vertices;
    vertices.push_back(-1);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(1);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(1);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(-1);
    vertices.push_back(0);
    vertices.push_back(0);
    vertices.push_back(1);

    m_vaoCoordinates.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // 将顶点数据传递给着色器程序
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    m_vaoCoordinates.release();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 标签
//    qglColor(QColor::fromRgbF(1, 0, 0));
//    renderText(-900, 0, 0, "-X", QFont("helvetica", 12, QFont::Bold, true));
//    renderText(900, 0, 0, "+X", QFont("helvetica", 12, QFont::Bold, true));
//    qglColor(QColor::fromRgbF(0, 1, 0));
//    renderText(0, -900, 0, "-Y", QFont("helvetica", 12, QFont::Bold, true));
//    renderText(0, 900, 0, "+Y", QFont("helvetica", 12, QFont::Bold, true));
//    qglColor(QColor::fromRgbF(0, 0, 1));
//    renderText(0, 0, 700, "+Z", QFont("helvetica", 12, QFont::Bold, true));
//    glLineWidth(1.0f);
//    glPopMatrix();
}

void SpaceCoordinatesGLWiget::initializeGL()
{
    // 在调用任何OpenGL函数之前调用，以确保能够正确加载和使用OpenGL函数
    initializeOpenGLFunctions();

//    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Res/Shader/vertex_object.vert");
//    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Res/Shader/fragment_object.frag");

    // 初始化几何着色器程序
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Res/Shader/vertex.vert");
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Res/Shader/fragment.frag");
    m_shaderProgram.link();

    //建立摄像机
    m_camera = new Camera();

    // 生成一个顶点数组对象名称
    m_vaoGrid.create();
    m_vaoCoordinates.create();
    m_vaoSTL.create();
    m_vaoPLY.create();

    // 指定点阵化点的直径大小为1
    glPointSize(1.0f);

    // 绘制网格和坐标系
    drawGrid();
    drawCoordinates();
}

// 用来绘制OpenGL的窗口了，只要有更新发生，这个函数就会被调用
void SpaceCoordinatesGLWiget::paintGL()
{

}

// 用来处理窗口大小变化这一事件的，width和height就是新的大小状态下的宽和高，另外resizeGL()在处理完后会自动刷新屏幕
void SpaceCoordinatesGLWiget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void SpaceCoordinatesGLWiget::normalizePointData(QVector<QVector3D> data)
{
    getMaxCoordinate();
    getMinCoordinate();
    // 将所有的点向(0,0,0)点移动，使中值点位于(0,0,0)点，这样就不用考虑旋转轴的问题了
    QVector3D centerPoint;
    getCenterPoint(centerPoint);

    for (int i = 0; i < m_pointData.size(); i++)
    {
        m_pointData[i].setX(m_pointData[i].x() - centerPoint.x());
        m_pointData[i].setY(m_pointData[i].y() - centerPoint.y());
        m_pointData[i].setZ(m_pointData[i].z() - centerPoint.z());
    }
    // 更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();

    // 建立一个可以容纳所有点的 正方体 然后 让顶点映射到 0.5（所有点都进行映射）
    float factor = getFactor();//获取变换因子
    for (int i = 0; i < m_pointData.size(); i++)
    {
        m_pointData[i].setX(m_pointData[i].x() * factor);
        m_pointData[i].setY(m_pointData[i].y() * factor);
        m_pointData[i].setZ(m_pointData[i].z() * factor);
    }

    // 更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();
}

float SpaceCoordinatesGLWiget::getFactor()
{
    float max = 0;
    if (max <= m_maxCoordinate.x())
    {
        max = m_maxCoordinate.x();
    }
    if (max <= m_maxCoordinate.y())
    {
        max = m_maxCoordinate.y();
    }
    if (max <= m_maxCoordinate.z())
    {
        max = m_maxCoordinate.z();
    }
    return 0.5 / max;
}

void SpaceCoordinatesGLWiget::getMaxCoordinate()
{
    if (0 == m_pointData.size())
    {
        return;
    }

    QVector<QVector3D>::iterator max_x = std::max_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.x() < vec2.x(); });

    QVector<QVector3D>::iterator max_y = std::max_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.y() < vec2.y(); });

    QVector<QVector3D>::iterator max_z = std::max_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.z() < vec2.z(); });

    m_maxCoordinate = QVector3D(max_x->x(), max_y->y(), max_z->z());
}

void SpaceCoordinatesGLWiget::getMinCoordinate()
{
    if (0 == m_pointData.size())
    {
        return;
    }

    QVector<QVector3D>::iterator max_x = std::min_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.x() < vec2.x(); });

    QVector<QVector3D>::iterator max_y = std::min_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.y() < vec2.y(); });

    QVector<QVector3D>::iterator max_z = std::min_element(m_pointData.begin(), m_pointData.end(), [](QVector3D vec1, QVector3D vec2)
    { return vec1.z() < vec2.z(); });

    m_minCoordinate = QVector3D(max_x->x(), max_y->y(), max_z->z());
}

void SpaceCoordinatesGLWiget::getCenterPoint(QVector3D &vec)
{
    if (0 == m_pointData.size())
    {
        return;
    }
    //取重心
    /*float xSum = 0;
    float ySum = 0;
    float zSum = 0;
    for (int i = 0; i < pointData.size(); i++)
    {
        xSum += pointData[i].x();
        ySum += pointData[i].y();
        zSum += pointData[i].z();
    }
    vec.setX(xSum / pointData.size());
    vec.setY(ySum / pointData.size());
    vec.setZ(zSum / pointData.size());*/

    //取中心， 因为人的眼睛 习惯上 会把物体的中心当做中心点，而不是 重心，取重心的话，旋转效果可能会很怪异
    vec.setX((m_maxCoordinate.x() + m_minCoordinate.x()) / 2);
    vec.setY((m_maxCoordinate.y() + m_minCoordinate.y()) / 2);
    vec.setZ((m_maxCoordinate.z() + m_minCoordinate.z()) / 2);
}
