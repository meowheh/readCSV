#ifndef __READCSV_H__
#define __READCSV_H__

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class ReadCSV{
    std::ifstream& file;                 //file stream reference
    void initColumns(std::string);       //reading columns names
    void readStr(std::string, int&);     //process the next line
    bool isRow(std::string);             //is the string the name of a table row
    int parseOperation(std::string);     //value of expression  = Column1Row1 <operator> Column2Row2
    void fileClose();                    //closing file
public:
    std::map<int, std::map<std::string, std::string>> data;          //file data: data[row][column] = value
    std::vector<std::string> columns;                                //array of columns names
    std::vector<int> rows;                                           //array of rows
    ReadCSV(std::ifstream&);                                         //class constructor, take argument of file stream reference
    void readData();                                                 //begin read and process data
    friend std::ostream& operator<<(std::ostream&, const ReadCSV&);  //operator overloading << for output data of object
    ~ReadCSV();                                                      //class destructor
};

#endif // __READCSV_H__