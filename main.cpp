#include "readCSV.h"

int main(int argc, char *argv[]){
    if(argc > 1){
        //argv[1] - имя файла
        ReadCSV file(argv[1]);
        file.readData();
        std::cout << file;
    }
    return 0;
}