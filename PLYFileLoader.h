#ifndef PLYFILELOADER_H
#define PLYFILELOADER_H

#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMap>

enum PLYFormat
{
    PLY_Unkonw = -1,            // 未知格式
    PLY_ASCII,                  // ascii格式
    PLY_BinaryBigEndian,        // 二进制大端格式
    PLY_BinaryLittleEndian      // 二进制小端格式
};

struct PLYPropertys
{
    PLYFormat Format = PLY_Unkonw;
    int ElementVertex = -1;                         // 顶点个数
    int FaceNum = -1;                               // 面片个数
//    std::vector<std::string> VertexPropertys;       // 点属性
//    std::vector<std::string> FacePropertys;         // 面片属性
};

class PLYFileLoader
{
public:
    /**
     * @brief PLYFileLoader
     * @param filename  点云文件路径
     */
    PLYFileLoader(QString fileName);
    ~PLYFileLoader();

    QVector3D maxCoordinate;        // 最大坐标点
    QVector3D minCoordinate;        // 最小坐标大
    QVector<QVector3D> pointData;   // 点数据
    QVector<float> amplitudeData;   // 幅度值
    float m_min;
    float m_max;
    int pointCount;
    QStringList property;
    PLYFormat plyFormat;

    int mLevelCount = 350;
    QVector<QRgb> mColorBuffer;
    QMap<double, QColor> mColorStops;
public:
    bool loadPLY(const QString &fileName);//从文件中读取点云数据 并按比例 映射到 -1.0 -- 1.0 的范围之内
    void readPLY_Head(char *s);
    void readPLY_ASCII(char *s);
    void handlePointDate(QVector<QVector3D> data);// 规格化点云数据， 映射到 -1.0 -- 1.0 的范围之内
    void getMaxCoordinate();// 分别计算出 x y z 的最大值 组合成一个点；
    void getMinCoordinate();//  分别计算 x y z 的最小值 组合成一个；
    void getCenterPoint(QVector3D &vec); // 获取中心点
    float getFactor();
    void updateColorBuffer();
    QColor mapValueToColor(double value);
};

#endif // PLYFILELOADER_H
