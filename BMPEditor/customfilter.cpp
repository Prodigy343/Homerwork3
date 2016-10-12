#include "customfilter.h"
#include "ui_customfilter.h"

CustomFilter::CustomFilter(QWidget *parent,int n) : QDialog(parent),ui(new Ui::CustomFilter)
{
    ui->setupUi(this);
    FilterSize = n;
    ui->GridFilter->setColumnCount(FilterSize);
    ui->GridFilter->setRowCount(FilterSize);
    ui->GridFilter->resizeColumnsToContents();
    ui->CX->setText(0);
    ui->CY->setText(0);
}

CustomFilter::~CustomFilter()
{
    delete ui;
}

void CustomFilter::reload(int n)
{
    FilterSize = n;
    ui->GridFilter->setColumnCount(n);
    ui->GridFilter->setRowCount(n);
    ui->GridFilter->resizeColumnsToContents();
    ui->CX->setText(0);
    ui->CY->setText(0);
}

void CustomFilter::on_buttonBox_accepted()
{
    QString a_ver;
    cx = ui->CX->text().toInt();
    cy = ui->CY->text().toInt();
    for (int k = 0; k < FilterSize; k++){
        for (int c = 0; c < FilterSize; c++){
            if (ui->GridFilter->item(k,c) == NULL)
                kernel[k*FilterSize+c] = 0;
            else{
                a_ver = ui->GridFilter->item(k,c)->text();
                kernel[k*FilterSize+c] = a_ver.toFloat();
            }
        }
    }
    accept();
}

void CustomFilter::on_buttonBox_rejected()
{
    reject();
}


void CustomFilter::on_Fill_clicked()
{
    for(int i=0;i<FilterSize;i++)
        for(int j=0;j<FilterSize;j++){
            QTableWidgetItem *item = ui->GridFilter->item(i, j);
                if(!item) {
                    item = new QTableWidgetItem();
                    ui->GridFilter->setItem(i, j, item);
                }
            item->setText(ui->FillValue->text());
        }
}
