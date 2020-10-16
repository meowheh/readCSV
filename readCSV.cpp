#include "readCSV.h"
#include <algorithm>
//file - file stream reference
ReadCSV::ReadCSV(std::ifstream& file): file(file) {};

bool ReadCSV::isRow(std::string row){
    bool isRow = false;       //flag for whether an element belongs to a column
    int rowNum;               //integer value of the row
    try{
        rowNum = std::stoi(row); //if row can be converted to int
        //running through the elements of the array of rows, 
        //until we reach the end or find a match with the value in question
        for(int i = 0; i < rows.size() && !isRow ; ++i){
            if(rowNum == rows[i])
                 isRow = true;
        }
        //if string can't be converted to a number, it's definitely not a row
    }catch(const std::invalid_argument& ){
        return false; 
    }
    return isRow;
}
/*line is the string in question looks like  "= arg1Str <operator> arg2Str"
                                             arg1Str:= column1row1
                                             arg2Str:= column2row2
                                             <operator>:= + | - | * | /  
*/
int ReadCSV::parseOperation(std::string line){
    std::string str = line,  //saving source string
                arg1Str,     //the left operand is entirely in string
                arg1Value,   //value of the cell of the left operand
                arg2Str,     //the rigth operand is entirely in string
                arg2Value,   //value of the cell of the rigth operand
                column1,     //column of the left operand
                column2,     //column of the rigth operand
                row1,        //row oof the left operand
                row2;        //column of the rigth operand
    std::vector<char> operators = {'+','-','*','/'};   //array of operators
    char op;                 //current operator
    int res = 0;             //result of expression
    //if the string is really an expression
    if(str[0] == '='){    
        size_t posOfOperator = 0,   //number of position of operator 
               posArg1 = 0,         //number of the first occurrence of left operand
               posArg2 = 0;         //number of the first occurrence of rigth operand
        bool isFound = false;       // true if the operator value is found
        //loop through an array of operators until its position is found
        for(int i = 0; i < operators.size() && !isFound; ++i){
            if((posOfOperator = str.find(operators[i]))!= std::string::npos){
                isFound = true;
                op = operators[i];
            }
        }
        //splitting the string into two parts: before the operator - the left operand
        arg1Str = str.substr(1,posOfOperator-1);
        //after the operator - the rigth operand
        arg2Str = str.substr(posOfOperator+1,str.length());
        bool isFound1 = false;    //true if the value of the rigth operand is found
        bool isFound2 = false;    //true if the value of the left operand if found
        //loop through all columns until we find the operands
        for(int i = 0; i < columns.size() && (!isFound1 || !isFound2); ++i){
                //if the left operand contains current column 
                if((posArg1 = arg1Str.find(columns[i]))!= std::string::npos &&!isFound1){
                    column1 = row1 = arg1Str;
                    //getting the column of the left operand
                    column1 = column1.substr(posArg1,columns[i].length());
                    //и строку
                    row1 = row1.substr(column1.length(),row1.length());
                    /*there may be situations whenn the column name is part of some other column, for example, one column is named C and another Cs,
                    the algorithm may incorrectly indentify a column
                    so I check the row number if the column and row that are incorrectly divided
                    this line number will not be found or it will not be a number at all*/
                    if(isRow(row1)) 
                        isFound1 = true;
                }
                //the situation is similar with the rigth operand
                if((posArg2 = arg2Str.find(columns[i]))!= std::string::npos && !isFound2){
                    column2 = row2 = arg2Str;
                    column2 = column2.substr(posArg2,columns[i].length());
                    row2 = row2.substr(columns[i].length(),row2.length());
                    //if the row and column are correctly defined
                    if(isRow(row2))
                    //then the rigth operand is found
                        isFound2 = true;
                }  
        }
        //if all values are found correctly
        if(isRow(row1) && isRow(row2) && !column1.empty() && !column2.empty()){
            //get the cell values of the left and rigth operands as a string
            arg1Value = data[std::stoi(row1)][column1];
            arg2Value = data[std::stoi(row2)][column2];
            int arg1, arg2;         //values of the cells of the left and rigth operands as an integer
            if(arg1Value[0] == '='){ //if the value of the left operand is an expression
                //execute the function with this string
                arg1 = parseOperation(arg1Value);
                //changing the cell value from the expression to the received value
                data[std::stoi(row1)][column1] = std::to_string(arg1);
            }
            else { //the resulting value is not an expression,
                    //then converting it to int
                    arg1 = std::stoi(arg1Value);
            }//similarly, if the rigth operand is an expression
            if(arg2Value[0] == '='){
                //execute the function with this string
                arg2 = parseOperation(arg2Value);
                //changing the cell value from the expression to the received value
                data[std::stoi(row2)][column2] = std::to_string(arg2);
            }
            else { //or converting it to int
                arg2 = std::stoi(arg2Value);
            }
            switch(op){ //depending on the operator, we calculate an operation with two operands
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
//line is read line from file
void ReadCSV::initColumns(std::string line){
    size_t pos = 0;                //position of separator
    bool flag = true;              //true if line ended
    std::string token;             //substring
    //until the string is completely split into substrings
    while(flag && !line.empty()){
        //if position of separator found
        if((pos = line.find(','))!= std::string::npos){
            //getting the substring before the separator
            token = line.substr(0, pos);
            //and erase part of the line
            line.erase(0, pos + 1);
        }
        else {
            //else we get the last substring after the separator
            token = line;
            flag = false; 
        }
        //adding the name of the next column to the array of columns
        columns.push_back(token);
    }
}
//line is line read, k - a reference to the secondary counter, 
//which will be counted if there is no explicit column with row numbering
void ReadCSV::readStr(std::string line, int& k){
    bool flag = true;   //true if line ended
    int key,            //value of the first element of string - number row
        count = 0,      //secondary counter for determining the number of the element in question in a row
        pos = 0;        //position of separator
    std::string token;  //substring
    //until line ended
    while(flag && !line.empty()){
        //if separator position is found in the string
        if((pos = line.find(','))!= std::string::npos){
            //find the substring before the separator
            token = line.substr(0, pos);
            //and remove it from consideration
            line.erase(0, pos + 1);
        }
        else { //have reached the end and write the last substring
            token = line;
            flag = false;
        }
        //the first element can be the row number
        if(count == 0){
            if(columns[0].length() == 0){
                try{ //trying to convert to a number
                    key = std::stoi(token);
                }catch(const std::invalid_argument& ){
                //if I can't convert it, use the counter k
                    k++;
                    key = k;
                }
            } //if the row numbers are not explicitly specified I use the counter k
            else{
                k++;
                key = k;
            }
            //adding the column number to the array of columns
            rows.push_back(key);
           
        }
        //adding the cell value
        data[key][columns[count]] = token;
        count++;
    }
}
void ReadCSV::readData(){
    int counter = 0;     //secondary row number counter if the file doesn't explicity contain one
    //until the file is open and there are no errors
    while(file.good()){
        std::string line;          //read string
        //reading sting from the file
        std::getline(file, line);  
        //removing all possible spaces and tabs from the string
        line.erase(std::remove_if(line.begin(), 
            line.end(), [](unsigned char x){return std::isspace(x);}),line.end());
        //the first row contains the column names - initialize then
        if(columns.empty()){
            initColumns(line);
        }   
        else{ //analyze the next string
            readStr(line, counter);
        }
    }
    //finished reading, close the file
    fileClose();
    //calculate expressions for all elements, if they are found
    for(auto row : rows){
        for(auto column: columns){
            //is the cell contains the expression
            if (data[row][column].find('=') != std::string::npos){
                //finding its value
                int var = parseOperation(data[row][column]);
                //and write it back to the cell
                data[row][column] = std::to_string(var);
            }
        }
    }
}
//overloading the << operator to write the contents of an object of the ReadCSV obj class to the out stream
 std::ostream& operator<<(std::ostream& out, const ReadCSV &obj){
    auto dt = obj.data;
    int i = 0;     
    //write all column names to the stream separated by commas
    for(auto column : obj.columns){
        out << column;
        if(i < obj.columns.size()-1){
            out << ", ";
        }
        i++;
    }
    out << std::endl;
    //write the contents of cells in each row separated by commas to the stream
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
    //if the file has not been closed yet, close it
    if (file.is_open())
        file.close();
}
ReadCSV::~ReadCSV(){
    //close the file at the end of the program, if it has not been closed yet
    fileClose();
}