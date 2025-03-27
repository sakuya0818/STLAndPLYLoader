#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QSurfaceFormat fmt;
	fmt.setDepthBufferSize(24);

	if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
		// 桌面 OpenGL（Core Profile）
		qDebug("Requesting 3.3 core context");
		fmt.setVersion(3, 3);
		fmt.setProfile(QSurfaceFormat::CoreProfile);
		fmt.setOption(QSurfaceFormat::DeprecatedFunctions, false); // 禁用旧函数
	} else {
		// OpenGL ES
		qDebug("Requesting 3.0 context");
		fmt.setVersion(3, 0);
		fmt.setRenderableType(QSurfaceFormat::OpenGLES); // 显式声明为 ES
	}

	QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;
    w.show();
    return a.exec();
}
