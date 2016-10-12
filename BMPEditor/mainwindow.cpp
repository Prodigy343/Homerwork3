#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    actualPath = "";
    ui->setupUi(this);
    setWindowTitle("Tarea 3");
    QVBoxLayout * lbl = new QVBoxLayout(this);
    lbl->addWidget(ui->ImgLabel);
    ui->scrollAreaWidgetContents->setLayout(lbl);
    CFilter.setWindowTitle("Custom Filter");
    undo = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* ---------------  Histogram   --------------- */

void MainWindow::on_Histogram_clicked(){
    if(actualPath != ""){
        update_Histogram();
        H.show();
    }else{
        QMessageBox::information(this, tr("Error"),"Load a BMP first..");
    }
}

/* -------------- Updating view --------------- */

void MainWindow::update_Labels(){
    if(actualPath != ""){
        string p = bmp.pathfile;
        int f = 0;
        for(int i=p.size();i>0;i--)if(p[i]==47){f = i;break;}
        p = p.substr(f+1,p.size()-f);

        ui->NameLabel->setText(QString::fromStdString(p));
        ui->WidthLabel->setText(QString::number(bmp.infoheader.width));
        ui->HeightLabel->setText(QString::number(bmp.infoheader.height));
        ui->BppLabel->setText(QString::number(bmp.infoheader.bpp));
        ui->ImgSizeLabel->setText(QString::number(bmp.infoheader.width*(bmp.infoheader.height*3+bmp.pad)));
    }
}

void MainWindow::update_ImgLabel(){
    uchar bulto[] = {1,1,1};
    QImage xx(bulto,1,1,QImage::Format_RGB888);
    ui->ImgLabel->setPixmap(QPixmap::fromImage(xx));
    ui->ImgLabel->setPixmap(QPixmap::fromImage(bmp.getQImageFromRawData()));
}

void MainWindow::on_Resize_clicked(){
    ui->ImgLabel->setPixmap(QPixmap::fromImage(bmp.getQImageFromRawData().scaled(1040,430)));
}

void MainWindow::update_Histogram(){
    bmp.Histogram();
    H.updateplot(bmp.red,bmp.green,bmp.blue);
}

/* ------------    Load  Save    -------------- */

void MainWindow::on_OpenFile_clicked(){

    QString x = QFileDialog::getOpenFileName(this, tr("Open File"),"C://", "BMP files (*.bmp)");
    actualPath = x.toLocal8Bit().constData();

    if(x != ""){
        bmp.BMPFileLoad(x);
        ui->PathLabel->setText(x);  //Showing file path
        bmp.Histogram();           //Filling up histogram vectors red, green & blue
        bmp.rotate(0);             //Filling up "result" Data Structure
        update_ImgLabel();         //Updating Label
        update_Labels();
    }
}

void MainWindow::on_SaveFile_clicked(){
    if(actualPath != ""){
        QString fileN = QFileDialog::getSaveFileName(this, tr("Save File Name"), "C://", "BMP files (*.bmp)");
        if(fileN != ""){
            ui->SPathLabel->setText(fileN);
            bmp.write(fileN);
            QMessageBox::information(this, tr("Success"),"File saved succesfully");
        }
    }else{
        QMessageBox::information(this, tr("Error"),"Load a BMP first..");
    }
}

/* ---- Transformations, Basic Operations ---- */

void MainWindow::on_RY_clicked()
{
    if(actualPath != ""){
        undo = 0;
        bmp.ReflectionY();
        bmp.rotate(0);
        update_ImgLabel();
    }
}

void MainWindow::on_RX_clicked(){
    if(actualPath != ""){
        undo = 0;
        bmp.ReflectionX();
        bmp.rotate(0);
        update_ImgLabel();
    }
}

void MainWindow::on_Rotate_clicked()
{
    undo = 0;
    QString x = ui->Degree->text();
    int deg = x.toInt()%360;

    bmp.ant_rotacion = -deg;

    if(deg <= -90){
        while(deg <= -90){
            deg+=90;
            bmp.r90();
            bmp.r90();
            bmp.r90();
        }
    }

    if(deg >= 90){
        for(int i=0;i<deg/90;i++)
            bmp.r90();
        deg %= 90;
    }

    bmp.rotate(-deg);

    update_ImgLabel();
}

void MainWindow::on_Negative_clicked(){
    if(actualPath != ""){
        undo = 0;
        bmp.Negative();
        bmp.rotate(0);
        update_Histogram();
        update_ImgLabel();
    }
}

void MainWindow::on_Contrast_clicked()
{
    QString x = ui->contrastLabel->text();

    if(actualPath != "" && x!= ""){
        float c = x.toFloat();
        if(c>0.0f){
            undo = 0;
            bmp.Contrast(c);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }else{
            QMessageBox::information(this, tr("Error"),"Invalid contrast parameter..");
        }
    }
}

void MainWindow::on_Brightness_clicked(){
    QString x = ui->brightLabel->text();
    if(actualPath != "" && x!= ""){
        int b = x.toInt();
        if(b>=-128 && b<=127){
            undo = 0;
            bmp.Brightness(b);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }else
            QMessageBox::information(this, tr("Error"),"Invalid bright parameter..");
    }
}

void MainWindow::on_Equalization_clicked()
{
    if(actualPath != ""){
        undo = 0;
        bmp.Equalization();
        bmp.rotate(0);
        update_Histogram();
        update_ImgLabel();
    }
}

void MainWindow::on_Scale_clicked()
{
    if(actualPath != ""){
        int x = ui->newWidth->text().toInt(),y = ui->newHeight->text().toInt();
        if(x>0&&y>0&&x<4300&&y<2900&&(x!=bmp.infoheader.width||y!=bmp.infoheader.height)){
            undo = 0;
            INTERPOLATIONS type;
            if(ui->Nearest->isChecked())
                type = NEAREST;
            else
                type = BILINEAR;

            bmp.Scale(bmp.img,bmp.auximg,x,y,type);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
            update_Labels();
        }else
            QMessageBox::information(this, tr("Error"),"Invalid scale parameter..");
    }
}

void MainWindow::on_ZoomIn_clicked()
{
    if(actualPath != ""){
        if(bmp.zoomVar<2){
            undo = 0;
            bmp.zoomVar++;
            int x = bmp.infoheader.width*2 , y = bmp.infoheader.height*2;
            INTERPOLATIONS type;
            if(ui->Nearest->isChecked())
                type = NEAREST;
            else
                type = BILINEAR;

            bmp.Scale(bmp.img,bmp.auximg,x,y,type);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }
    }
}

void MainWindow::on_ZoomOut_clicked()
{
    if(actualPath != ""){
        if(bmp.zoomVar>-2){
            undo = 0;
            int x = bmp.infoheader.width/2 , y = bmp.infoheader.height/2;
            bmp.zoomVar--;
            INTERPOLATIONS type;
            if(ui->Nearest->isChecked())
                type = NEAREST;
            else
                type = BILINEAR;

            bmp.Scale(bmp.img,bmp.auximg,x,y,type);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }
    }
}

void MainWindow::Box_generator(int n, float kernel[]){
    float aux = n*n;
    int auxn = n*n;
    for(int i=0;i<auxn;i++)kernel[i] = 1.0/aux;
}

void MainWindow::Gauss_generator(int n, float kernel[]){
    int aux = n*n;
    if(n == 3){
        int v3[3];
        v3[0] = 1; v3[1] = 2; v3[2] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v3[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v3[i]*v3[j]/div;

        return ;
    }
    if(n == 5){
        int v5[5];
        v5[0] = 1; v5[1] = 4; v5[2] = 6; v5[3] = 4; v5[4] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v5[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v5[i]*v5[j]/div;
        return ;
    }
    if(n == 7){
        int v7[7];
        v7[0] = 1; v7[1] = 6; v7[2] = 15; v7[3] = 20; v7[4] = 15; v7[5] = 6; v7[6] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v7[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v7[i]*v7[j]/div;
        return ;
    }
    if(n == 9){
        int v9[9];
        v9[0] = 1; v9[1] = 8; v9[2] = 28; v9[3] = 56; v9[4] = 70; v9[5] = 56; v9[6] = 28; v9[7] = 8; v9[8] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v9[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v9[i]*v9[j]/div;
        return ;
    }
    if(n == 11){
        int v11[11];
        v11[0] = 1; v11[1] = 10; v11[2] = 45; v11[3] = 120; v11[4] = 210; v11[5] = 252; v11[6] = 210; v11[7] = 120; v11[8] = 45; v11[9] = 10; v11[10] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v11[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v11[i]*v11[j]/div;
        return ;
    }
    if(n == 13){
        int v13[13];
        v13[0] = 1; v13[1] = 12; v13[2] = 66; v13[3] = 220; v13[4] = 495; v13[5] = 792; v13[6] = 924; v13[7] = 792; v13[8] = 495; v13[9] = 220; v13[10] = 66; v13[11] = 12; v13[12] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v13[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v13[i]*v13[j]/div;
        return ;
    }
    if(n == 15){
        int v15[15];
        v15[0] = 1; v15[1] = 14; v15[2] = 91; v15[3] = 364; v15[4] = 1001; v15[5] = 2002; v15[6] = 3003; v15[7] = 3432; v15[8] = 3003; v15[9] = 2002; v15[10] = 1001; v15[11] = 364; v15[12] = 91; v15[13] = 14; v15[14] = 1;
        float div = 0;
        for(int i=0;i<n;i++)div += v15[i];
        div = div*div;
        for(int i=0;i<n;i++)
            for(int j=0;j<n;j++)
                kernel[i*n+j] = v15[i]*v15[j]/div;
        return ;
    }
}

void MainWindow::Sharpening_generator(int n, float kernel[]){
    for(int i=0;i<n*n;i++)kernel[i] = -1;
    kernel[n/2*(n+n/2)] = n*n;
}

void MainWindow::Prewitt_generator(int n, float kernel1[], float kernel2[]){
    int aux = n/2;
    for (int i=0;i<n*n;i++)kernel2[i] = kernel1[i] = 0;

    for (int i=0; i < n/2; i++){
        for (int j=0; j < n; j++){
                kernel1[j*n+i]=-aux;
                kernel1[j*n+(n-1-i)]=aux;
        }
        aux--;
    }

    aux = n/2;
    for (int i=0; i < n/2; i++){
        for (int j=0; j < n; j++){
            kernel2[i*n+j]=-aux;
            kernel2[n*(n-1-i)+j]=aux;
        }
        aux--;
    }
}

void MainWindow::Sobel_generator(int n, float kernel1[], float kernel2[]){
    for (int i=0;i<n*n;i++)kernel2[i] = kernel1[i] = 0;
    int aux = n/2;
    int gen = 1;

    for (int i=0; i < n/2; i++){
        for (int j=0; j < n; j++){
                kernel1[j*n+i]=aux;
                kernel1[j*n+(n-1-i)]=-aux;
        if (j<n/2)
            aux = aux + gen;
        else
            aux = aux - gen;
        }
    }

    aux = n/2;
    gen = 1;
    for (int i=0; i < n/2; i++){
        for (int j=0; j < n; j++){
                kernel2[i*n+j]=-aux;
                kernel2[n*(n-1-i)+j]=aux;
                //kernel1[i*n+(n-1-i)]=-aux;
        if (j<n/2)
            aux = aux + gen;
        else
            aux = aux - gen;
        }
    }

}

bool MainWindow::Custom_generator(int n,float kernel[],int &cx,int &cy){
    CFilter.reload(n);
    CFilter.show();

    if (!CFilter.exec())
        return false;
    else{
        cx = CFilter.cx-1;
        cy = CFilter.cy-1;
        for (int k=0;k<n;k++)
            for (int c=0;c<n;c++){
                kernel[k*n+c] = CFilter.kernel[k*n+c];
            }

        return true;
    }
}

void MainWindow::on_ApplyFilter_clicked()
{
    if(actualPath != ""){
        undo = 0;
        int n = ui->Dim->currentIndex(), filterType = ui->FilterType->currentIndex();
        int acum = 1, cx,cy;

        for(int i=0;i<=n;i++)acum+=2;

        float aux[20*20],aux2[20*20];
        cx = cy = acum/2;

        if(filterType == 0){
            Box_generator(acum,aux);
            bmp.Smoothing(bmp.img,bmp.buff,aux,cx,cy,acum);
        }else if(filterType == 1){
            Gauss_generator(acum,aux);
            bmp.Smoothing(bmp.img,bmp.buff,aux,cx,cy,acum);
        }else if(filterType == 2){
            Sharpening_generator(acum,aux);
            bmp.Smoothing(bmp.img,bmp.buff,aux,cx,cy,acum);
        }else if(filterType == 3){
            Prewitt_generator(acum,aux,aux2);
            bmp.EdgeDetection(bmp.img,bmp.buff,aux,aux2,acum,cx,cy);
        }else if(filterType == 4){
            Sobel_generator(acum,aux,aux2);
            bmp.EdgeDetection(bmp.img,bmp.buff,aux,aux2,acum,cx,cy);
        }else{
            if(Custom_generator(acum,aux,cx,cy))
                bmp.Smoothing(bmp.img,bmp.buff,aux,cx,cy,acum);
        }

        bmp.rotate(0);
        update_Histogram();
        update_ImgLabel();
    }
}

void MainWindow::on_Thresholding_clicked()
{
    if(actualPath != ""){
        undo = 0;
        int t = ui->threshold->text().toInt();
        if(t < 255 && t > 0){
            bmp.Thresholding(t);
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }else
            QMessageBox::information(this, tr("Error"),"Out of range..");
    }
}

void MainWindow::on_Undo_clicked()
{
    if(actualPath != ""){
        if(!undo){
            undo = 1;
            bmp.undo();
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }
   }
}

void MainWindow::on_Redo_clicked()
{
    if(actualPath != ""){
        if(undo){
            undo = 0;
            bmp.redo();
            bmp.rotate(0);
            update_Histogram();
            update_ImgLabel();
        }
    }
}

