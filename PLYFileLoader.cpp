#include "PLYFileLoader.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <string.h>
#if defined(WIN32) || defined(WIN64)

#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

PLYFileLoader::PLYFileLoader(QString filename)
{
    loadPLY(filename);
}

PLYFileLoader::~PLYFileLoader()
{

}

bool PLYFileLoader::loadPLY(const QString &fileName)
{
#if defined(WIN32) || defined(WIN64)
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开文件" << fileName;
    }

    // 将文件映射到内存上
    uchar* data = file.map(0, file.size());
    char *s = strdup((char *)data);
    file.unmap(data);
    file.close();
#else
    QFile file(fileName);
    // QFile的map在Linux上文件过大时会无法映射，使用mmap
    int fd = open(fileName.toStdString().c_str(), O_RDONLY);
    if (fd == -1)
    {
        qDebug() << "failed to open file descriptor";
    }
    void *data = mmap(nullptr, file.size(), PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED)
    {
        qDebug() << "failed to mmap file";
        close(fd);
    }

    char *s = strdup((char *)data);
    munmap(data, file.size());
    file.close();
#endif

    // ply文件前几行中定义了文件是二进制还是ascii存储，还定义了点的个数
    readPLY_Head(s);

    // 根据文件存储格式读取点
    switch (plyFormat)
    {
        case PLY_ASCII:
            readPLY_ASCII(s);
            break;
        case PLY_BinaryBigEndian:

            break;
        case PLY_BinaryLittleEndian:

            break;
        default:
            break;
    }

    free(s);

    getMaxCoordinate();
    getMinCoordinate();
    //将所有的点想0.0.0点 移动，使中值点  位于0.0.0点，这样就不用考虑旋转轴的问题了
    QVector3D centerPoint;
    getCenterPoint(centerPoint);

    for (int i = 0; i < pointData.size(); i++)
    {
        pointData[i].setX(pointData[i].x() - centerPoint.x());
        pointData[i].setY(pointData[i].y() - centerPoint.y());
        pointData[i].setZ(pointData[i].z() - centerPoint.z());
    }
    //更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();

    //建立一个 以可以容纳所有点的 正方体 然后 让顶点映射到 0.5（所有点都进行映射）
    float factor = getFactor();//获取变换因子
    for (int i = 0; i < pointData.size(); i++)
    {
        pointData[i].setX(pointData[i].x() * factor);
        pointData[i].setY(pointData[i].y() * factor);
        pointData[i].setZ(pointData[i].z() * factor);
    }

    //更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();

//    QDataStream stream(&file);
//    stream.skipRawData(in.pos());
//    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

//    float number;
//    stream >> number;
//    qDebug() << "Float: " << number;

    // 根据文件格式修改大小端字节顺序
//    in.setByteOrder(QDataStream::LittleEndian);
//    file.close();
//    return foundVertex;

    return true;
}

void PLYFileLoader::readPLY_Head(char *s)
{
    // 分割每一行
    char *token = std::strtok(s, "\n");
    while(token != NULL)
    {
        QString line = QString(token);
        if (line.startsWith(QString("format")))
        {
            QStringList listFormat = line.split(QString(" "));
            if (listFormat.count() >= 3)
            {
                // 读取当前文件的存储格式
                QString format = listFormat.at(1);
                if (0 == format.compare("ascii"))
                {
                    plyFormat = PLYFormat::PLY_ASCII;
                }
                else if (0 == format.compare("binary_big_endian"))
                {
                    plyFormat = PLYFormat::PLY_BinaryBigEndian;
                }
                else if (0 == format.compare("binary_little_endian"))
                {
                    plyFormat = PLYFormat::PLY_BinaryLittleEndian;
                }
            }
        }
        else if (line.startsWith(QString("element vertex")))
        {
            // 读取点数
            pointCount = line.split(QString(" ")).last().toInt();
        }
        else if (line.startsWith(QString("property float")))
        {
            property.push_back(line.split(QString(" ")).last());
        }
        else if (line == "end_header")
        {
            // 读取定义结尾
            break;
        }

        token = std::strtok(NULL, "\n");
    }
}

void PLYFileLoader::readPLY_ASCII(char *s)
{
    // 继续分割每一行
    char *token = std::strtok(NULL, "\n");
    int cnt = 0;
    QVector3D data;
    pointData.reserve(pointCount);
    bool bAmplitude = property.contains("amplitude");
    while(token != NULL && cnt < pointCount)
    {
        data.setX(std::strtof(token, &token));
        data.setY(std::strtof(token, &token));
        data.setZ(std::strtof(token, &token));
        pointData.push_back(data);
        if (bAmplitude)
        {
            amplitudeData.push_back(std::strtof(token, nullptr));
        }
        token = std::strtok(NULL, "\n");
        ++cnt;
    }

    if (bAmplitude)
    {
        // 求出最大最小幅度值
        QVector<float> temp = amplitudeData;
        std::sort(temp.begin(), temp.end());
        m_min= temp.first();
        m_max = temp.last();
    }
}

void PLYFileLoader::handlePointDate(QVector<QVector3D> data) {
    pointData = data;

    getMaxCoordinate();
    getMinCoordinate();
    //将所有的点想0.0.0点 移动，使中值点  位于0.0.0点，这样就不用考虑旋转轴的问题了
    QVector3D centerPoint;
    getCenterPoint(centerPoint);

    for (int i = 0; i < pointData.size(); i++)
    {
        pointData[i].setX(pointData[i].x() - centerPoint.x());
        pointData[i].setY(pointData[i].y() - centerPoint.y());
        pointData[i].setZ(pointData[i].z() - centerPoint.z());
    }
    //更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();

    //建立一个 以可以容纳所有点的 正方体 然后 让顶点映射到 0.5（所有点都进行映射）
    float factor = getFactor();//获取变换因子
    for (int i = 0; i < pointData.size(); i++)
    {
        pointData[i].setX(pointData[i].x() * factor);
        pointData[i].setY(pointData[i].y() * factor);
        pointData[i].setZ(pointData[i].z() * factor);
    }

    //更新最大 最小值点 坐标s
    getMaxCoordinate();
    getMinCoordinate();
}

float PLYFileLoader::getFactor()
{
    float max = 0;
    if (max <= maxCoordinate.x())
    {
        max = maxCoordinate.x();
    }
    if (max <= maxCoordinate.y())
    {
        max = maxCoordinate.y();
    }
    if (max <= maxCoordinate.z())
    {
        max = maxCoordinate.z();
    }
    return 0.5 / max;
}

void PLYFileLoader::getMaxCoordinate()
{
    if (0 == pointData.size())
    {
        return;
    }
    QVector3D vec;
    vec.setX(pointData[0].x());
    vec.setY(pointData[0].y());
    vec.setZ(pointData[0].z());

    for (int i = 0; i < pointData.size(); i++)
    {
        if (vec.x() < pointData[i].x())
        {
            vec.setX(pointData[i].x());
        }
        if (vec.y() < pointData[i].y())
        {
            vec.setY(pointData[i].y());
        }
        if (vec.z() < pointData[i].z())
        {
            vec.setZ(pointData[i].z());
        }
    }
    maxCoordinate = vec;
}

void PLYFileLoader::getMinCoordinate()
{
    if (0 == pointData.size())
    {
        return;
    }
    QVector3D vec;

    vec.setX(pointData[0].x());
    vec.setY(pointData[0].y());
    vec.setZ(pointData[0].z());

    for (int i = 0; i < pointData.size(); i++)
    {
        if (vec.x() > pointData[i].x())
        {
            vec.setX(pointData[i].x());
        }
        if (vec.y() > pointData[i].y())
        {
            vec.setY(pointData[i].y());
        }
        if (vec.z() > pointData[i].z())
        {
            vec.setZ(pointData[i].z());
        }
    }
    minCoordinate = vec;
}

void PLYFileLoader::getCenterPoint(QVector3D &vec)
{
    if (0 == pointData.size())
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
    vec.setX((maxCoordinate.x() + minCoordinate.x()) / 2);
    vec.setY((maxCoordinate.y() + minCoordinate.y()) / 2);
    vec.setZ((maxCoordinate.z() + minCoordinate.z()) / 2);
}

// 初始化颜色数组，插值色带的颜色值
void PLYFileLoader::updateColorBuffer()
{
    // gpJet模式的插值
    mColorStops.insert(0, QColor(0, 0, 100));
    mColorStops.insert(0.15, QColor(0, 50, 255));
    mColorStops.insert(0.35, QColor(0, 255, 255));
    mColorStops.insert(0.65, QColor(255, 255, 0));
    mColorStops.insert(0.85, QColor(255, 30, 0));
    mColorStops.insert(1, QColor(100, 0, 0));

    if (mColorBuffer.size() != mLevelCount)
        mColorBuffer.resize(mLevelCount);
    if (mColorStops.size() > 1)
    {
        double indexToPosFactor = 1.0/double(mLevelCount-1);
        for (int i=0; i<mLevelCount; ++i)
        {
            double position = i*indexToPosFactor;
            QMap<double, QColor>::const_iterator it = mColorStops.lowerBound(position);
            if (it == mColorStops.constEnd()) // position is on or after last stop, use color of last stop
            {
                mColorBuffer[i] = std::prev(it).value().rgba();
            }
            else if (it == mColorStops.constBegin()) // position is on or before first stop, use color of first stop
            {
                mColorBuffer[i] = it.value().rgba();
            }
            else // position is in between stops (or on an intermediate stop), interpolate color
            {
                QMap<double, QColor>::const_iterator high = it;
                QMap<double, QColor>::const_iterator low = std::prev(it);
                double t = (position-low.key())/(high.key()-low.key()); // interpolation factor 0..1
                mColorBuffer[i] = qRgb(int( ((1-t)*low.value().red() + t*high.value().red()) ),
                                       int( ((1-t)*low.value().green() + t*high.value().green()) ),
                                       int( ((1-t)*low.value().blue() + t*high.value().blue())) );
            }
        }
    }
    else if (mColorStops.size() == 1)
    {
        const QRgb rgb = mColorStops.constBegin().value().rgb();
        const double alpha = mColorStops.constBegin().value().alphaF();
        mColorBuffer.fill(qRgba(int(qRed(rgb)*alpha), int(qGreen(rgb)*alpha), int(qBlue(rgb)*alpha), int(255*alpha)));
    }
    else // mColorStops is empty, fill color buffer with black
    {
        mColorBuffer.fill(qRgb(0, 0, 0));
    }
}

// 将值映射到颜色
QColor PLYFileLoader::mapValueToColor(double value)
{
//    // 计算插值因子
//    double factor = (value - m_min) / (m_max - m_min);

//    // 计算色调 (hue)，蓝色为240度，绿色为120度，红色为0度
//    double hue = 240.0 - factor * 240.0;

//    // 创建颜色，饱和度和亮度设为1.0，透明度设为1.0
//    return QColor::fromHsvF(hue / 360.0, 1.0, 1.0, 1.0);

    const double posToIndexFactor = (mLevelCount-1)/(m_max - m_min);
    int index = int((value - m_min) * posToIndexFactor);
    index = qBound(0, index, mLevelCount-1);

    return mColorBuffer.at(index);
}
