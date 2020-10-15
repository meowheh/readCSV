#ifndef __READCSV_H__
#define __READCSV_H__

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

class ReadCSV{
    std::ifstream file;                  //переменная для взаимодействия с файлом
    void initColumns(std::string);       //функция инициализации столбцов таблицы
    void readStr(std::string, int&);     //функция, обрабатывающая очередную строку
    bool isRow(std::string);             //функция, определяющая принадлежность строки к номеру строки
    int parseOperation(std::string);     //функция, вычисляющая значения выражений = Column1Row1 <operator> Column2Row2
    void fileClose();                    //функция, закрывающая файл
public:
    std::map<int, std::map<std::string, std::string>> data;          //считываемые данные в двухмерном ассоциативном хранилище: data[номер_строки][название_столбца] = ячейка
    std::vector<std::string> columns;                                //массив названий столбцов
    std::vector<int> rows;                                           //массив номеров строк
    ReadCSV(char*);                                                  //конструктор, принимает имя файла
    void readData();                                                 //основная функция, которая считывает и обрабатывает данные из файла
    friend std::ostream& operator<<(std::ostream&, const ReadCSV&);  //перегрузка оператора вывода в поток ostream объекта ReadCVS (выводит содержимое в консоль)
    ~ReadCSV();                                                      //деструктор класса
};

#endif // __READCSV_H__