#include <iostream>
#include <fstream>
#include "Globals.h"
#include "Services.h"
#include "App.h"
#include "Utils.h"

using namespace std;

int main() {
    // Check if configuration files exist, if not initialize them
    ifstream test(PROMO_FILE);
    if (!test) {
        initializePromoCodes();
        initializeRiders();
    }
    test.close();
    
    int ch;
    do {
        showHeader("FOOD DELIVERY SYSTEM");
        cout << "\n";
        cout << "       Welcome to FoodExpress!" << endl;
        cout << "    Your Favorite Meals, Delivered Fast" << endl;
        cout << "\n1. Customer Portal" << endl;
        cout << "2. Admin Panel" << endl;
        cout << "3. Exit" << endl;
        printLine('=', 50);
        cout << "Enter Your Choice: ";
        cin >> ch;

        switch(ch) {
            case 1: customerMenu(); break;
            case 2: adminMenu(); break;
            case 3: 
                showHeader("THANK YOU");
                cout << "\n   Thank you for choosing FoodExpress!" << endl;
                cout << "         Visit Again!" << endl << endl;
                break;
            default: 
                cout << "[ERROR] Invalid choice!" << endl;
                pauseScreen();
        }

    } while (ch != 3);

    return 0;
}