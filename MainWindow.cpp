#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    showMaximized();

    QList<int> sizes;
    //设置QSplitter比例2:1，在保持比例的情况下，绝对值要尽量大
    sizes << 20000 << 10000;
    ui->splitter->setSizes(sizes);

    connect(ui->modelControl, &ModelControl::sigShowSTLModel, ui->modelWidget, &ModelWidget::showSTLModel);
    connect(ui->modelControl, &ModelControl::sigShowPLYModel, ui->modelWidget, &ModelWidget::showPLYModel);
    connect(ui->modelControl, &ModelControl::sigShowGrid, ui->modelWidget, &ModelWidget::showGrid);
    connect(ui->modelControl, &ModelControl::sigShowCoordinates, ui->modelWidget, &ModelWidget::showCoordinates);
    connect(ui->modelControl, &ModelControl::sigShowHighLight, ui->modelWidget, &ModelWidget::showHighLight);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

