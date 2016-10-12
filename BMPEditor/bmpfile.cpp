#include "bmpfile.h"

/* ---------------- LOADING BMP 1,4,8,24 ---------------- */

BMPFile::BMPFile(){
    result =    new uchar[100000000];
    img =       new int[100000000];
    buff =      new uchar[100000000];
    auximg =    new int[100000000];
    zoomVar = 0;
    prox_rotacion = 0;
    ant_rotacion = 0;
}

BMPFile::~BMPFile(){
    /*free(palette);
    free(img);
    free(result);*/
}

void BMPFile::BMPFileLoad(QString x){

    pathfile = x.toLocal8Bit().constData();

    pad = 0;
    rotacion = 0;
    FILE *f;
    f = fopen(pathfile.c_str(),"rb+");

    fread(&type,2,1,f);
    fread(&header,12,1,f);
    fread(&infoheader,40,1,f);

    for(int i=0;i<100000000;i++)result[i] = 0;
    for(int i=0;i<100000000;i++)img[i] = 0;


    if(infoheader.bpp == 1){
        palette = (uchar*) malloc (2*4);

        fseek(f, infoheader.size - 40, SEEK_CUR);// dropping useless bytes.. between InfoHeader & Palette
        fread(palette,2*4,1,f);
        fseek(f, header.offset, SEEK_SET);// going at the beginning of pixel data..

        if(!(infoheader.width*infoheader.bpp%32)){	// NO PADDING
            for(int i=infoheader.height-1; i>=0 ;i--)
                for(unsigned int j=0; j<infoheader.width/8 ;j++){
                    fread(&buffer,1,1,f);
                    c1to24(buffer,i,j,infoheader.width/8);
                }
        }else{
            int RD 	= infoheader.width; //easy math
            int M32	= 32*((RD/32)+1);
            int PD 	= M32 - RD;
            int BF 	= RD/8;
            int BP 	= PD/8;

            if((BF + BP) ==((RD + PD)/8)){
                for(int i=infoheader.height-1; i>=0 ;i--){
                    for(unsigned int j=0; j<infoheader.width/8 ;j++){
                        fread(&buffer,1,1,f);
                        c1to24(buffer,i,j,infoheader.width/8);
                    }
                    fseek(f, BP, 1);
                }
            }else{
                pad = (4-(infoheader.width*3%4))%4;
                for(int i=infoheader.height-1; i>=0 ;i--){
                    long long int pos = i*((infoheader.width)*3+pad);
                    for(unsigned int j=0; j<infoheader.width/8 ;j++){
                        fread(&buffer,1,1,f);
                        p1to24(buffer,pos,0);
                    }
                    fread(&buffer,1,1,f);
                    p1to24(buffer,pos,1);
                    fseek(f, BP, 1);
                }
            }
        }

    }else if(infoheader.bpp == 4){
        palette = (uchar*) malloc (16*4);

        fseek(f, infoheader.size - 40, SEEK_CUR);// dropping useless bytes.. between InfoHeader & Palette
        fread(palette,16*4,1,f);
        fseek(f, header.offset, SEEK_SET);// going at the beginning of pixel data..

        if(!(infoheader.width*infoheader.bpp%32)){	// normal case
            for(int i=infoheader.height-1; i>=0 ;i--)
                for(unsigned int j=0; j<infoheader.width/2 ;j++){
                    fread(&buffer,1,1,f);
                    c4to24(buffer,i,j,infoheader.width/2);
                }
        }else{
            int RD 	= infoheader.width*4; //ez math
            int M32	= 32*((RD/32)+1);
            int PD 	= M32 - RD;
            int BF 	= RD/8;
            int BP 	= PD/8;

            if((BF + BP) ==((RD + PD)/8)){ //external padding
                for(int i=infoheader.height-1; i>=0 ;i--){
                    for(unsigned int j=0; j<infoheader.width/2 ;j++){
                        fread(&buffer,1,1,f);
                        c4to24(buffer,i,j,infoheader.width/2);
                    }
                    fseek(f, BP, 1);
                }
            }else{ //internal padding
                pad = (4-(infoheader.width*3%4))%4;
                for(int i=infoheader.height-1; i>=0 ;i--){
                    long long int pos = i*((infoheader.width)*3+pad);
                    for(unsigned int j=0; j<infoheader.width/2 ;j++){
                        fread(&buffer,1,1,f);
                        p4to24(buffer,pos,0);
                    }
                    fread(&buffer,1,1,f);
                    p4to24(buffer,pos,1);
                    fseek(f, BP, 1);
                }
            }
        }

    }else if(infoheader.bpp == 8){
        long long int pos;
        palette = (uchar*) malloc (256*4);

        fseek(f, infoheader.size - 40, SEEK_CUR);// dropping useless bytes.. between InfoHeader & Palette
        fread(palette,256*4,1,f);
        fseek(f, header.offset, SEEK_SET);// going at the beginning of pixel data..

        if(!(infoheader.width*infoheader.bpp%32)){// normal case
            for(int i=infoheader.height-1; i>=0 ;i--){
                for(unsigned int j=0; j<infoheader.width ;j++){
                    fread(&buffer,1,1,f);
                    pos = ((i * infoheader.width) + j)*3;
                    img[pos]    = palette[buffer*4+2];
                    img[pos+1]  = palette[buffer*4+1];
                    img[pos+2]  = palette[buffer*4];
                }
            }
        }else{// padding case
            int auxpad = 4-(infoheader.width%4);
            pad = (4-(infoheader.width*3%4))%4;
            for(int i=infoheader.height-1; i>=0 ;i--){
                for(unsigned int j=0,k=0; j<infoheader.width ;j++,k+=3){
                    fread(&buffer,1,1,f);
                    pos = (i*(infoheader.width*3+pad));
                    img[pos+k]    = palette[buffer*4+2];
                    img[pos+k+1]  = palette[buffer*4+1];
                    img[pos+k+2]  = palette[buffer*4];
                }
                fseek(f,auxpad,1); // dropping padding bytes
            }
        }
    }else{

        long int pos;
        fseek(f, header.offset, SEEK_SET);
        if(!(infoheader.width*infoheader.height%32)){ // normal case
            for(int i=infoheader.height-1; i>=0 ;i--){
                for(unsigned int j=0; j<infoheader.width;j++){
                    pos = ((i * infoheader.width) + j)*3;

                    fread(&img[pos+2],1,1,f);
                    fread(&img[pos+1],1,1,f);
                    fread(&img[pos],1,1,f);
                }
            }
        }else{  // padding case
            pad = (4-(infoheader.width*3%4))%4;
            for(int i=infoheader.height-1; i>=0 ;i--){
                for(unsigned int j=0; j<infoheader.width*3;j++){
                    pos = (i*(infoheader.width*3+pad))+j;
                    if(j%3==0)fread(&img[pos+2],1,1,f);
                    if(j%3==1)fread(&img[pos],1,1,f);
                    if(j%3==2)fread(&img[pos-2],1,1,f);
                }
                fseek(f,pad,1);
            }
        }
    }
    fclose(f);
}

/* ------------ TRANSFORMATIONS & OPERATIONS ------------ */

void BMPFile::rotate(int grades){
    int stay = grades;
    while (rotacion>=360){
        rotacion-=360;
    }

    grades += rotacion;
    rotacion += stay;

    if(grades>0){
        while (grades>=90){
            grades-=90;
            r90();
            r90();
            r90();
            rotacion -= 90;
        }
    }else{
        while (grades < -90){
            grades+=90;
            r90();
            rotacion += 90;
        }
    }

    int width = infoheader.width;
    int height = infoheader.height;
    pad = (4 - (infoheader.width*3%4))%4;
    resolucion = (int)(ceil(sqrt((height*height)+(width*width))));
    double seno = sin((grades*3.1416)/180.0);
    double tang = tan(((grades/2)*3.1416)/180.0);

    uchar r, g, b;

    int result_pad = (4 - (resolucion*3%4))%4;
    for(int i=0;i<resolucion*(resolucion+result_pad)*3;i++)result[i] = 241;

    int x0 = (int)(floor(width/2));
    int y0 = (int)(floor(height/2));
    int x2 = 0;
    int y2 = 0;

    for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                r = cast8Bits(img[j*(width*3+pad)+i*3]);
                g = cast8Bits(img[j*(width*3+pad)+i*3+1]);
                b = cast8Bits(img[j*(width*3+pad)+i*3+2]);

                x2 = (int)((i-x0) - tang*(y0-j));
                y2 = (int)(y0-j);

                y2 = (int)((seno*x2) + y2);

                x2 = (int)((x2) - tang*(y2))+((resolucion/2));
                y2 = (int)(-y2)+((resolucion/2));

                if(x2 < resolucion && y2 < resolucion && x2 > -1 && y2 > -1){
                    result[y2*(resolucion*3+result_pad)+x2*3] = r;
                    result[y2*(resolucion*3+result_pad)+x2*3+1] = g;
                    result[y2*(resolucion*3+result_pad)+x2*3+2] = b;
                }
                x2 = 0;
                y2 = 0;

            }
    }
}

void BMPFile::r90(){
    for(int i=0;i<100000000;i++)result[i] = 0;
    int hhpad = 4-(infoheader.height*3%4);

    pad = 4-(infoheader.width*3%4);
    pad %= 4;
    hhpad %= 4;

    int column = infoheader.height* 3 + hhpad;
    int row = infoheader.width * 3 + pad;

    for (int q = 0; q < infoheader.width; q++) {
        for (int z = 0; z < infoheader.height; z++) {
            result[column*q + 0 + z * 3] = cast8Bits(img[row*(infoheader.height - 1 - z) + 0 + q * 3]);
            result[column*q + 1 + z * 3] = cast8Bits(img[row*(infoheader.height - 1 - z) + 1 + q * 3]);
            result[column*q + 2 + z * 3] = cast8Bits(img[row*(infoheader.height - 1 - z) + 2 + q * 3]);
        }
    }

    swap(infoheader.width , infoheader.height);
    pad = hhpad;
    for(int i=0;i<100000000;i++)img[i] = result[i];
}

void BMPFile::Negative(){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    for(unsigned int i=0;i<(infoheader.width*infoheader.height*3);i++)img[i] = cast8Bits(255 - img[i]);
}

void BMPFile::ReflectionX(){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    int row = infoheader.width * 3 + pad;
    for (int a = 0; a < infoheader.height/2; a++){
        for (int b = 0; b < infoheader.width*3; b++){
            swap(img[row*(infoheader.height - 1 - a) + b], img[row*a + b]);
        }
    }

}

void BMPFile::ReflectionY(){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    int row = infoheader.width * 3 + pad;
    for (int a = 0; a < infoheader.height; a++) {
        for (int b = 0; b < infoheader.width/2; b++) {
            swap(img[row * a + infoheader.width * 3 - 1 - 2 - b * 3] , img[row * a + 0 + b * 3]);
            swap(img[row * a + infoheader.width * 3 - 1 - 1 - b * 3] , img[row * a + 1 + b * 3]);
            swap(img[row * a + infoheader.width * 3 - 1 - 0 - b * 3] , img[row * a + 2 + b * 3]);
        }
    }
}

void BMPFile::Histogram(){
    long long int pos;
    red = vector<int>(256,0);
    green = vector<int>(256,0);
    blue = vector<int>(256,0);
    for(int i=0;i<infoheader.height;i++){
        for(int j=0;j<infoheader.width*3;j+=3){
            pos = i*(infoheader.width*3+pad)+j;
            red[cast8Bits(img[pos])]++;
            green[cast8Bits(img[pos+1])]++;
            blue[cast8Bits(img[pos+2])]++;
        }
    }
}

void BMPFile::Equalization(){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    long long int NM = infoheader.height * infoheader.width;
    long long int eqR[256];
    long long int eqG[256];
    long long int eqB[256];

    eqR[0] = 0;
    eqR[255] = 255;

    eqG[0] = 0;
    eqG[255] = 255;

    eqB[0] = 0;
    eqB[255] = 255;

    long long int acR = red[0];
    long long int acG = green[0];
    long long int acB = blue[0];

    for(int i=1;i<255;i++){
        eqR[i] = (acR * 255) / NM;
        acR += red[i];

        eqG[i] = (acG * 255) / NM;
        acG += green[i];

        eqB[i] = (acB * 255) / NM;
        acB += blue[i];
    }

    int pos;
    for(int i=0;i<infoheader.height;i++)
        for(int j=0;j<infoheader.width*3;j+=3){
            pos = i*(infoheader.width*3+pad)+j;
            img[pos]    =   eqR[img[pos]];
            img[pos+1]  =   eqG[img[pos+1]];
            img[pos+2]  =   eqB[img[pos+2]];
        }
}

void BMPFile::Thresholding(int Threshold){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    for(int i=0;i<infoheader.height;i++){
        for(int j=0;j<infoheader.width;j++){
            int grayVal = (cast8Bits(img[i*(infoheader.width*3+pad)+(j*3)]) + cast8Bits(img[i*(infoheader.width*3+pad)+(j*3)+1]) + cast8Bits(img[i*(infoheader.width*3+pad)+(j*3)]+2))/3;
            if(grayVal>Threshold){
                img[i*(infoheader.width*3+pad)+(j*3)] = 255;
                img[i*(infoheader.width*3+pad)+(j*3)+1] = 255;
                img[i*(infoheader.width*3+pad)+(j*3)+2] = 255;
            }else{
                img[i*(infoheader.width*3+pad)+(j*3)] = 0;
                img[i*(infoheader.width*3+pad)+(j*3)+1] = 0;
                img[i*(infoheader.width*3+pad)+(j*3)+2] = 0;
            }
        }
    }
}

void BMPFile::Brightness(int b){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    for(uint i=0;i<infoheader.height;i++)for(uint j=0;j<infoheader.width*3;j++)img[i*(infoheader.width*3+pad)+j] += b;
}

void BMPFile::Contrast(float c){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    for(uint i=0;i<infoheader.height;i++)for(uint j=0;j<infoheader.width*3;j++)img[i*(infoheader.width*3+pad)+j] = c*(img[i*(infoheader.width*3+pad)+j]-128)+128;
}

void BMPFile::Scale(int *&src,int *&dst,int newWidth,int newHeight,int type){

    int newPad  = (4-(newWidth*3%4))%4;
    if(type){

    float x_ratio = ((float)(infoheader.width-1))/newWidth;
    float y_ratio = ((float)(infoheader.height-1))/newHeight;

    int p1[3],p2[3],p3[3],p4[3],r,g,b,x,y,index;

    float x_diff, y_diff;

    for (int i=0;i<newHeight;i++) {
        for (int j=0;j<newWidth;j++) {
            x = (int)(x_ratio * j) ;
            y = (int)(y_ratio * i) ;
            x_diff = (x_ratio * j) - x ;
            y_diff = (y_ratio * i) - y ;

            index = (y*(infoheader.width*3+pad)+x*3);
            p1[0] = src[index];
            p1[1] = src[index+1];
            p1[2] = src[index+2];

            p2[0] = src[index+3];
            p2[1] = src[index+4];
            p2[2] = src[index+5];

            index += infoheader.width*3+pad;
            p3[0] = src[index];
            p3[1] = src[index+1];
            p3[2] = src[index+2];

            p4[0] = src[index+3];
            p4[1] = src[index+4];
            p4[2] = src[index+5];

            r = p1[0]*(1-x_diff)*(1-y_diff) + p2[0]*(x_diff)*(1-y_diff) +
                  p3[0]*(y_diff)*(1-x_diff)   + p4[0]*(x_diff*y_diff);

            g = p1[1]*(1-x_diff)*(1-y_diff) + p2[1]*(x_diff)*(1-y_diff) +
                    p3[1]*(y_diff)*(1-x_diff)   + p4[1]*(x_diff*y_diff);

            b = p1[2]*(1-x_diff)*(1-y_diff) + p2[2]*(x_diff)*(1-y_diff) +
               p3[2]*(y_diff)*(1-x_diff)   + p4[2]*(x_diff*y_diff);

            dst[i*(newWidth*3+newPad)+(j*3)] = r;
            dst[i*(newWidth*3+newPad)+(j*3)+1] = g;
            dst[i*(newWidth*3+newPad)+(j*3)+2] = b;
        }
    }

    }else{
        int x_ratio = (int)((infoheader.width<<16)/newWidth) +1;
        int y_ratio = (int)((infoheader.height<<16)/newHeight) +1;

        int x2, y2 ;
        for (int i=0;i<newHeight;i++) {
            for (int j=0;j<newWidth;j++) {
                x2 = ((j*x_ratio)>>16) ;
                y2 = ((i*y_ratio)>>16) ;
                dst[(i*(newWidth*3+newPad))+j*3] = src[(y2*(infoheader.width*3+pad))+x2*3] ;
                dst[(i*(newWidth*3+newPad))+j*3+1] = src[(y2*(infoheader.width*3+pad))+x2*3+1] ;
                dst[(i*(newWidth*3+newPad))+j*3+2] = src[(y2*(infoheader.width*3+pad))+x2*3+2] ;
            }
        }
    }

    ant_rotacion = 0;
    prox_rotacion = 0;
    ant_pad = pad;
    ant_w   = infoheader.width;
    ant_h   = infoheader.height;

    pad = newPad;
    infoheader.width = newWidth;
    infoheader.height = newHeight;

    for(int i=0;i<max((int)(infoheader.height*(infoheader.width*3+pad)),ant_h*(ant_w*3+ant_pad));i++)swap(src[i],dst[i]);
}

void BMPFile::Smoothing(int *&src,uchar *&dest,float *kernel,int iKernel,int jKernel,int nKernel){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    int act,div;
    float R,G,B;
    float cKernel = kernel[iKernel*nKernel+jKernel];
    for(int i=0;i<infoheader.height;i++)
        for(int j=0;j<infoheader.width;j++){
            div = 0;
            R = G = B = 0;
            for(int x=0;x<nKernel;x++){
                for(int y=0;y<nKernel;y++){
                    int posi = iKernel-x,posj = jKernel-y;
                    int iactual = i - posi,jactual = j - posj;
                    if(!(x==iKernel&&y==jKernel)&&iactual>=0&&iactual<infoheader.height&&jactual>=0&&jactual<infoheader.width){
                        R += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        div++;

                    }else{
                        if(iactual<0)
                            iactual = abs(iactual);
                        if(jactual<0)
                            jactual = abs(jactual);
                        if(iactual>=infoheader.height)
                            iactual = infoheader.height - 1 - (iactual - (infoheader.height - 1));
                        if(jactual>=infoheader.width)
                            jactual = infoheader.width - 1 - (jactual - (infoheader.width - 1));

                        R += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B += kernel[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        div++;
                    }

                }
            }
            dest[i*(infoheader.width*3+pad) + j*3] = cast8Bits((int)R);
            dest[i*(infoheader.width*3+pad) + j*3+1] = cast8Bits((int)G);
            dest[i*(infoheader.width*3+pad) + j*3+2] = cast8Bits((int)B);
        }

    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)src[i] = dest[i];
}

void BMPFile::EdgeDetection(int *&src, uchar *&dest,float *kernel1,float *kernel2,int nKernel,int iKernel,int jKernel){
    ant_rotacion = 0;
    prox_rotacion = 0;
    prox_w = 0;
    prox_h = 0;
    prox_pad = 0;
    ant_w = 0;
    ant_h = 0;
    ant_pad = 0;
    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)auximg[i] = img[i];

    int act,div;
    float R1,G1,B1,R2,G2,B2;
    float cKernel1 = kernel1[iKernel*nKernel+jKernel];
    float cKernel2 = kernel2[iKernel*nKernel+jKernel];
    for(int i=0;i<infoheader.height;i++)
        for(int j=0;j<infoheader.width;j++){
            div = 0;
            R1 = G1 = B1 = R2 = G2 = B2 = 0;
            for(int x=0;x<nKernel;x++){
                for(int y=0;y<nKernel;y++){
                    int posi = iKernel-x,posj = jKernel-y;
                    int iactual = i - posi,jactual = j - posj;

                    if(!(x==iKernel&&y==jKernel)&&iactual>=0&&iactual<infoheader.height&&jactual>=0&&jactual<infoheader.width){
                        R1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        R2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        div++;

                    }else{
                        if(iactual<0)
                            iactual = abs(iactual);
                        if(jactual<0)
                            jactual = abs(jactual);
                        if(iactual>=infoheader.height)
                            iactual = infoheader.height - 1 - (iactual - (infoheader.height - 1));
                        if(jactual>=infoheader.width)
                            jactual = infoheader.width - 1 - (jactual - (infoheader.width - 1));

                        R1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B1 += kernel1[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        R2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3];
                        G2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+1];
                        B2 += kernel2[x*nKernel+y]*src[iactual*(infoheader.width*3+pad) + jactual*3+2];
                        div++;
                    }

                }
            }
            dest[i*(infoheader.width*3+pad) + j*3] = cast8Bits((int)sqrt(R1*R1+R2*R2));
            dest[i*(infoheader.width*3+pad) + j*3+1] = cast8Bits((int)sqrt(G1*G1+G2*G2));
            dest[i*(infoheader.width*3+pad) + j*3+2] = cast8Bits((int)sqrt(B1*B1+B2*B2));
        }

    for(int i=0;i<infoheader.height*(infoheader.width*3+pad);i++)src[i] = dest[i];
}

void BMPFile::write(QString x){
    /* changing format (Nbits => 24bits)*/
    if(zoomVar > 0){
        infoheader.width<<2*zoomVar;
        infoheader.height<<2*zoomVar;

        Scale(img,auximg,infoheader.width,infoheader.height,1);

    }else if(zoomVar < 0){
        infoheader.width>>2*zoomVar;
        infoheader.height>>2*zoomVar;

        Scale(img,auximg,infoheader.width,infoheader.height,1);
    }else{

    }

    header.size = header.size - infoheader.imgsize;
    infoheader.imgsize = infoheader.width*infoheader.height*3;
    infoheader.bpp = 24;
    infoheader.ncolours = 0;
    infoheader.imxtcolours = 0;
    int pad = (4 - (infoheader.width*3%4))%4;
    if(pad!=0)infoheader.imgsize += pad*infoheader.height;

    header.size = header.size + infoheader.imgsize;

    string wpath = x.toLocal8Bit().constData();
    FILE *f;
    f = fopen(wpath.c_str(), "wb+");

    fseek(f, 0, SEEK_SET);

    fwrite(&type,2,1,f);
    fwrite(&header,12,1,f);
    fwrite(&infoheader,40,1,f);

    unsigned long long int pos;
    unsigned char *garbage = (unsigned char*) malloc (pad);
    unsigned char *more = (unsigned char*) malloc (header.offset-54);

    fwrite(&*more,header.offset-54,1,f);

    //qInfo() << pad;

    for(int i=infoheader.height-1; i>=0 ;i--){
        for(int j=0; j<infoheader.width*3;j++){
            if((j+1)%3==0){
                fwrite(&img[(i*(infoheader.width*3+pad))+j],1,1,f);
                fwrite(&img[(i*(infoheader.width*3+pad))+j-1],1,1,f);
                fwrite(&img[(i*(infoheader.width*3+pad))+j-2],1,1,f);
            }
        }
        fwrite(&*garbage,pad,1,f);
    }

    fclose(f);
}

/* ----------------- AUXILIAR METHODS ------------------- */

void BMPFile::undo(){
    if(ant_rotacion != 0){
        prox_rotacion = ant_rotacion;
        rotate(-ant_rotacion);
        ant_rotacion = 0;
    }else{
        for(int i=0;i<max((int)(infoheader.height*(infoheader.width*3+pad)),ant_h*(ant_w*3+ant_pad));i++)swap(img[i],auximg[i]);
        if(ant_w != 0 ){
          /*prox_h = ant_h;
            prox_w = ant_w;
            prox_pad = ant_pad;*/
            prox_h =    infoheader.height;
            prox_w =    infoheader.width;
            prox_pad =  pad;
            infoheader.height = ant_h;
            infoheader.width = ant_w;
            pad = ant_pad;
            ant_h = 0;
            ant_w = 0;
            ant_pad = 0;
        }
    }

}

void BMPFile::redo(){
    if(prox_rotacion != 0){
        ant_rotacion = prox_rotacion;
        rotate(prox_rotacion);
        prox_rotacion = 0;
    }else{
        for(int i=0;i<max((int)(infoheader.height*(infoheader.width*3+pad)),prox_h*(prox_w*3+prox_pad));i++)swap(img[i],auximg[i]);
        if(prox_w != 0 ){
            ant_h =    infoheader.height;
            ant_w =    infoheader.width;
            ant_pad =  pad;
            infoheader.height = prox_h;
            infoheader.width = prox_w;
            pad = prox_pad;
            prox_h = 0;
            prox_w = 0;
            prox_pad = 0;
        }
    }
}

void BMPFile::c1to24(unsigned char B,int i,int j,int w){
    quint8 mask = 128;
    for(int d=7,l=0;d>=0;d--,l++){
        int index = ((mask>>l)&B)>>d;
        img[((i*w+j)*8+l)*3]    = palette[index*4+2];
        img[((i*w+j)*8+l)*3+1]  = palette[index*4+1];
        img[((i*w+j)*8+l)*3+2]  = palette[index*4];
    }
}

void BMPFile::c4to24(unsigned char B,int i,int j,int w){
    int mask = 240;
    for(int d=1,l=0;d>=0;d--,l++){
        int index = ((mask>>4*l)&B)>>(4*d);
        img[((i*w+j)*2+l)*3]    = palette[index*4+2];
        img[((i*w+j)*2+l)*3+1]  = palette[index*4+1];
        img[((i*w+j)*2+l)*3+2]  = palette[index*4];
    }
}

void BMPFile::p1to24(unsigned char B,long long int &pos,int top){
    int mask = 128;
    for(int d=7,l=0;d>=top;d--,l++){
        int index = ((mask>>l)&B)>>d;
        img[pos++]     = palette[index*4+2];
        img[pos++]    = palette[index*4+1];
        img[pos++]    = palette[index*4];
    }
}

void BMPFile::p4to24(unsigned char B,long long int &pos,int top){
    int mask = 240;
    for(int d=1,l=0;d>=top;d--,l++){
        int index = ((mask>>4*l)&B)>>(4*d);
        img[pos++]    = palette[index*4+2];
        img[pos++]    = palette[index*4+1];
        img[pos++]    = palette[index*4];
    }
}

uchar BMPFile::cast8Bits(int value){
    if(value<0) return (uchar)0;
    else if(value>255)return (uchar)255;
    else return (uchar)value;
}

QImage BMPFile::getQImageFromRawData(){
    QImage aux(result, resolucion, resolucion, QImage::Format_RGB888);
    return aux;
}

