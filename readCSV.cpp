#include "readCSV.h"
#include <algorithm>
//конструктор, fileName - название файла
ReadCSV::ReadCSV(char* fileName){
    //открываем файл по его названию
    file.open(fileName);
} 
//row - предположительный столбец в виде строки string
bool ReadCSV::isRow(std::string row){
    bool isRow = false;       //вспомогательный флаг принадлежности элемента к столбцу
    int rowNum;               //целочисленное значение row
    try{
        rowNum = std::stoi(row); //если row может быть преобразована в int
        //перебегаем по элементам массива считанных столбцов rows, 
        //пока не достигнем конца или не найдем совпадение с рассматриваем значением
        for(int i = 0; i < rows.size() && !isRow ; ++i){
            if(rowNum == rows[i])
                 isRow = true;
        }
        //строку нельзя преобразовать в число - это точно не номер строки
    }catch(const std::invalid_argument& ){
        return false; 
    }
    return isRow;
}
/*line - рассматриваемая строка вида "= arg1Str <operator> arg2Str"
                                   arg1Str:= column1row1
                                   arg2Str:= column2row2
                                   <operator>:= + | - | * | /  
*/
int ReadCSV::parseOperation(std::string line){
    std::string str = line,  //вспомогательная переменная, сохраняющая исходную строку
                arg1Str,     //левый операнд целиком в виде строки
                arg1Value,   //значение ячейки левого операнда
                arg2Str,     //правый операнд целиком в виде строки
                arg2Value,   //значение ячейки правого операнда
                column1,     //столбец левого операнда
                column2,     //столбец правого операнда
                row1,        //строка левого операнда
                row2;        //строка правого операнда
    std::vector<char> operators = {'+','-','*','/'};   //массив операторов
    char op;                 //оператор текущего выражения
    int res = 0;             //результат выражения
    //если строка действительно выражение
    if(str[0] == '='){    
        size_t posOfOperator = 0,   //номер положения оператора в строке
               posArg1 = 0,         //номер первого вхождения левого операнда
               posArg2 = 0;         //номер первого вхождения правого операнда
        bool isFound = false;       //флаг нахождения значения оператора
        //цикл по массиву операторов, пока не будет найдена его позиция 
        for(int i = 0; i < operators.size() && !isFound; ++i){
            if((posOfOperator = str.find(operators[i]))!= std::string::npos){
                isFound = true;
                op = operators[i];
            }
        }
        //разделяем строку на две части: до оператора - левый операнд
        arg1Str = str.substr(1,posOfOperator-1);
        //после оператора - правый операнд
        arg2Str = str.substr(posOfOperator+1,str.length());
        bool isFound1 = false;    //флаг нахождения левого операнда
        bool isFound2 = false;    //флаг нахождения правого операнда
        //цикл по всем столбцам, пока не найдем операнды
        for(int i = 0; i < columns.size() && (!isFound1 || !isFound2); ++i){
                //если текущий столбец входит в левый операнд
                if((posArg1 = arg1Str.find(columns[i]))!= std::string::npos &&!isFound1){
                    column1 = row1 = arg1Str;
                    //получаем столбец левого операнда
                    column1 = column1.substr(posArg1,columns[i].length());
                    //и строку
                    row1 = row1.substr(column1.length(),row1.length());
                    /*так как возможны ситуации, когда название столбца является составной частью какого-то другого столбца
                    например один столбец назван C, а другой - Cs, алгоритм может неправильно определить столбец
                    и поэтому можно проверить полученный номер строки - если столбец и строка неправильно разделены - такой номер строки
                    не будет найден или он вовсе не будет числом*/
                    if(isRow(row1))  //если все нормально - левый операнд найден
                        isFound1 = true;
                }
                //аналогичная ситуация с правым операндом
                if((posArg2 = arg2Str.find(columns[i]))!= std::string::npos && !isFound2){
                    column2 = row2 = arg2Str;
                    column2 = column2.substr(posArg2,columns[i].length());
                    row2 = row2.substr(columns[i].length(),row2.length());
                    //если строка и столбец правильно определены - правый операнд найден
                    if(isRow(row2))
                        isFound2 = true;
                }  
        }
        //если все значения найдены верно
        if(isRow(row1) && isRow(row2) && !column1.empty() && !column2.empty()){
            //получаем значения ячеек левого и правого операнда в виде строки
            arg1Value = data[std::stoi(row1)][column1];
            arg2Value = data[std::stoi(row2)][column2];
            int arg1, arg2;         //значения ячеек левого и правого операнда в виде целого числа
            if(arg1Value[0] == '='){ //если значение левого операнда оказалось выражением
                //рекурсивно выполняем функцию с этой строкой
                arg1 = parseOperation(arg1Value);
                //меняем значение ячейки из выражения на полученное значение
                data[std::stoi(row1)][column1] = std::to_string(arg1);
            }
            else { //полученное значение - не выражение
                    //преобразуем его в число
                    arg1 = std::stoi(arg1Value);
            }//аналогично, если правый операнд - выражение
            if(arg2Value[0] == '='){
                //вычисляем его значение
                arg2 = parseOperation(arg2Value);
                //и записываем в ячейку вычисленное значение
                data[std::stoi(row2)][column2] = std::to_string(arg2);
            }
            else { //или сразу преобразуем в число
                arg2 = std::stoi(arg2Value);
            }
            switch(op){ //в зависимост от оператора вычисляем операцию с двумя операндами
                case '+': 
                    res = arg1 + arg2;
                    break;
                case '-':
                    res = arg1 - arg2;
                    break;
                case '*':
                    res = arg1 * arg2;
                    break;
                case '/':
                    res = arg1 / arg2;
                    break;
            }
        }
    }
    return res;
}
//line - прочитанная строка из файла
void ReadCSV::initColumns(std::string line){
    size_t pos = 0;                //позиция разграничителя строки
    bool flag = true;              //флаг окончания строки
    std::string token;             //подстрока
    //пока строка полностью не разделена на подстроки
    while(flag && !line.empty()){
        //если найдена позиция очередной запятой 
        if((pos = line.find(','))!= std::string::npos){
            //получаем построку до этой запятой
            token = line.substr(0, pos);
            //и стираем часть строки
            line.erase(0, pos + 1);
        }
        else {
            //иначе получаем последнюю подстроку после запятой
            token = line;
            flag = false; 
        }
        //добавляем название очередного столбца в массив столбцов
        columns.push_back(token);
    }
}
//line - прочитанная строка, k - ссылка на вспомогательный счетчик,
//который будет учитываться при отсутствии явного столбца с нумирацией строк
void ReadCSV::readStr(std::string line, int& k){
    bool flag = true;   //флаг окончания обработки строки
    int key,            //значение первого столбца строки - номера строки
        count = 0,      //вспомогательный счетчик для определения номера рассматриваемого элемента в строке
        pos = 0;        //позиция очередной запятой
    std::string token;  //подстрока
    //цикл, пока строка не обработана
    while(flag && !line.empty()){
        //если найдена очередная позиция запятой в строке
        if((pos = line.find(','))!= std::string::npos){
            //находим подстроку до запятой
            token = line.substr(0, pos);
            //и удаляем ее из рассмотрения
            line.erase(0, pos + 1);
        }
        else { //иначе дошли до конца и записываем последнюю подстроку
            token = line;
            flag = false;
        }
        //первым элементом может находится номер строки
        if(count == 0){
            if(columns[0].length() == 0){
                try{ //пытаемся преобразовать в число
                    key = std::stoi(token);
                }catch(const std::invalid_argument& ){
                //если преобразовать нельзя - используем вспомогательный счетчик k
                    k++;
                    key = k;
                }
            } //если номера строк явно не заданы используем вспомогательный счетчик k
            else{
                k++;
                key = k;
            }
            //добавляем номер столбца в массив столбцов
            rows.push_back(key);
           
        }
        //добавляем значение ячейки
        data[key][columns[count]] = token;
        count++;
    }
}
//функция, начинающая чтение файла
void ReadCSV::readData(){
    int counter = 0;     //вспомогательный счетчик номеров строк, если в файле его явно не будет
    //пока файл открыт и нет ошибок
    while(file.good()){
        std::string line;          //считываемая строка
        //считываем очередную строку с файла
        std::getline(file, line);  
        //удаляем все возможные пробелы и табы со строки
        line.erase(std::remove_if(line.begin(), 
            line.end(), [](unsigned char x){return std::isspace(x);}),line.end());
        //в первой строке находятся названия столбцов - инициализируем их
        if(columns.empty()){
            initColumns(line);
        }   
        else{ //иначе просто анализируем очередную строку
            readStr(line, counter);
        }
    }
    //окончили чтение, закрываем файл
    fileClose();
    //теперь по всем элементам вычисляем выражения, если они будут найдены
    for(auto row : rows){
        for(auto column: columns){
            //если ячейка содержит выражение
            if (data[row][column].find('=') != std::string::npos){
                //находим его значение
                int var = parseOperation(data[row][column]);
                //и записываем его обратно в ячейку
                data[row][column] = std::to_string(var);
            }
        }
    }
}
//перегрузжаем оператор << для записи содержимого объекта класса ReadCVS obj в поток out
 std::ostream& operator<<(std::ostream& out, const ReadCSV &obj){
    auto dt = obj.data;
    int i = 0;     
    //записываем в поток все названия столбцов через запятую
    for(auto column : obj.columns){
        out << column;
        if(i < obj.columns.size()-1){
            out << ", ";
        }
        i++;
    }
    out << std::endl;
    //записываем в поток содержимое ячеек в каждой строке через запятую
    for(int row: obj.rows){
        int i = 0;
        for(auto column: obj.columns){
            out << dt[row][column];
            if(i < obj.columns.size()-1){
             out << ", ";
            }
            i++;
        }
       out << std::endl;
    }
    return out;
 }

void ReadCSV::fileClose(){
    //если файл еще не был закрыт - закрываем
    if (file.is_open())
        file.close();
}
ReadCSV::~ReadCSV(){
    //закрываем файл в конце выполнения программы, если до этого еще вдруг не закрыли
    fileClose();
}