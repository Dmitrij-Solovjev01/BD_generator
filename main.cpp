#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <map>

using namespace std;

string current_domainname;

string Read_CSV(int RowNumber, fstream* file, int index, int ulala=0) {
    string row, line, word;

    if (file->is_open()) {
        uint32_t count = 0;
        file->clear();
        file->seekg(0);

        while (count+1 < RowNumber and getline(*file, line)) {
            count++;
        }

        getline(*file, line);
        if (ulala == 1) {
            //cout << line << endl;
            size_t pos = line.find(",http");
            while (pos == std::string::npos) {
                getline(*file, line);
                pos = line.find(",http");
            }
        } else if (ulala == 2) {
            size_t pos = line.find(",,,");

            while (pos != std::string::npos) {
                getline(*file, line);
                pos = line.find(",,,");
            }
        }


        stringstream str(line);
        int count2=0;

        while (getline(str, word, ',')) {
            if (index == count2 or index == -1 and !word.empty()) {
                row += word;
            }
            if (ulala == 3 and index == count2-1){
                current_domainname = "@" + word;
            }

            count2++;
        }
    } else cout<<"Could not open the file\n";
    return row;
}


pair <string, int> Get_Rand_Time(const time_t *Start, const time_t *Stop){
    int64_t module = difftime(*Stop,*Start);

    int64_t rand_diff_time = rand()%module;                                         // случайным образом получаем значение, принадлежащее промежутку
    int64_t rand_time = *Start+rand_diff_time;

    tm *ltm = localtime(&rand_time);

    return make_pair(to_string(ltm->tm_mday) + "/" +
                        to_string(ltm->tm_mon ) + "/" +
                        to_string(1900 + ltm->tm_year), ltm->tm_mon);       // отдельно возвращаем месяц для соответствия товара сезону
}


int main() {
    fstream *arr[12];     ofstream myfile;

    fstream URL_file("../URL2.csv", ios::in);                              //открываем потоки
    fstream LOGIN_file("../LOGIN.csv", ios::in);
    myfile.open("../BD.csv");

    map<string, string> check;                                                      //словарь для проверки, чтобы одному человеку не попаласть та же реклама

    for (int i = 0; i < 12; ++i)
        arr[i] = new fstream("../Sale/Sales" + to_string(i) + ".csv", ios::in);

    struct tm timeStart{}, timeStop{};                                              //временные интервалы

    timeStart.tm_mday = 25;                                                         //была взята дата моего рождения
    timeStart.tm_mon = 9;
    timeStart.tm_year = 2003 - 1900;
    timeStart.tm_hour = 0;
    timeStart.tm_min = 0;
    timeStart.tm_sec = 0;

    timeStop.tm_mday = 25;                                                          //пред. дата + 1 месяц
    timeStop.tm_mon = 10;
    timeStop.tm_year = 2003 - 1900;
    timeStop.tm_hour = 0;
    timeStop.tm_min = 0;
    timeStop.tm_sec = 0;

    time_t pointTimeStart = mktime(&timeStart);
    time_t pointTimeStop = mktime(&timeStop);


    for (int i = 0; i < 50000; i++) {                                               //основной цикл
        pair <string, int> date = Get_Rand_Time(&pointTimeStart, &pointTimeStop);
        string login, tovar, url = Read_CSV(rand()%60, &URL_file, 1, 3);

        do {
            login = Read_CSV(rand()%600, &LOGIN_file, 1, 1);
            tovar = Read_CSV(rand()%100, &*arr[date.second], 1, 2);
        } while (check[login] == tovar and !check[login].empty()); //check[login] != ""

        check[login] = tovar;

        // Строчки 119 - 122 задают ip адресс
        int ip1 = rand()%256;
        if (ip1 == 10 or ip1 == 172 or ip1 == 192) ip1++;                           // ограничения на глобальный ip обходятся простым смещением на +1
        int countrecl = rand()%100;

        //Строчки 124-125 задают время и количество рекламы
        int k = rand()%90+30;
        int tttime = k*countrecl;

        myfile << login + current_domainname << ','                                 // имя + имя домена
        << ip1 << '.' << rand()%256<< '.' << rand()%256<< '.' << rand()%256 << ','  // ip адресс
        << url << ","                                                               // сайт
        << date.first << "," <<                                                     // дата
        countrecl << ','                                                            // количество просмотренной рекламы
        << (tttime/60)/10<< (tttime/60)%10 << ":" << (tttime%60)/10 << tttime%10
        <<',' << tovar << ",\n";
    }


    for (auto & i : arr)                                                  // закрываем потоки
        i->close();

    URL_file.close();
    LOGIN_file.close();
    myfile.close();

    return 0;
}