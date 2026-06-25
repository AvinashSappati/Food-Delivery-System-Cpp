#pragma once
#include <string>

// Wallet Functions
int getWalletBalance(const std::string& phone);
void updateWallet(const std::string& phone, int amount);
void rechargeWallet();

// Promo Code Functions
void initializePromoCodes();
int validatePromoCode(const std::string& code);

// Rider Functions
void initializeRiders();
int assignRider();
std::string getRiderName(int riderId);

// Delivery Calculation
int calculateDeliveryTime(const std::string& area);
int calculateDeliveryFee(const std::string& area);