#include "histogram.h"
#include "ui_histogram.h"

histogram::histogram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::histogram)
{
    ui->setupUi(this);
    vector<int> A(256,0),B(256,0),C(256,0);
    histogram::makeplot(A,B,C);
    g = b = r = 1;
}

histogram::~histogram()
{
    delete ui;
}

void histogram::makeplot(vector<int> R,vector<int> G,vector<int> B){
    // add two new graphs and set their look:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    //ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::green)); // line color red for second graph

    ui->customPlot->addGraph();
    ui->customPlot->graph(2)->setPen(QPen(Qt::red)); // line color red for second graph

    // generate some points of data (y0 for first, y1 for second graph):
    //QVector<double> x(256), y0(256), y1(256) ,y2(256);

    QVector<double> x(256), y0(256), y1(256) ,y2(256);

    ui->customPlot->yAxis->setLabel("N pixels");

    for (int i=0; i<256; ++i){
      x[i] = i;
      y0[i] = R[i];
      y1[i] = G[i];
      y2[i] = B[i];
    }
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    ui->customPlot->graph(0)->setData(x, y0);
    ui->customPlot->graph(1)->setData(x, y1);
    ui->customPlot->graph(2)->setData(x, y2);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->customPlot->graph(0)->rescaleAxes(true);
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->graph(2)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void histogram::updateplot(vector<int> R,vector<int> G,vector<int> B){
    QVector<double> x(256), y0(256), y1(256) ,y2(256);

    ui->customPlot->yAxis->setLabel("N pixels");

    for (int i=0; i<256; ++i){
      x[i] = i;
      y0[i] = R[i];
      y1[i] = G[i];
      y2[i] = B[i];
    }

    ui->customPlot->graph(0)->setData(x, y0);
    ui->customPlot->graph(1)->setData(x, y1);
    ui->customPlot->graph(2)->setData(x, y2);
    ui->customPlot->graph(0)->rescaleAxes(true);
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->graph(2)->rescaleAxes(true);

    ui->customPlot->replot();
}

void histogram::on_RedCheck_clicked()
{
    r?ui->customPlot->graph(2)->setVisible(0):ui->customPlot->graph(2)->setVisible(1);
    r = (r?0:1);
    ui->customPlot->replot();
}

void histogram::on_GreenCheck_clicked()
{
    g?ui->customPlot->graph(1)->setVisible(0):ui->customPlot->graph(1)->setVisible(1);
    g = (g?0:1);
    ui->customPlot->replot();
}

void histogram::on_BlueCheck_clicked()
{
    b?ui->customPlot->graph(0)->setVisible(0):ui->customPlot->graph(0)->setVisible(1);
    b = (b?0:1);
    ui->customPlot->replot();
}
