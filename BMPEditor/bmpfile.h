#ifndef BMPFILE_H
#define BMPFILE_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <QtGlobal>
#include <QCoreApplication>
#include <QString>
#include <QPixmap>
#include <QtDebug>
#include <QImage>
#include <QFile>
#include <QDataStream>
#include <QVector>

using namespace std;

struct Header {
    //unsigned short int type;
    unsigned int size;
    unsigned short int r1,r2;
    unsigned int offset;
};

struct InfoHeader {
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short int planes;
    unsigned short int bpp;
    unsigned int compression;
    unsigned int imgsize;
    unsigned int xr;
    unsigned int yr;
    unsigned int ncolours;
    unsigned int imxtcolours;
};

class BMPFile{

    public:
        //=============DS=============/

        /* BMP DS Basic */
        string pathfile;
        unsigned short int type;
        Header header;
        InfoHeader infoheader;
        uchar *palette;

        /* Histrogram DS */
        vector<int> red,green,blue;

        /* RGB Buffers */
        int *img,*auximg, zoomVar;
        unsigned char buffer,*result,*buff;

        /* Auxiliars */
        int ant_rotacion,prox_rotacion,rotacion,resolucion,pad,ant_pad,ant_w,ant_h,prox_pad,prox_w,prox_h;



        //=============METHODS=============//

        /* Basic constructor, BMP Loader 1,4,8,24 bits*/

        BMPFile();
        ~BMPFile();
        //BMPFile(QString x);
        void BMPFileLoad(QString x);


        /* Transformations & Operations */

        void r90();
        void Negative();
        void ReflectionX();
        void ReflectionY();
        void Equalization();
        void Thresholding(int Threshold);
        void Brightness(int b);
        void Contrast(float c);
        void Scale(int *&src,int *&dest,int newWidth,int newHeight, int type);
        void Smoothing(int *&src,uchar *&dest,float *kernel,int iKernel,int jKernel,int nKernel);
        void EdgeDetection(int *&src, uchar *&dest,float *kernel1,float *kernel2,int nKernel,int iKernel,int jKernel);
        void rotate(int grades);

        /* Auxiliar methods */

        void undo();
        void redo();
        void Histogram();                                       // Filling up Histogram DS needed
        uchar cast8Bits(int value);                             // return 0 <= value <= 255
        QImage getQImageFromRawData();                          // Setting up QImage variable with uchar *img
        void c1to24(unsigned char B,int i,int j,int w);
        void c4to24(unsigned char B,int i,int j,int w);
        void p1to24(unsigned char B,long long int &pos,int top);
        void p4to24(unsigned char B,long long int &pos,int top);

        /* Save BMP 24 bits */

        void write(QString x);

};

#endif // BMPFILE_H
