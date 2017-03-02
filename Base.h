//
// Created by vadim on 02.03.17.
//

#ifndef VJDETECTOR_BASE_H
#define VJDETECTOR_BASE_H

#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>



//èìÿ ôàéëà è ŵàñøèŵåíèå
struct PAIR_OF_FILENAME_AND_EXTENSION{
    std::string fileName;//èìÿ ôàéëà
    std::string extension;//ŵàñøèŵåíèå
};
class Base {
private:
    boost::filesystem::directory_iterator it, end;
    boost::filesystem::path directory;
    size_t count;
    size_t NumOfFiles;
public:
    //óñòàíîâêà èòåŵàòîŵà
    void iterator(const char* path){
        count = 0;
        boost::filesystem::path directory(path);
        boost::filesystem::directory_iterator it(directory), end;//óñòàíîâêà èòåŵàòîŵà íà íàṫàëî è êîíåö äèŵåêòîŵèè
        this->it = it;
        this->end = end;
    }
    size_t getSizeDirectory(){
        while (this->it != this->end){
            count++;
            ++this->it;
        }
        return count;
    }
    //ïŵîâåŵêà íà êîíåö äèŵåêòîŵèè
    bool hasNext(){
        return this->it != this->end ? true : false;
    }

    //âîçâŵàùàåò ïàŵó (Èìÿ ôàéëà / ŵàñøèŵåíèå ôàéëà)
    void next(PAIR_OF_FILENAME_AND_EXTENSION &file){
        file.fileName = it->path().string();
        count++;
        file.extension = it->path().extension().string();
        it++;
    }

    //ñîçäàẁò âŵåìåííóŷ äèŵåêòîŵèŷ
    void createTmpDirectory(const char* path){
        NumOfFiles = 0;
        boost::filesystem::path directory(path);
        this->directory = directory;

        if (boost::filesystem::exists(this->directory)
            && boost::filesystem::is_directory(this->directory))
            boost::filesystem::remove_all(this->directory);

        boost::filesystem::create_directory(this->directory);
    }

    //îṫèùàåò âŵåìåííóŷ äèŵåêòîŵèŷ
    void clearTmpDirectory(){
        boost::filesystem::remove_all(this->directory);
    }

    //ñäàẁò è çàïîëíÿåò âŵåìåííûé ôàéë
    template<class toFile>
    void createAndFillTmpFile(std::string &fileName, std::vector<toFile> &data){
        std::string line;
        line += directory.string() + '/' + fileName;
        const char * fname = (line).c_str();
        FILE* tmpfile = fopen(fname, "wb");
        for (int i = 0; i < data.size(); i++){
            fwrite(&data.at(i), sizeof(toFile), 1, tmpfile);
        }
        fclose(tmpfile);
    }

    template<class Struct>
    void createAndFillTmpFile(std::string &fileName, Struct &data){
        std::string line;
        line += directory.string() + '/' + fileName;
        const char * fname = (line).c_str();
        FILE* tmpfile = fopen(fname, "wb");
        fwrite(&data, sizeof(Struct), 1, tmpfile);
        fclose(tmpfile);
    }

    //îòêŵûâàåò äèŵåêòîŵèŷ
    void openDirectory(const char* path){
        boost::filesystem::path direct(path);
        this->directory = direct;
    }

    //ṫèòàåò äàííûå èç ôàéëà
    template <class fromFile>
    void readFile(std::string fileName, std::vector<fromFile> &data, int sizeAcces){
        const char* fname = fileName.c_str();
        FILE* fin = fopen(fname, "rb");
        for (int i = 0; i < sizeAcces; i++){
            fromFile tmp;
            fread(&tmp, sizeof(fromFile), 1, fin);
            data.push_back(tmp);
        }
        fclose(fin);
    }

    template <class Struct>
    void readFile(std::string fileName, Struct &data){
        const char* fname = fileName.c_str();
        FILE* fin = fopen(fname, "rb");
        std::cout<<"size = "<< sizeof(Struct)<<"!"<<std::endl;
        fread(&data, sizeof(Struct), 1, fin);
        fclose(fin);
    }
};
#endif //VJDETECTOR_BASE_H
