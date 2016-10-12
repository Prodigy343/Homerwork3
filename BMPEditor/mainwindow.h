#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QtDebug>
#include <iostream>
#include <QPixmap>
#include <QPicture>
#include <QString>
#include <QLayout>
#include <QVBoxLayout>

#include "customfilter.h"
#include "histogram.h"
#include "bmpfile.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum INTERPOLATIONS{
        NEAREST,
        BILINEAR
    };
    explicit MainWindow(QWidget *parent = 0);
    string actualPath;
    ~MainWindow();

private slots:
    /* Histogram */
    void on_Histogram_clicked();

    /* Updating view */
    void update_Labels();
    void update_Histogram();
    void update_ImgLabel();
    //void update_ImgLabelR();
    void on_Resize_clicked();

    /* Load Save */
    void on_OpenFile_clicked();
    void on_SaveFile_clicked();

    /* Transformations, Basic Operations*/
    void on_RY_clicked();           //Reflection Y axis
    void on_RX_clicked();           //Reflection X axis
    void on_Rotate_clicked();       //Free Rotation
    void on_Negative_clicked();
    void on_Contrast_clicked();
    void on_Brightness_clicked();
    void on_Equalization_clicked();


    /* Kernel Generators*/
    void Box_generator(int n, float kernel[]);
    void Gauss_generator(int n, float kernel[]);
    void Sharpening_generator(int n, float kernel[]);
    void Prewitt_generator(int n, float kernel1[], float kernel2[]);
    void Sobel_generator(int n, float kernel1[], float kernel2[]);
    bool Custom_generator(int n,float kernel[],int &cx,int &cy);


    void on_Scale_clicked();

    void on_ZoomIn_clicked();

    void on_ZoomOut_clicked();

    void on_ApplyFilter_clicked();

    void on_Thresholding_clicked();

    void on_Undo_clicked();

    void on_Redo_clicked();

private:

    Ui::MainWindow *ui;
    CustomFilter CFilter;
    BMPFile bmp;
    histogram H;
    bool undo;
};

#endif // MAINWINDOW_H
