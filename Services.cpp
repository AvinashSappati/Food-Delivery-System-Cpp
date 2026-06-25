#include "Services.h"
#include "Globals.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

/* =========================
   WALLET FUNCTIONS
   ========================= */
int getWalletBalance(const string& phone) {
    ifstream file(WALLET_FILE);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string p;
        int balance;
        getline(ss, p, ',');
        ss >> balance;
        if (p == phone) {
            file.close();
            return balance;
        }
    }
    file.close();
    return 0;
}

void updateWallet(const string& phone, int amount) {
    ifstream in(WALLET_FILE);
    ofstream out("temp_wallet.txt");
    string line;
    bool found = false;
    
    while (getline(in, line)) {
        stringstream ss(line);
        string p;
        int balance;
        getline(ss, p, ',');
        ss >> balance;
        
        if (p == phone) {
            found = true;
            out << p << "," << (balance + amount) << "\n";
        } else {
            out << line << "\n";
        }
    }
    
    if (!found) {
        out << phone << "," << amount << "\n";
    }
    
    in.close();
    out.close();
    remove(WALLET_FILE.c_str());
    rename("temp_wallet.txt", WALLET_FILE.c_str());
}

void rechargeWallet() {
    showHeader("WALLET RECHARGE");
    
    string phone;
    int amount;
    
    cin.ignore();
    cout << "\nEnter Phone Number: ";
    getline(cin, phone);
    
    int currentBalance = getWalletBalance(phone);
    cout << "Current Balance: Rs " << currentBalance << endl;
    
    cout << "Enter Recharge Amount: Rs ";
    cin >> amount;
    
    if (amount < 100) {
        cout << "[ERROR] Minimum recharge amount is Rs 100!" << endl;
        pauseScreen();
        return;
    }
    
    updateWallet(phone, amount);
    cout << "\n[SUCCESS] Wallet recharged successfully!" << endl;
    cout << "New Balance: Rs " << (currentBalance + amount) << endl;
    pauseScreen();
}

/* =========================
   PROMO CODE FUNCTIONS
   ========================= */
void initializePromoCodes() {
    ofstream file(PROMO_FILE);
    file << "FIRST50,50,50\n";
    file << "SAVE100,100,30\n";
    file << "WELCOME20,20,100\n";
    file.close();
}

int validatePromoCode(const string& code) {
    ifstream file(PROMO_FILE);
    string line;
    
    while (getline(file, line)) {
        stringstream ss(line);
        string promoCode;
        int discount, uses;
        
        getline(ss, promoCode, ',');
        ss >> discount;
        ss.ignore();
        ss >> uses;
        
        if (promoCode == code && uses > 0) {
            file.close();
            
            ifstream in(PROMO_FILE);
            ofstream out("temp_promo.txt");
            string l;
            while (getline(in, l)) {
                if (l.find(code) != string::npos) {
                    out << promoCode << "," << discount << "," << (uses - 1) << "\n";
                } else {
                    out << l << "\n";
                }
            }
            in.close();
            out.close();
            remove(PROMO_FILE.c_str());
            rename("temp_promo.txt", PROMO_FILE.c_str());
            
            return discount;
        }
    }
    
    file.close();
    return 0;
}

/* =========================
   RIDER FUNCTIONS
   ========================= */
void initializeRiders() {
    ofstream file(RIDER_FILE);
    file << "201,Ahmed Ali,03001234567,Available,45\n";
    file << "202,Hassan Khan,03009876543,Available,38\n";
    file << "203,Bilal Shah,03012345678,Busy,52\n";
    file << "204,Usman Tariq,03019876543,Available,29\n";
    file.close();
}

int assignRider() {
    ifstream file(RIDER_FILE);
    string line;
    int assignedRider = 201;
    
    while (getline(file, line)) {
        stringstream ss(line);
        int id, deliveries;
        string name, phone, status;
        
        ss >> id;
        ss.ignore();
        getline(ss, name, ',');
        getline(ss, phone, ',');
        getline(ss, status, ',');
        ss >> deliveries;
        
        if (status == "Available") {
            assignedRider = id;
            break;
        }
    }
    
    file.close();
    return assignedRider;
}

string getRiderName(int riderId) {
    ifstream file(RIDER_FILE);
    string line;
    
    while (getline(file, line)) {
        stringstream ss(line);
        int id;
        string name;
        
        ss >> id;
        ss.ignore();
        getline(ss, name, ',');
        
        if (id == riderId) {
            file.close();
            return name;
        }
    }
    
    file.close();
    return "Not Assigned";
}

/* =========================
   DELIVERY CALCULATION
   ========================= */
int calculateDeliveryTime(const string& area) {
    string areaLower = area;
    transform(areaLower.begin(), areaLower.end(), areaLower.begin(), ::tolower);
    
    if (areaLower.find("johar") != string::npos || 
        areaLower.find("gulberg") != string::npos) return 25;
    if (areaLower.find("dha") != string::npos || 
        areaLower.find("cantt") != string::npos) return 35;
    if (areaLower.find("model") != string::npos) return 30;
    return 40;
}

int calculateDeliveryFee(const string& area) {
    string areaLower = area;
    transform(areaLower.begin(), areaLower.end(), areaLower.begin(), ::tolower);
    
    if (areaLower.find("johar") != string::npos || 
        areaLower.find("gulberg") != string::npos) return 50;
    if (areaLower.find("dha") != string::npos || 
        areaLower.find("cantt") != string::npos) return 80;
    if (areaLower.find("model") != string::npos) return 60;
    return 100;
}