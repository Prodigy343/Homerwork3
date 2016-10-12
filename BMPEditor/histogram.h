#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <vector>
#include <iostream>

using namespace std;

namespace Ui {
class histogram;
}

class histogram : public QWidget
{
    Q_OBJECT

public:
    explicit histogram(QWidget *parent = 0);
    void makeplot(vector<int> R,vector<int> G,vector<int> B);
    void updateplot(vector<int> R,vector<int> G,vector<int> B);
    ~histogram();

private slots:
    void on_RedCheck_clicked();

    void on_GreenCheck_clicked();

    void on_BlueCheck_clicked();

private:
    Ui::histogram *ui;
    bool r,g,b;
};

#endif // HISTOGRAM_H
