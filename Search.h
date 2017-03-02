//
// Created by vadim on 02.03.17.
//

#ifndef VJDETECTOR_SEARCH_H
#define VJDETECTOR_SEARCH_H
#include <iostream>
#include <vector>
#include "Classifiers.h"
#include "Pictures.h"
#include "omp.h"

#define HAARS 6

using std::cout;
using std::endl;
using std::vector;

typedef unsigned char byte;

struct FILENAME_PALETTE_AND_SIZE{
    vector<vector<int>> palette;
    size_t height;
    size_t width;
};

struct region{
    size_t firstX;
    size_t firstY;
    size_t secondX;
    size_t secondY;
};

class Search{
private:
    const char* fileName;
    std::string extension;
    FILENAME_PALETTE_AND_SIZE image;
    Classifiers *wclass;
    std::vector<region> windowsWFace;
    MY_RGB* rgbPalette;
    bool _check = false;

public:

    void setFile(const char* fileName, std::string extension){
        this->fileName = fileName;
        this->extension = extension;
        OpenFileAndSavePalette(&image);
    }

    //äåòåêòèŵîâàíèå
    void searchFaceOnImage(const char* classifiers, size_t MinH, size_t MinW){
        wclass = new Classifiers(classifiers);
        wclass->init();
        double scale = 1;
        //for (size_t i = 1; i < 11;i++){
            SlidingWindow(scale, MinH, MinW);
        //    scale *= 1.56;
         //   cout << scale << endl;
        //}
        delete wclass;
    }

    void saveResults(){
        cout << "Visualization..." << endl;
        IplImage* image = cvLoadImage(fileName, 1);
        for (int i = 0; i < windowsWFace.size();i++){
            cvRectangle(image, cvPoint(windowsWFace.at(i).firstX, windowsWFace.at(i).firstY), cvPoint(windowsWFace.at(i).secondX, windowsWFace.at(i).secondY), cvScalar(0, 0, 255), 2);
        }
        cvResetImageROI(image);
        cvNamedWindow("ROI", CV_WINDOW_AUTOSIZE);
        cvShowImage("ROI", image);
        cvWaitKey(0);
        cvReleaseImage(&image);
        cvDestroyAllWindows();
    }

protected:
    //ñêîëüçÿùåå îêíî
    void SlidingWindow(double scale, size_t MinH, size_t MinW){
        size_t scaleH = (size_t)((MinH*scale)+0.5);
        std::cout<<scaleH<<std::endl;
        size_t scaleW = (size_t)((MinW*scale)+0.5);
        std::cout<<scaleW<<std::endl;
        cout << scaleH << "	;	" << scaleW << endl;
        for (size_t x = 0; x < image.height - scaleH; x += (size_t)(2*scale)){
            for (size_t y = 0; y < image.width - scaleW; y += (size_t)(2*scale)){
                std::vector<std::vector<int>> rectMatr;
                rect(image.palette, x, y, scaleH, scaleW, &rectMatr);
                _check = StrongClassifier(rectMatr, scale);
                if (_check){
                    _check = true;
                    region w;
                    w.firstX = y;
                    w.firstY = x;
                    w.secondX = y + scaleH;
                    w.secondY = x + scaleW;
                    windowsWFace.push_back(w);
                    IplImage* image = cvLoadImage(fileName, 1);
                    for (int i = 0; i < windowsWFace.size(); i++){
                        cvRectangle(image, cvPoint(windowsWFace[i].firstX, windowsWFace[i].firstY), cvPoint(windowsWFace[i].secondX, windowsWFace[i].secondY), cvScalar(0, 0, 255), 2);
                    }
                    cvResetImageROI(image);
                    cvNamedWindow("ROI", CV_WINDOW_AUTOSIZE);
                    cvShowImage("ROI", image);
                    cvWaitKey(2);
                    cvReleaseImage(&image);
                    cvDestroyAllWindows();
                }
            }
        }
    }
    //ñèëüíûé êëàññèôèêàòîŵ
    bool StrongClassifier(std::vector<std::vector<int>> &window, double scale){
        double teor = 0;
        double fact = 0;
        for (size_t i = 0; i < wclass->size();i++){
            teor += wclass->get(i).coefficientA;
            RESPONSE_COORDINATES_SIZE_AND_TYPE optr = wclass->get(i).optimalResponse;
            int h = weak_classifier(optr, window, scale);
            fact += wclass->get(i).coefficientA * (double) h;
        }
        return fact >= (teor / 2.0)? true : false;
    }

    //ñëàáûé êëàññèôèêàòîŵ èç áàçû
    int weak_classifier(RESPONSE_COORDINATES_SIZE_AND_TYPE &opt, std::vector<std::vector<int>> &window, double scale){
        std::vector<std::vector<int>> feature;
        //std::cout<<(size_t)((double)opt.coordinate_x*scale)<<" "<<(size_t)((double)opt.coordinate_y*scale)<<" "<<(size_t)(scale*(double)opt.height)<<" "<<(size_t)(scale*(double)opt.height)<<std::endl;
        rect(window, (size_t)((double)opt.coordinate_x*scale ), (size_t)((double)opt.coordinate_y*scale), (size_t)(scale*(double)opt.height), (size_t)(scale*(double)opt.width), &feature);
        //std::cout<<"?"<<std::endl;
        int fact = response(feature, opt.numOfFeatures, (size_t)(scale*(double)opt.height - 1), (size_t)(scale*(double)opt.width ) - 1);
        return fact < opt.response ? 1 : 0;
    }

    //îòêŵûòèå ôàéëà è ñîõŵàíåíèå ïàëèòŵû
    void OpenFileAndSavePalette(FILENAME_PALETTE_AND_SIZE *image){
        if (extension == ".png"){
            PNG png(fileName);
            png.load();
            image->palette = png.getPalette();
            image->height = png.getHeight();
            image->width = png.getWidth();
            rgbPalette = png.getRGBpalette();
        }
        if (extension == ".jpg"){
            JPG jpg(fileName);
            jpg.load();
            image->palette = jpg.getPalette();
            image->height = jpg.getHeight();
            image->width = jpg.getWidth();
            rgbPalette = jpg.getRGBpalette();
        }
    }

    //âûäåëåíèå îáëàñòè
    void rect(std::vector<std::vector<int>> &matrix, size_t x, size_t y, size_t h, size_t w, std::vector<std::vector<int>> *rectMatr){
        for (size_t l = 0; l < h; l++){
            std::vector<int> temp;
            for (size_t k = 0; k < w; k++){
                temp.push_back(matrix[l + x][k + y]);
            }
            rectMatr->push_back(temp);
        }
    }

    int response(std::vector<std::vector<int>> &rect, size_t i, size_t h, size_t w){
        int SUM_ALL;
        int SUM_BLACK;
        int SUM_WHITE;
        switch (i){
            /*ṫẁŵíàÿ ïîëîñà ñïŵàâà( ṫåŵíûé = ñóììà âñåõ - ñóììà áåëûõ)*/
            case 0:{
                SUM_WHITE = rect[h][w / 2] + rect[0][0] - rect[0][w / 2] - rect[h][0];
                SUM_BLACK = rect[h][w] + rect[0][w / 2] - rect[h][w / 2] - rect[0][w];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ âåŵòèêàëüíàÿ ïîëîñà ïî öåíòŵó(ṫåŵíûé = (áåëûé+ṫåŵíûé) - áåëûé)*/
            case 1:{
                SUM_WHITE = (rect[h][w / 3] + rect[0][0] - rect[0][w / 3] - rect[h][0]) + (rect[h][w] + rect[0][w - (w / 3)] - rect[0][w] - rect[h][w - (w / 3)]);
                SUM_BLACK = rect[h][w - (w / 3)] + rect[0][w / 3] - rect[0][w - (w / 3)] - rect[h][w / 3];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ ãîŵèçîíòàëüíàÿ ïîëîñà ïî öåíòŵó(ṫåŵíûé = (áåëûé+ṫåŵíûé) - áåëûé)*/
            case 2:{
                SUM_WHITE = (rect[h / 3][w] + rect[0][0] - rect[0][w] - rect[h / 3][0]) + (rect[h][w] + rect[h - (h / 3)][0] - rect[h - (h / 3)][w] - rect[h][0]);
                SUM_BLACK = rect[h - (h / 3)][w] + rect[h / 3][0] - rect[h / 3][w] - rect[h - (h / 3)][0];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                /*Ṫåŵíàÿ ïîëîñà ñíèçó(ṫåŵíûé = ñóììà âñåõ - ñàììà áåëûõ)*/
            case 3:{
                SUM_WHITE = rect[h / 2][w] + rect[0][0] - rect[0][w] - rect[h / 2][0];
                SUM_BLACK = rect[h][w] + rect[h / 2][0] - rect[h / 2][w] - rect[h][0];
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                //êâàäŵàòèê ïî öåíòŵó
            case 4:{
                SUM_BLACK = rect[h - (h / 3)][w - (w / 3)] + rect[h / 3][w / 3] - rect[h / 3][w - (w / 3)] - rect[h - (h / 3)][w / 3];
                SUM_WHITE = rect[h][w] + rect[0][0] - rect[h][0] - rect[0][w] - SUM_BLACK;
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
                //øàøåṫêè
            case 5:{
                SUM_WHITE = (rect[h / 2][w / 2] + rect[0][0] - rect[0][w / 2] - rect[h / 2][0]) + (rect[h][w] + rect[h / 2][w / 2] - rect[h / 2][w] - rect[h][w / 2]);
                SUM_BLACK = rect[h][w] + rect[0][0] - rect[h][0] - rect[0][w] - SUM_WHITE;
                SUM_ALL = SUM_WHITE - SUM_BLACK;
                return SUM_ALL;
            }
            default:{
                std::cout << "Error: incorrect response" << std::endl;
                return NULL;
            }
        }
    }
};

#endif //VJDETECTOR_SEARCH_H
