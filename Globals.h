#pragma once
#include <string>

const int FOOD_COUNT = 12;
const std::string ORDER_FILE = "orders.txt";
const std::string FEEDBACK_FILE = "feedback.txt";
const std::string RIDER_FILE = "riders.txt";
const std::string PROMO_FILE = "promocodes.txt";
const std::string WALLET_FILE = "wallet.txt";
const int ADMIN_PIN = 5944;
const double TAX_RATE = 0.05;

struct FoodItem {
    std::string name;
    int price;
    std::string category;
    bool available;
};

// Declares the menu array so other files know it exists
extern FoodItem menu[FOOD_COUNT];