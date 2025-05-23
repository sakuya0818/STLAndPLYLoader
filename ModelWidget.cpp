#include "ModelWidget.h"

ModelWidget::ModelWidget(QWidget *parent)
    :SpaceCoordinatesGLWiget(parent)
    , m_ShowType(ShowType_STL)
{

}

ModelWidget::~ModelWidget()
{
    makeCurrent();
    m_vaoGrid.destroy();
    m_vaoCoordinates.destroy();
    m_vaoSTL.destroy();
    m_vaoPLY.destroy();
}

void ModelWidget::showSTLModel(QString stlPath)
{
    STLFileLoader *stlModel = new STLFileLoader(stlPath);
    m_ShowType = ShowType_STL;
    QList<STLTriangle> triangles = stlModel->getSTLData();
    vertices.clear();
    m_pointData.clear();
    QVector<GLfloat> normalList;
    foreach(STLTriangle tri, triangles)
    {
        for (int j = 0; j < 3; ++j) {
            QVector3D vertex = tri.getVertex(j);
            m_pointData.push_back(vertex);
            QVector3D normal = tri.getNormal();
            normalList.push_back(normal.x());
            normalList.push_back(normal.y());
            normalList.push_back(normal.z());
        }
    }
    normalizePointData(m_pointData);
    // 将所有的点数据放入点数据容器中
    QVector<QVector3D>::iterator it = m_pointData.begin();
    QVector<GLfloat> ::iterator itNormal = normalList.begin();
    for (; it != m_pointData.end(); ++it)
    {
        vertices.push_back(it->x());
        vertices.push_back(it->y());
        vertices.push_back(it->z());

        // 装入向量坐标 同一个三角片面 使用相同的法向量
        vertices.push_back(*itNormal);
        ++itNormal;
        vertices.push_back(*itNormal);
        ++itNormal;
        vertices.push_back(*itNormal);
        ++itNormal;
    }

    makeCurrent();

    m_vaoSTL.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[2]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    m_vaoSTL.release();

    update();
}

void ModelWidget::showMultiSTLModel(QStringList stlPaths)
{
    m_ShowType = ShowType_STL;
    vertices.clear();
    m_pointData.clear();
    m_modelRanges.clear();
    QVector<GLfloat> normalList;

    int currentVertexIndex = 0;

    for (auto stlPath : stlPaths)
    {
        ModelRange range;
        range.startIndex = currentVertexIndex;
        range.modelName = QFileInfo(stlPath).fileName();

        STLFileLoader *stlModel = new STLFileLoader(stlPath);
        QList<STLTriangle> triangles = stlModel->getSTLData();
        foreach(STLTriangle tri, triangles)
        {
            for (int j = 0; j < 3; ++j)
            {
                QVector3D vertex = tri.getVertex(j);
                m_pointData.push_back(vertex);
                QVector3D normal = tri.getNormal();
                normalList.push_back(normal.x());
                normalList.push_back(normal.y());
                normalList.push_back(normal.z());
                currentVertexIndex++;
            }
        }

        range.vertexCount = currentVertexIndex - range.startIndex;
        m_modelRanges.append(range);
        delete stlModel;
    }

    normalizePointData(m_pointData);
    // 将所有的点数据放入点数据容器中
    QVector<QVector3D>::iterator it = m_pointData.begin();
    QVector<GLfloat> ::iterator itNormal = normalList.begin();
    for (; it != m_pointData.end(); ++it)
    {
        vertices.push_back(it->x());
        vertices.push_back(it->y());
        vertices.push_back(it->z());

        // 装入向量坐标 同一个三角片面 使用相同的法向量
        vertices.push_back(*itNormal);
        ++itNormal;
        vertices.push_back(*itNormal);
        ++itNormal;
        vertices.push_back(*itNormal);
        ++itNormal;
    }

    makeCurrent();

    m_vaoSTL.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[2]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    m_vaoSTL.release();

    update();
}

void ModelWidget::showPLYModel(QString plyPath)
{
    PLYFileLoader *pointCloud = new PLYFileLoader(plyPath);
    m_ShowType = ShowType_PLY;
    // 将所有的点的位置和颜色数据放入容器中
    pointVertices.clear();
    pointCloud->updateColorBuffer();
    for (int i = 0; i < pointCloud->pointData.size(); i++)
    {
        pointVertices.push_back(pointCloud->pointData[i].x());
        pointVertices.push_back(pointCloud->pointData[i].y());
        pointVertices.push_back(pointCloud->pointData[i].z());

        if(pointCloud->property.contains("amplitude"))
        {
            QColor color = pointCloud->mapValueToColor(pointCloud->amplitudeData[i]);
            pointVertices.push_back(color.redF());
            pointVertices.push_back(color.greenF());
            pointVertices.push_back(color.blueF());
        }
    }

    makeCurrent();

    m_vaoPLY.bind();
    // 生成一个缓冲对象名称
    glGenBuffers(1, &m_vbo[3]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
    if (pointCloud->property.contains("amplitude"))
    {
        // 设置顶点属性指针
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pointVertices.size(), &pointVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 设置颜色属性指针
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else
    {
        // 设置顶点属性指针
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pointVertices.size(), &pointVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    m_vaoPLY.release();

    update();
}

void ModelWidget::showGrid(bool bShow)
{
    m_bDrawGrid = bShow;

    update();
}

void ModelWidget::showCoordinates(bool bShow)
{
    m_bDrawCoordinates = bShow;

    update();
}

void ModelWidget::showHighLight(bool bShow)
{
    m_bDrawHighLight = bShow;

    m_selectedModelIndex = 3;

    update();
}

// 用来绘制OpenGL的窗口，只要有更新发生，这个函数就会被调用
void ModelWidget::paintGL()
{
    // 清屏并设置OpenGL状态
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // QPainter每次绘制时，内部会关闭深度测试，此处重新开启，否则模型会出现孔洞
    glEnable(GL_DEPTH_TEST);

    m_plyShaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(45.0f, (GLfloat)width() / (GLfloat)height(), 0.1f, 200.f);
    m_plyShaderProgram.setUniformValue("model", modelMatrix);
    m_plyShaderProgram.setUniformValue("projection", projection);
    m_plyShaderProgram.setUniformValue("view", m_camera->getViewMatrix());

    // 绘制网格
    if (m_bDrawGrid)
    {
        m_vaoGrid.bind();
        glDrawArrays(GL_LINES, 0, 84);
        m_vaoGrid.release();
    }

    if (ShowType_PLY == m_ShowType)
    {
        m_plyShaderProgram.bind();
        // 绘制点云
        m_vaoPLY.bind();
        glDrawArrays(GL_POINTS, 0, pointVertices.size() / 6);
        m_vaoPLY.release();
        m_plyShaderProgram.release();
    }
    else if (ShowType_STL == m_ShowType)
    {
        if (m_bDrawHighLight)
        {
            // 第一阶段：绘制原模型到模板缓冲
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);  // 总是通过测试
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 通过时替换为1
            glStencilMask(0xFF); // 启用模板写入
        }

        m_stlShaderProgram.bind();
        // 设置MVP矩阵和法线矩阵
        m_stlShaderProgram.setUniformValue("model", modelMatrix);
        m_stlShaderProgram.setUniformValue("projection", projection);
        m_stlShaderProgram.setUniformValue("view", m_camera->getViewMatrix());
        m_stlShaderProgram.setUniformValue("normalMatrix", modelMatrix.inverted().transposed().normalMatrix());
        // 对显示 stl 的着色器进行一定的配置
        m_stlShaderProgram.setUniformValue("isHighLight", 0.0f);
        m_stlShaderProgram.setUniformValue("viewPos", m_camera->m_position);    // 需要动态更新，因为摄像机的位置会变化
        m_stlShaderProgram.setUniformValue("material.ambient", QVector3D(0.2f, 0.8f, 0.8f));
        m_stlShaderProgram.setUniformValue("material.diffuse", QVector3D(0.2f, 0.8f, 0.8f));
        m_stlShaderProgram.setUniformValue("material.specular", QVector3D(0.05f, 0.05f, 0.05f));
        m_stlShaderProgram.setUniformValue("material.shininess", 32.0f);        // 材质

        m_stlShaderProgram.setUniformValue("light1.ambient", QVector3D(0.2f, 0.2f, 0.2f));
        m_stlShaderProgram.setUniformValue("light1.diffuse", QVector3D(0.5f, 0.5f, 0.5f));      // 将光照调暗了一些以搭配场景
        m_stlShaderProgram.setUniformValue("light1.specular", QVector3D(1.0f, 1.0f, 1.0f));
        m_stlShaderProgram.setUniformValue("light1.direction", QVector3D(0.0f, 0.0f, 3.0f));    // 前平行光的方向

        m_stlShaderProgram.setUniformValue("light2.ambient", QVector3D(0.2f, 0.2f, 0.2f));
        m_stlShaderProgram.setUniformValue("light2.diffuse", QVector3D(0.5f, 0.5f, 0.5f));      // 将光照调暗了一些以搭配场景
        m_stlShaderProgram.setUniformValue("light2.specular", QVector3D(1.0f, 1.0f, 1.0f));
        m_stlShaderProgram.setUniformValue("light2.direction", QVector3D(0.0f, 0.0f, -3.0f));   // 后平行光的方向

        if (vertices.size() > 0)
        {
            m_vaoSTL.bind();

            if (m_selectedModelIndex >= 0 && m_selectedModelIndex < m_modelRanges.size())
            {
                ModelRange range = m_modelRanges[m_selectedModelIndex];
                glDrawArrays(GL_TRIANGLES, 0, range.startIndex);
                m_stlShaderProgram.setUniformValue("material.ambient", QVector3D(1.0f, 0.0f, 0.0f));
                glDrawArrays(GL_TRIANGLES, range.startIndex, range.vertexCount);
                m_stlShaderProgram.setUniformValue("material.ambient", QVector3D(0.2f, 0.8f, 0.8f));
                glDrawArrays(GL_TRIANGLES, range.startIndex + range.vertexCount, vertices.size() / 6 - range.startIndex - range.vertexCount);
            }
            else
            {
                // 渲染所有模型
                glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
            }

            m_vaoSTL.release();
        }

        if (m_bDrawHighLight)
        {
            // 第二阶段：绘制放大轮廓
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // 只绘制模板值不等于1的区域
            glStencilMask(0x00); // 禁用模板写入
            glDisable(GL_DEPTH_TEST); // 关键：禁用深度测试

            // 设置轮廓颜色（在着色器中添加新的uniform）
            m_stlShaderProgram.setUniformValue("highLightColor", QVector3D(1.0f, 1.0f, 1.0f)); // 白色
            m_stlShaderProgram.setUniformValue("isHighLight", 1.0f); // 启用颜色覆盖

            // 应用缩放矩阵（建议使用模型矩阵的5%放大）
            QMatrix4x4 outlineModel = modelMatrix;
            outlineModel.scale(1.05f);
            m_stlShaderProgram.setUniformValue("model", outlineModel);

            // 重新绘制放大后的模型
            if (vertices.size() > 0) {
                m_vaoSTL.bind();
                if (m_selectedModelIndex >= 0 && m_selectedModelIndex < m_modelRanges.size())
                {
                    ModelRange range = m_modelRanges[m_selectedModelIndex];
                    glDrawArrays(GL_TRIANGLES, range.startIndex, range.vertexCount);
                }
                m_vaoSTL.release();
            }

            // 恢复状态
            glStencilMask(0xFF);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            m_stlShaderProgram.setUniformValue("model", modelMatrix); // 恢复原始模型矩阵
        }
        m_stlShaderProgram.release();
    }

    // 最后绘制坐标轴，不用再把坐标轴加入模板测试
    if (m_bDrawCoordinates)
    {
        m_plyShaderProgram.bind();
        m_vaoCoordinates.bind();
        glDrawArrays(GL_LINES, 0, 512);
        m_vaoCoordinates.release();
        // 释放ShaderProgram，准备使用QPainter
        m_plyShaderProgram.release();

        // 计算坐标轴末端点的屏幕坐标
        QMatrix4x4 mvp = projection * m_camera->getViewMatrix() * modelMatrix;
        QRect viewport(0, 0, width(), height());

        // 定义坐标轴末端点（模型空间）
        QVector3D xStart(-1, 0, 0); // X轴起始端
        QVector3D xEnd(1, 0, 0); // X轴末端
        QVector3D yEnd(0, 1, 0); // Y轴末端
        QVector3D zStart(0, 0, 1); // Z轴起始端
        QVector3D zEnd(0, 0, -1); // Z轴末端

        // 将3D点投影到屏幕坐标
        QPointF screenMinusX = projectToScreen(xStart, mvp, viewport);
        QPointF screenX = projectToScreen(xEnd, mvp, viewport);
        QPointF screenY = projectToScreen(yEnd, mvp, viewport);
        QPointF screenMinusZ = projectToScreen(zStart, mvp, viewport);
        QPointF screenZ = projectToScreen(zEnd, mvp, viewport);

        // 使用QPainter绘制标签
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setFont(QFont("Arial", 16));
        painter.setPen(Qt::red);
        // 绘制标签（稍微偏移以避免覆盖坐标轴末端）
        painter.drawText(screenMinusX + QPointF(-5, 0), "-x");
        painter.drawText(screenX + QPointF(5, 0), "+x");
        painter.setPen(Qt::green);
        painter.drawText(screenY + QPointF(5, 0), "+y");
        painter.setPen(Qt::blue);
        painter.drawText(screenMinusZ + QPointF(-5, 0), "-z");
        painter.drawText(screenZ + QPointF(5, 0), "+z");
        painter.end();
    }
    m_plyShaderProgram.release();

    // 更新界面，并强制父窗口更新
    update();
    if (parentWidget())
    {
        parentWidget()->update();
    }
}

void ModelWidget::mouseMoveEvent(QMouseEvent * event)// 获取鼠标移动
{
    QPoint p_ab = event->pos();
    translate_point(p_ab);

    QPoint sub_point = p_ab - press_position;
    if (event->buttons() & Qt::LeftButton)
    {
        modelMatrix.setToIdentity();// 转化成单位矩阵

        GLfloat angle_now = qSqrt(qPow(sub_point.x(), 2) + qPow(sub_point.y(), 2)) / 5;

        modelMatrix.rotate(angle_now, -sub_point.y(), sub_point.x(), 0.0);
        modelMatrix = modelMatrix * modelUse;

        // 对旋转进行叠加
        modelSave.setToIdentity();// 转化成单位矩阵，
        modelSave.rotate(angle_now, -sub_point.y(), sub_point.x(), 0.0);
        modelSave = modelSave * modelUse;
    }
}

void ModelWidget::wheelEvent(QWheelEvent * event)
{
    // 获得滚轮转动的方向和被转动的距离
    QPoint offset = event->angleDelta();
    m_camera->processMouseScroll(offset.y() / -20);
}

void ModelWidget::mousePressEvent(QMouseEvent * event)
{
    // 单击
    QPoint p_ab = event->pos();
    // 如果是鼠标左键按下
    if (event->button() == Qt::LeftButton)
    {
        modelUse = modelSave;
        translate_point(p_ab);
        press_position = p_ab;
    }
}

void ModelWidget::translate_point(QPoint &p_ab)
{
    int x = p_ab.x() - this->width() / 2;
    int y = -(p_ab.y() - this->width() / 2);

    // 由于点击的时候  原点是在 窗口左上角，y轴正半轴朝下，而咱们opengl中， 原点是在下面，
    // y轴正半轴朝上，咱们这里要转换成  原点在在窗口中间，y轴正半轴 朝上；
    p_ab.setX(x);
    p_ab.setY(y);
}

// 辅助函数：将3D点投影到屏幕坐标
QPointF ModelWidget::projectToScreen(const QVector3D &point, const QMatrix4x4 &mvp, const QRect &viewport)
{
    // 将3D点转换为裁剪空间
    QVector4D clipPos = mvp * QVector4D(point, 1.0f);
    if (clipPos.w() == 0) return QPointF();

    // 透视除法，得到标准化设备坐标（NDC）
    QVector3D ndc(clipPos.x() / clipPos.w(), clipPos.y() / clipPos.w(), clipPos.z() / clipPos.w());

    // 将NDC映射到视口坐标
    float x = (ndc.x() + 1.0f) * 0.5f * viewport.width() + viewport.x();
    float y = (1.0f - ndc.y()) * 0.5f * viewport.height() + viewport.y(); // Y轴翻转

    return QPointF(x, y);
}
