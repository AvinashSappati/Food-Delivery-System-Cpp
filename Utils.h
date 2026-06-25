#pragma once
#include <string>

std::string getCurrentDateTime();
void printLine(char c = '=', int length = 60);
void clearScreen();
void pauseScreen();
int generateOrderID();
void showHeader(const std::string& title);
bool validatePhone(const std::string& phone);