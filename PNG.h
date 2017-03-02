//
// Created by vadim on 02.03.17.
//

#ifndef VJDETECTOR_PNG_H
#define VJDETECTOR_PNG_H

#include "Image.h"
class PNG : public Picture
{
private:
    IplImage* image = 0;
    CvSize size;
public:
    //template <class Type>
    PNG(const char *fileName) : Picture(fileName) {}

    void load(){
        image = cvLoadImage(fileName, 1);

        w = image->width;
        h = image->height;

        palette = new MY_RGB[w*h];

        for (int y = 0, k = 0; y < image->height; y++) {
            uchar* ptr = (uchar*)(image->imageData + y * image->widthStep);
            for (int x = 0; x < image->width; x++, k++) {
                palette[k].rgbtBlue = ptr[3 * x];
                palette[k].rgbtGreen = ptr[3 * x + 1];
                palette[k].rgbtRed = ptr[3 * x + 2];
            }
        }
        toY(palette, h, w);
        std::vector<std::vector<int>> tmpMatrix = IntegralImage(matrix);
        Imatrix = tmpMatrix;
        cvReleaseImage(&image);
    }
};
#endif //VJDETECTOR_PNG_H
