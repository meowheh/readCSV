# readCSV
Чтение файла расширения csv
***
Предполагается, что у файла есть названия колонок.<br>
Помимо обычных строковых значений, ячейки могут иметь вид ***= ARG1 op ARG2***,<br> где op := + | - | \* | / 
<br>
Было успешно скомпилировано на Linux с помощью clang: **clang++ main.cpp readCSV.cpp -o <name_file>**
<br>И на Windows с помощью компилятора mingw: **g++ main.cpp readCSV.cpp -o <name_file>**
<br>
Запуск: **./<name_file> file.csv**
