#ifndef MODELCONTROL_H
#define MODELCONTROL_H

#include <QWidget>

namespace Ui {
class ModelControl;
}

class ModelControl : public QWidget
{
    Q_OBJECT

public:
    explicit ModelControl(QWidget *parent = nullptr);
    ~ModelControl();

signals:
    void sigShowSTLModel(QString stlPath);

    void sigShowPLYModel(QString plyPath);

    void sigShowGrid(bool bShow);

    void sigShowCoordinates(bool bShow);

private slots:
    void on_Btn_ImportSTL_clicked();

    void on_checkBoxGrid_stateChanged(int arg1);

    void on_checkBoxCoordinate_stateChanged(int arg1);

    void on_Btn_ImportPLY_clicked();

private:
    Ui::ModelControl *ui;
};

#endif // MODELCONTROL_H
