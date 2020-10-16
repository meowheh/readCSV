#include "readCSV.h"

int main(int argc, char *argv[]){
    std::string nameFile;
    //if user did not enter a file name in the argument
    if(argc == 1){
        std::cout << "Enter a file name: ";
        std::cin >> nameFile;
    }
    else if(argc > 1){
         //argv[1] - file name
        nameFile = argv[1];
    }
    //check if name of file is not empty
    if(!nameFile.empty()){
        std::ifstream file;
        //open file
        file.open(nameFile);
        //check if file is open
        if(file.is_open()){
            //create object and read data from file
            ReadCSV csv(file);
            csv.readData();
            std::cout << csv;
        }
       else std::cout << "The file was not opened" <<std::endl;
    }
    else std::cout << "You didn't enter a file name" <<std::endl;
    return 0;
}
