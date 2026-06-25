#include "Utils.h"
#include "Globals.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <cstdlib>

using namespace std;

string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    stringstream ss;
    ss << setfill('0') 
       << setw(2) << ltm->tm_mday << "/"
       << setw(2) << 1 + ltm->tm_mon << "/"
       << 1900 + ltm->tm_year << " "
       << setw(2) << ltm->tm_hour << ":"
       << setw(2) << ltm->tm_min;
    
    return ss.str();
}

void printLine(char c, int length) {
    for(int i = 0; i < length; i++) {
        cout << c;
    }
    cout << endl;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

int generateOrderID() {
    ifstream file(ORDER_FILE);
    int count = 0;
    string line;
    while (getline(file, line)) count++;
    file.close();
    return 1000 + count;
}

void showHeader(const string& title) {
    clearScreen();
    printLine('=', 60);
    cout << "                   " << title << endl;
    printLine('=', 60);
}

bool validatePhone(const string& phone) {
    if (phone.length() != 11) return false;
    for (char c : phone) {
        if (!isdigit(c)) return false;
    }
    return true;
}