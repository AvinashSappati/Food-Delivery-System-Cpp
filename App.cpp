#include "App.h"
#include "Globals.h"
#include "Utils.h"
#include "Services.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

void showFoodMenu() {
    cout << "\n";
    printLine('-', 75);
    cout << left << setw(5) << "No." 
         << setw(20) << "Item" 
         << setw(15) << "Category"
         << right << setw(10) << "Price"
         << setw(15) << "Status" << endl;
    printLine('-', 75);
    
    for (int i = 0; i < FOOD_COUNT; i++) {
        cout << left << setw(5) << (i + 1)
             << setw(20) << menu[i].name
             << setw(15) << menu[i].category
             << right << setw(8) << "Rs " << menu[i].price
             << setw(15) << (menu[i].available ? "Available" : "Unavailable") << endl;
    }
    printLine('-', 75);
}

void searchMenu() {
    showHeader("SEARCH MENU");
    
    string keyword;
    cin.ignore();
    cout << "\nEnter item name or category: ";
    getline(cin, keyword);
    
    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    
    cout << "\n[SEARCH RESULTS]" << endl;
    printLine('-', 70);
    
    bool found = false;
    for (int i = 0; i < FOOD_COUNT; i++) {
        string itemName = menu[i].name;
        string category = menu[i].category;
        transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
        transform(category.begin(), category.end(), category.begin(), ::tolower);
        
        if (itemName.find(keyword) != string::npos || 
            category.find(keyword) != string::npos) {
            cout << (i + 1) << ". " << menu[i].name 
                 << " - Rs " << menu[i].price
                 << " (" << menu[i].category << ")"
                 << (menu[i].available ? " [Available]" : " [Unavailable]") << endl;
            found = true;
        }
    }
    
    if (!found) {
        cout << "No items found!" << endl;
    }
    
    printLine('-', 70);
    pauseScreen();
}

void placeOrder() {
    showHeader("PLACE NEW ORDER");
    
    string name, phone, address;
    int choice, qty;
    char more;
    int subtotal = 0;
    string items = "";
    int orderID = generateOrderID();

    cin.ignore();
    
    cout << "\n--- Customer Details ---" << endl;
    printLine('-', 40);
    
    cout << "Name: ";
    getline(cin, name);
    
    while (true) {
        cout << "Phone (11 digits): ";
        getline(cin, phone);
        if (validatePhone(phone)) break;
        cout << "[ERROR] Invalid phone number!" << endl;
    }
    
    cout << "Delivery Address: ";
    getline(cin, address);

    cout << "\n--- Order Items ---" << endl;
    bool hasValidItems = false;
    do {
        showFoodMenu();
        
        cout << "\nSelect item (1-" << FOOD_COUNT << "): ";
        cin >> choice;

        if (choice < 1 || choice > FOOD_COUNT) {
            cout << "[ERROR] Invalid choice!" << endl;
            pauseScreen();
            continue;
        }
        
        if (!menu[choice - 1].available) {
            cout << "[ERROR] Sorry, this item is currently unavailable!" << endl;
            pauseScreen();
            continue;
        }

        cout << "Quantity: ";
        cin >> qty;

        if (qty <= 0) {
            cout << "[ERROR] Invalid quantity!" << endl;
            pauseScreen();
            continue;
        }

        int itemTotal = qty * menu[choice - 1].price;
        subtotal += itemTotal;
        items += menu[choice - 1].name + "x" + to_string(qty) + ";";
        hasValidItems = true;
        
        cout << "[ADDED] " << qty << " x " << menu[choice - 1].name 
             << " = Rs " << itemTotal << endl;

        cout << "\nAdd more items? (y/n): ";
        cin >> more;

    } while (more == 'y' || more == 'Y');
    
    if (!hasValidItems || subtotal == 0) {
        cout << "\n[ERROR] No items added to cart. Order cancelled." << endl;
        pauseScreen();
        return;
    }

    int deliveryFee = calculateDeliveryFee(address);
    int deliveryTime = calculateDeliveryTime(address);
    double tax = subtotal * TAX_RATE;
    
    int promoDiscount = 0;
    cout << "\nHave a promo code? (y/n): ";
    char hasPromo;
    cin >> hasPromo;
    
    if (hasPromo == 'y' || hasPromo == 'Y') {
        string promoCode;
        cin.ignore();
        cout << "Enter Promo Code: ";
        getline(cin, promoCode);
        
        promoDiscount = validatePromoCode(promoCode);
        if (promoDiscount > 0) {
            cout << "[SUCCESS] Promo code applied! Rs " << promoDiscount << " off" << endl;
        } else {
            cout << "[ERROR] Invalid or expired promo code!" << endl;
        }
    }
    
    int orderDiscount = 0;
    if (subtotal >= 1500) {
        orderDiscount = subtotal * 0.15;
        cout << "[DISCOUNT] 15% off on orders above Rs 1500!" << endl;
    } else if (subtotal >= 1000) {
        orderDiscount = subtotal * 0.10;
        cout << "[DISCOUNT] 10% off on orders above Rs 1000!" << endl;
    }
    
    int totalDiscount = orderDiscount + promoDiscount;
    int finalTotal = subtotal + tax + deliveryFee - totalDiscount;
    
    cout << "\n--- Payment Method ---" << endl;
    cout << "1. Cash on Delivery" << endl;
    cout << "2. Wallet Payment" << endl;
    cout << "Choose: ";
    int paymentChoice;
    cin >> paymentChoice;
    
    string paymentMethod = "Cash";
    int walletBalance = getWalletBalance(phone);
    
    if (paymentChoice == 2) {
        cout << "Wallet Balance: Rs " << walletBalance << endl;
        
        if (walletBalance >= finalTotal) {
            updateWallet(phone, -finalTotal);
            paymentMethod = "Wallet";
            cout << "[SUCCESS] Payment successful! Remaining balance: Rs " 
                 << (walletBalance - finalTotal) << endl;
        } else {
            cout << "[ERROR] Insufficient balance! Switching to Cash on Delivery." << endl;
            paymentMethod = "Cash";
        }
    }
    
    int riderId = assignRider();
    
    ofstream out(ORDER_FILE, ios::app);
    out << orderID << ","
        << name << ","
        << phone << ","
        << address << ","
        << items << ","
        << subtotal << ","
        << totalDiscount << ","
        << (int)tax << ","
        << deliveryFee << ","
        << finalTotal << ","
        << "Pending" << ","
        << paymentMethod << ","
        << riderId << ","
        << deliveryTime << ","
        << getCurrentDateTime() << "\n";
    out.close();

    showHeader("ORDER RECEIPT");
    cout << "\n";
    cout << "Order ID: #" << orderID << endl;
    cout << "Date/Time: " << getCurrentDateTime() << endl;
    cout << "Customer: " << name << endl;
    cout << "Phone: " << phone << endl;
    cout << "Address: " << address << endl;
    printLine('-', 50);
    cout << "Subtotal:" << setw(35) << "Rs " << subtotal << endl;
    cout << "GST (5%):" << setw(35) << "Rs " << (int)tax << endl;
    cout << "Delivery Fee:" << setw(31) << "Rs " << deliveryFee << endl;
    if (totalDiscount > 0) {
        cout << "Discount:" << setw(35) << "- Rs " << totalDiscount << endl;
    }
    printLine('-', 50);
    cout << "TOTAL:" << setw(38) << "Rs " << finalTotal << endl;
    printLine('=', 50);
    cout << "\n[SUCCESS] Order placed successfully!" << endl;
    cout << "Payment: " << paymentMethod << endl;
    cout << "Rider: " << getRiderName(riderId) << " (ID: " << riderId << ")" << endl;
    cout << "Status: Order Confirmed" << endl;
    cout << "Estimated Delivery: " << deliveryTime << " minutes" << endl;
    
    pauseScreen();
}

void cancelOrder() {
    showHeader("CANCEL ORDER");
    
    int orderID;
    string phone;
    
    cout << "\nEnter Order ID: ";
    cin >> orderID;
    
    cin.ignore();
    cout << "Enter Phone Number: ";
    getline(cin, phone);
    
    ifstream in(ORDER_FILE);
    ofstream out("temp.txt");
    
    if (!in) {
        cout << "[ERROR] No orders found!" << endl;
        pauseScreen();
        return;
    }
    
    string line;
    bool found = false;
    bool canCancel = false;
    
    while (getline(in, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex >= 15 && stoi(parts[0]) == orderID && parts[2] == phone) {
            found = true;
            
            if (parts[10] == "Pending") {
                canCancel = true;
                cout << "\n[SUCCESS] Order cancelled successfully!" << endl;
                
                if (parts[11] == "Wallet") {
                    updateWallet(phone, stoi(parts[9]));
                    cout << "[REFUND] Rs " << parts[9] << " refunded to wallet!" << endl;
                }
                continue;
            } else {
                cout << "[ERROR] Cannot cancel! Order is already " << parts[10] << endl;
            }
        }
        
        out << line << "\n";
    }
    
    in.close();
    out.close();
    
    if (canCancel) {
        remove(ORDER_FILE.c_str());
        rename("temp.txt", ORDER_FILE.c_str());
    } else {
        remove("temp.txt");
        if (!found) {
            cout << "[ERROR] Order not found or phone number mismatch!" << endl;
        }
    }
    
    pauseScreen();
}

void trackOrder() {
    showHeader("TRACK YOUR ORDER");
    
    int orderID;
    cout << "\nEnter Order ID: ";
    cin >> orderID;

    ifstream file(ORDER_FILE);
    if (!file) {
        cout << "[ERROR] No orders found!" << endl;
        pauseScreen();
        return;
    }

    string line;
    bool found = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex < 15) continue;
        
        if (stoi(parts[0]) == orderID) {
            found = true;
            
            cout << "\n--- Order Tracking ---" << endl;
            printLine('-', 55);
            cout << "Order ID: #" << parts[0] << endl;
            cout << "Placed on: " << parts[14] << endl;
            cout << "Customer: " << parts[1] << endl;
            cout << "Delivery Address: " << parts[3] << endl;
            printLine('-', 55);
            
            cout << "\n--- Order Status ---" << endl;
            string status = parts[10];
            
            if (status == "Pending") {
                cout << "[DONE] Order Confirmed" << endl;
                cout << "[CURRENT] Preparing..." << endl;
                cout << "[PENDING] Out for Delivery" << endl;
                cout << "[PENDING] Delivered" << endl;
            } else if (status == "Preparing") {
                cout << "[DONE] Order Confirmed" << endl;
                cout << "[DONE] Preparing..." << endl;
                cout << "[CURRENT] Out for Delivery" << endl;
                cout << "[PENDING] Delivered" << endl;
            } else if (status == "Out for Delivery") {
                cout << "[DONE] Order Confirmed" << endl;
                cout << "[DONE] Preparing..." << endl;
                cout << "[DONE] Out for Delivery" << endl;
                cout << "[CURRENT] Delivering..." << endl;
            } else if (status == "Delivered") {
                cout << "[DONE] Order Confirmed" << endl;
                cout << "[DONE] Preparing..." << endl;
                cout << "[DONE] Out for Delivery" << endl;
                cout << "[DONE] Delivered" << endl;
            }
            
            cout << "\nRider: " << getRiderName(stoi(parts[12])) << " (ID: " << parts[12] << ")" << endl;
            cout << "Estimated Time: " << parts[13] << " minutes" << endl;
            cout << "Total Amount: Rs " << parts[9] << endl;
            cout << "Payment: " << parts[11] << endl;
            printLine('-', 55);
            break;
        }
    }
    
    if (!found) {
        cout << "[ERROR] Order not found!" << endl;
    }
    
    file.close();
    pauseScreen();
}

void rateOrder() {
    showHeader("RATE YOUR ORDER");
    
    int orderID, rating;
    string feedback;
    
    cout << "\nEnter Order ID: ";
    cin >> orderID;
    
    cout << "Rate your experience (1-5 stars): ";
    cin >> rating;
    
    if (rating < 1 || rating > 5) {
        cout << "[ERROR] Invalid rating!" << endl;
        pauseScreen();
        return;
    }
    
    cin.ignore();
    cout << "Your Feedback (optional): ";
    getline(cin, feedback);
    
    ofstream out(FEEDBACK_FILE, ios::app);
    out << orderID << "," << rating << "," << feedback << "," << getCurrentDateTime() << "\n";
    out.close();
    
    cout << "\n[SUCCESS] Thank you for your feedback!" << endl;
    
    if (rating >= 4) {
        cout << "[REWARD] You earned 50 loyalty points!" << endl;
    }
    
    pauseScreen();
}

void viewOrders() {
    showHeader("ALL ORDERS");
    
    ifstream file(ORDER_FILE);
    if (!file) {
        cout << "[ERROR] No orders found!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n";
    printLine('-', 100);
    cout << left << setw(8) << "ID" 
         << setw(15) << "Customer"
         << setw(13) << "Phone"
         << setw(10) << "Total"
         << setw(15) << "Status"
         << setw(10) << "Rider"
         << setw(18) << "Date/Time" << endl;
    printLine('-', 100);

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex < 15) continue;

        cout << left << setw(8) << parts[0]
             << setw(15) << parts[1]
             << setw(13) << parts[2]
             << setw(10) << parts[9]
             << setw(15) << parts[10]
             << setw(10) << parts[12]
             << setw(18) << parts[14] << endl;
    }
    
    printLine('-', 100);
    file.close();
    pauseScreen();
}

void updateOrderStatus() {
    showHeader("UPDATE ORDER STATUS");
    
    int orderID;
    cout << "\nEnter Order ID: ";
    cin >> orderID;
    
    cout << "\n--- Select New Status ---" << endl;
    cout << "1. Pending" << endl;
    cout << "2. Preparing" << endl;
    cout << "3. Out for Delivery" << endl;
    cout << "4. Delivered" << endl;
    cout << "Choose: ";
    
    int choice;
    cin >> choice;
    
    string newStatus;
    switch(choice) {
        case 1: newStatus = "Pending"; break;
        case 2: newStatus = "Preparing"; break;
        case 3: newStatus = "Out for Delivery"; break;
        case 4: newStatus = "Delivered"; break;
        default:
            cout << "[ERROR] Invalid choice!" << endl;
            pauseScreen();
            return;
    }
    
    ifstream in(ORDER_FILE);
    ofstream out("temp.txt");
    
    if (!in) {
        cout << "[ERROR] No orders found!" << endl;
        pauseScreen();
        return;
    }
    
    string line;
    bool found = false;
    
    while (getline(in, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex >= 15 && stoi(parts[0]) == orderID) {
            found = true;
            out << parts[0] << "," << parts[1] << "," << parts[2] << "," 
                << parts[3] << "," << parts[4] << "," << parts[5] << "," 
                << parts[6] << "," << parts[7] << "," << parts[8] << "," 
                << parts[9] << "," << newStatus << "," << parts[11] << "," 
                << parts[12] << "," << parts[13] << "," << parts[14] << "\n";
        } else {
            out << line << "\n";
        }
    }
    
    in.close();
    out.close();
    
    if (found) {
        remove(ORDER_FILE.c_str());
        rename("temp.txt", ORDER_FILE.c_str());
        cout << "\n[SUCCESS] Order status updated to: " << newStatus << endl;
    } else {
        remove("temp.txt");
        cout << "[ERROR] Order not found!" << endl;
    }
    
    pauseScreen();
}

void toggleItemAvailability() {
    showHeader("MANAGE MENU ITEMS");
    
    showFoodMenu();
    
    cout << "\nEnter item number to toggle availability: ";
    int choice;
    cin >> choice;
    
    if (choice < 1 || choice > FOOD_COUNT) {
        cout << "[ERROR] Invalid choice!" << endl;
        pauseScreen();
        return;
    }
    
    menu[choice - 1].available = !menu[choice - 1].available;
    
    cout << "[SUCCESS] " << menu[choice - 1].name << " is now " 
         << (menu[choice - 1].available ? "Available" : "Unavailable") << endl;
    
    pauseScreen();
}

void viewRiders() {
    showHeader("DELIVERY RIDERS");
    
    ifstream file(RIDER_FILE);
    if (!file) {
        initializeRiders();
        file.open(RIDER_FILE);
    }
    
    cout << "\n";
    printLine('-', 75);
    cout << left << setw(8) << "ID"
         << setw(20) << "Name"
         << setw(15) << "Phone"
         << setw(15) << "Status"
         << setw(12) << "Deliveries" << endl;
    printLine('-', 75);
    
    string line;
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
        
        cout << left << setw(8) << id
             << setw(20) << name
             << setw(15) << phone
             << setw(15) << status
             << setw(12) << deliveries << endl;
    }
    
    printLine('-', 75);
    file.close();
    pauseScreen();
}

void salesSummary() {
    showHeader("SALES SUMMARY & ANALYTICS");
    
    ifstream file(ORDER_FILE);
    if (!file) {
        cout << "[ERROR] No orders found!" << endl;
        pauseScreen();
        return;
    }

    string line;
    int totalOrders = 0, totalRevenue = 0, totalDiscount = 0;
    int pending = 0, preparing = 0, outForDelivery = 0, delivered = 0;
    int cashOrders = 0, walletOrders = 0;
    int totalTax = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex < 15) continue;

        totalRevenue += stoi(parts[9]);
        totalDiscount += stoi(parts[6]);
        totalTax += stoi(parts[7]);
        totalOrders++;

        if (parts[10] == "Pending") pending++;
        else if (parts[10] == "Preparing") preparing++;
        else if (parts[10] == "Out for Delivery") outForDelivery++;
        else delivered++;
        
        if (parts[11] == "Cash") cashOrders++;
        else walletOrders++;
    }

    cout << "\n--- Sales Statistics ---" << endl;
    printLine('=', 60);
    cout << "Total Orders:" << setw(38) << totalOrders << endl;
    cout << "  - Pending:" << setw(40) << pending << endl;
    cout << "  - Preparing:" << setw(38) << preparing << endl;
    cout << "  - Out for Delivery:" << setw(31) << outForDelivery << endl;
    cout << "  - Delivered:" << setw(38) << delivered << endl;
    printLine('-', 60);
    
    cout << "\n--- Revenue Analysis ---" << endl;
    printLine('=', 60);
    cout << "Total Revenue:" << setw(35) << "Rs " << totalRevenue << endl;
    cout << "Total Discounts:" << setw(33) << "Rs " << totalDiscount << endl;
    cout << "Total Tax (GST):" << setw(33) << "Rs " << totalTax << endl;
    printLine('-', 60);
    
    cout << "\n--- Payment Methods ---" << endl;
    printLine('=', 60);
    cout << "Cash on Delivery:" << setw(32) << cashOrders << " orders" << endl;
    cout << "Wallet Payment:" << setw(34) << walletOrders << " orders" << endl;
    printLine('-', 60);
    
    if (totalOrders > 0) {
        cout << "\n--- Key Metrics ---" << endl;
        printLine('=', 60);
        cout << "Average Order Value:" << setw(27) << "Rs " << (totalRevenue / totalOrders) << endl;
        cout << "Completion Rate:" << setw(33) << (delivered * 100 / totalOrders) << "%" << endl;
        cout << "Success Rate:" << setw(36) << ((delivered + outForDelivery) * 100 / totalOrders) << "%" << endl;
        printLine('=', 60);
    }
    
    file.close();
    pauseScreen();
}

void viewFeedback() {
    showHeader("CUSTOMER FEEDBACK & RATINGS");
    
    ifstream file(FEEDBACK_FILE);
    if (!file) {
        cout << "[ERROR] No feedback found!" << endl;
        pauseScreen();
        return;
    }

    string line;
    int count = 1;
    int totalRating = 0;
    int ratingCount = 0;
    
    cout << "\n";
    while (getline(file, line)) {
        stringstream ss(line);
        string orderID, feedback, dateTime;
        int rating;
        
        getline(ss, orderID, ',');
        ss >> rating;
        ss.ignore();
        getline(ss, feedback, ',');
        getline(ss, dateTime);
        
        totalRating += rating;
        ratingCount++;
        
        printLine('-', 65);
        cout << "#" << count++ << " | Order #" << orderID << " | Rating: " << rating << "/5 (";
        for (int i = 0; i < rating; i++) cout << "*";
        cout << ")" << endl;
        cout << dateTime << endl;
        if (!feedback.empty()) {
            cout << "Feedback: " << feedback << endl;
        }
    }
    printLine('-', 65);
    
    if (ratingCount > 0) {
        double avgRating = (double)totalRating / ratingCount;
        cout << "\nAverage Rating: " << fixed << setprecision(1) << avgRating << "/5.0";
        cout << " (" << ratingCount << " reviews)" << endl;
    }
    
    file.close();
    pauseScreen();
}

void generateReport() {
    showHeader("GENERATE BUSINESS REPORT");
    
    cout << "\n[INFO] Generating comprehensive report..." << endl << endl;
    
    ofstream report("business_report.txt");
    report << "========================================\n";
    report << "   FOOD DELIVERY SYSTEM - BUSINESS REPORT\n";
    report << "   Generated: " << getCurrentDateTime() << "\n";
    report << "========================================\n\n";
    
    ifstream orderFile(ORDER_FILE);
    int totalOrders = 0, revenue = 0;
    string line;
    
    while (getline(orderFile, line)) {
        stringstream ss(line);
        string parts[15];
        int partIndex = 0;
        string temp;
        
        while (getline(ss, temp, ',') && partIndex < 15) {
            parts[partIndex++] = temp;
        }
        
        if (partIndex >= 15) {
            totalOrders++;
            revenue += stoi(parts[9]);
        }
    }
    orderFile.close();
    
    report << "SALES SUMMARY:\n";
    report << "-----------------\n";
    report << "Total Orders: " << totalOrders << "\n";
    report << "Total Revenue: Rs " << revenue << "\n\n";
    
    ifstream feedbackFile(FEEDBACK_FILE);
    int totalRating = 0, feedbackCount = 0;
    
    while (getline(feedbackFile, line)) {
        stringstream ss(line);
        string temp;
        int rating;
        
        getline(ss, temp, ',');
        ss >> rating;
        
        totalRating += rating;
        feedbackCount++;
    }
    feedbackFile.close();
    
    if (feedbackCount > 0) {
        report << "CUSTOMER SATISFACTION:\n";
        report << "----------------------\n";
        report << "Total Reviews: " << feedbackCount << "\n";
        report << "Average Rating: " << fixed << setprecision(2) 
               << ((double)totalRating / feedbackCount) << "/5.0\n\n";
    }
    
    report << "========================================\n";
    report << "         END OF REPORT\n";
    report << "========================================\n";
    report.close();
    
    cout << "[SUCCESS] Report generated successfully!" << endl;
    cout << "[INFO] Saved as: business_report.txt" << endl;
    pauseScreen();
}

void customerMenu() {
    int ch;
    do {
        showHeader("CUSTOMER PORTAL");
        cout << "\n1. View Menu" << endl;
        cout << "2. Search Menu" << endl;
        cout << "3. Place Order" << endl;
        cout << "4. Track Order" << endl;
        cout << "5. Cancel Order" << endl;
        cout << "6. Rate Order" << endl;
        cout << "7. Wallet Recharge" << endl;
        cout << "8. Back to Main Menu" << endl;
        printLine('-', 40);
        cout << "Enter Choice: ";
        cin >> ch;

        switch(ch) {
            case 1: 
                showHeader("FOOD MENU");
                showFoodMenu();
                pauseScreen();
                break;
            case 2: searchMenu(); break;
            case 3: placeOrder(); break;
            case 4: trackOrder(); break;
            case 5: cancelOrder(); break;
            case 6: rateOrder(); break;
            case 7: rechargeWallet(); break;
            case 8: break;
            default: 
                cout << "[ERROR] Invalid choice!" << endl;
                pauseScreen();
        }

    } while (ch != 8);
}

void adminMenu() {
    showHeader("ADMIN LOGIN");
    
    int pin;
    cout << "\nEnter Admin PIN: ";
    cin >> pin;

    if (pin != ADMIN_PIN) {
        cout << "[ERROR] Incorrect PIN!" << endl;
        pauseScreen();
        return;
    }

    int ch;
    do {
        showHeader("ADMIN PANEL");
        cout << "\n1. View All Orders" << endl;
        cout << "2. Update Order Status" << endl;
        cout << "3. Toggle Item Availability" << endl;
        cout << "4. View Riders" << endl;
        cout << "5. Sales Summary" << endl;
        cout << "6. Customer Feedback" << endl;
        cout << "7. Generate Report" << endl;
        cout << "8. Logout" << endl;
        printLine('-', 40);
        cout << "Enter Choice: ";
        cin >> ch;

        switch(ch) {
            case 1: viewOrders(); break;
            case 2: updateOrderStatus(); break;
            case 3: toggleItemAvailability(); break;
            case 4: viewRiders(); break;
            case 5: salesSummary(); break;
            case 6: viewFeedback(); break;
            case 7: generateReport(); break;
            case 8: break;
            default: 
                cout << "[ERROR] Invalid choice!" << endl;
                pauseScreen();
        }

    } while (ch != 8);
}