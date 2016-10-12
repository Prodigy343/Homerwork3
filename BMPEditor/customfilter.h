#ifndef CUSTOMFILTER_H
#define CUSTOMFILTER_H

#include <QDialog>

namespace Ui {
class CustomFilter;
}

class CustomFilter : public QDialog
{
    Q_OBJECT

public:
    explicit CustomFilter(QWidget *parent = 0,int n = 3);
    ~CustomFilter();
    int FilterSize,cx,cy;
    float kernel[20*20];
    void reload(int n);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();


    void on_Fill_clicked();

private:
    Ui::CustomFilter *ui;
};

#endif // CUSTOMFILTER_H
