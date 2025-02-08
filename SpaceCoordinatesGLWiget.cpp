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
    float step = 0.1f;
    int num = 10;

    auto addLine = [&](float x1, float y1, float z1, float x2, float y2, float z2) {
            vertices << x1 << y1 << z1 << 0.2f << 0.8f << 0.8f
                     << x2 << y2 << z2 << 0.2f << 0.8f << 0.8f;
        };

    for (int i = -num; i <= num; ++i) {
        float pos = i * step;
        addLine(pos, -0.5f, -num * step, pos, -0.5f, num * step);
        addLine(-num * step, -0.5f, pos, num * step, -0.5f, pos);
    }

    m_vaoGrid.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // 将顶点数据传递给着色器程序
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 设置颜色属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_vaoGrid.release();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpaceCoordinatesGLWiget::drawCoordinates()
{
    makeCurrent();

    std::vector<GLfloat> vertices;

    // 原坐标轴顶点数据（颜色分别为红、绿、蓝）
    const GLfloat originalVertices[] = {
        // X轴（红色）
        -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Y轴（绿色）
         0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Z轴（蓝色）
         0.0f, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    vertices.insert(vertices.end(), originalVertices, originalVertices + sizeof(originalVertices)/sizeof(GLfloat));

    // 生成刻度线参数
    const float mainTickSize = 0.02f; // 主刻度线长度的一半
    const float subTickSize = 0.01f;  // 次刻度线长度的一半
    const int numXTicks = 21;         // X轴主刻度数（-1.0到1.0，步长0.1）
    const int numYTicks = 11;         // Y轴主刻度数（0.0到1.0，步长0.1）
    const int numZTicks = 21;         // Z轴主刻度数（-1.0到1.0，步长0.1）

    // 生成X轴刻度线（沿Y方向，红色）
    for (int i = 0; i < numXTicks; ++i) {
        float x = -1.0f + i * 0.1f;
        // 主刻度线
        vertices.insert(vertices.end(), {x, -mainTickSize, 0.0f, 1.0f, 0.0f, 0.0f});
        vertices.insert(vertices.end(), {x,  mainTickSize, 0.0f, 1.0f, 0.0f, 0.0f});

        // 次刻度线（每两个主刻度之间加四个次刻度）
        if (i < numXTicks - 1) {
            for (int j = 1; j < 5; ++j) {
                float subX = x + j * 0.02f; // 次刻度间隔为0.02
                vertices.insert(vertices.end(), {subX, -subTickSize, 0.0f, 1.0f, 0.0f, 0.0f});
                vertices.insert(vertices.end(), {subX,  subTickSize, 0.0f, 1.0f, 0.0f, 0.0f});
            }
        }
    }

    // 生成Y轴刻度线（沿X方向，绿色）
    for (int i = 0; i < numYTicks; ++i) {
        float y = i * 0.1f;
        // 主刻度线
        vertices.insert(vertices.end(), {-mainTickSize, y, 0.0f, 0.0f, 1.0f, 0.0f});
        vertices.insert(vertices.end(), { mainTickSize, y, 0.0f, 0.0f, 1.0f, 0.0f});

        // 次刻度线（每两个主刻度之间加四个次刻度）
        if (i < numYTicks - 1) {
            for (int j = 1; j < 5; ++j) {
                float subY = y + j * 0.02f; // 次刻度间隔为0.02
                vertices.insert(vertices.end(), {-subTickSize, subY, 0.0f, 0.0f, 1.0f, 0.0f});
                vertices.insert(vertices.end(), { subTickSize, subY, 0.0f, 0.0f, 1.0f, 0.0f});
            }
        }
    }

    // 生成Z轴刻度线（沿X方向，蓝色）
    for (int i = 0; i < numZTicks; ++i) {
        float z = -1.0f + i * 0.1f;
        // 主刻度线
        vertices.insert(vertices.end(), {-mainTickSize, 0.0f, z, 0.0f, 0.0f, 1.0f});
        vertices.insert(vertices.end(), { mainTickSize, 0.0f, z, 0.0f, 0.0f, 1.0f});

        // 次刻度线（每两个主刻度之间加四个次刻度）
        if (i < numZTicks - 1) {
            for (int j = 1; j < 5; ++j) {
                float subZ = z + j * 0.02f; // 次刻度间隔为0.02
                vertices.insert(vertices.end(), {-subTickSize, 0.0f, subZ, 0.0f, 0.0f, 1.0f});
                vertices.insert(vertices.end(), { subTickSize, 0.0f, subZ, 0.0f, 0.0f, 1.0f});
            }
        }
    }

    m_vaoCoordinates.bind();

    // 创建、绑定并更新VBO数据
    glGenBuffers(1, &m_vbo[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_vaoCoordinates.release();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
