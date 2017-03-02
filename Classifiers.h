//
// Created by vadim on 02.03.17.
//

#ifndef VJDETECTOR_CLASSIFIERS_H
#define VJDETECTOR_CLASSIFIERS_H
#include "Base.h"
#include <iostream>
#include <sstream>
struct RESPONSE_COORDINATES_SIZE_AND_TYPE{
    int response;
    int coordinate_x;
    int coordinate_y;
    int height;
    int width;
    int numOfFeatures;
    double Error;
};

struct weak_classifiers{
    RESPONSE_COORDINATES_SIZE_AND_TYPE optimalResponse;
    double coefficientA;
};

class Classifiers{
private:
    const char* direct;
    FILE* fin;
    std::vector<weak_classifiers> wclasses;
public:
    Classifiers(const char* direct) : direct(direct){}

    void init(){
        Base* base = new Base();
        base->iterator(direct);
        size_t sizeDir = base->getSizeDirectory();
        for (size_t countDir = 0; countDir < sizeDir;countDir++){
            weak_classifiers tmp;
            std::ostringstream name;
            PAIR_OF_FILENAME_AND_EXTENSION file;
            name << direct << "/" << countDir << ".vjc";
            file.fileName = name.str();
            file.extension = ".vjc";
            base->readFile(file.fileName, tmp);
            wclasses.push_back(tmp);
        }
        delete base;
    }

    size_t size(){
        return wclasses.size();
    }

    weak_classifiers get(size_t num){
        return wclasses.at(num);
    }
};
#endif //VJDETECTOR_CLASSIFIERS_H
