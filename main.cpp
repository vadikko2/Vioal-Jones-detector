#include "Search.h"

int main(int argc, char** argv){
    if(argc != 6){
        printf("usage: ./VJDetector <FileName> <FileExt> <PathWeakClassifiers> <MinHSizeSlidingWindow> <MinWSizeSlidingWindow>\n");
        return -1;
    }
    const char* pict = argv[1];
    std::string ext = argv[2];
    const char* classifiers = argv[3];
    size_t h = atoi(argv[4]);
    size_t w = atoi(argv[5]);

    Search serch;
    serch.setFile(pict, ext);
    serch.searchFaceOnImage(classifiers, h, w);
    serch.saveResults();
    return 1;
}