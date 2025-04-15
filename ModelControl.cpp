#include "ModelControl.h"
#include "ui_ModelControl.h"

#include <QFileDialog>
#include <QDebug>

ModelControl::ModelControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelControl)
{
    ui->setupUi(this);
}

ModelControl::~ModelControl()
{
    delete ui;
}

void ModelControl::on_Btn_ImportSTL_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open  file"), "", tr("point cloud files( *.stl) ;; All files (*.*)"));
    if (!fileName.isEmpty())
    {
        emit sigShowSTLModel(fileName);
    }
}

void ModelControl::on_Btn_ImportPLY_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open  file"), "", tr("point cloud files( *.ply) ;; All files (*.*)"));
    if (!fileName.isEmpty())
    {
        emit sigShowPLYModel(fileName);
    }
}

void ModelControl::on_checkBoxGrid_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        emit sigShowGrid(true);
    }
    else
    {
        emit sigShowGrid(false);
    }
}


void ModelControl::on_checkBoxCoordinate_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        emit sigShowCoordinates(true);
    }
    else
    {
        emit sigShowCoordinates(false);
    }
}

void ModelControl::on_checkBoxHighLight_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        emit sigShowHighLight(true);
    }
    else
    {
        emit sigShowHighLight(false);
    }
}
