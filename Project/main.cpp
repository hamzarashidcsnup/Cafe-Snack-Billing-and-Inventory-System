#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

const int MAX = 50;

void saveData(int* itemID, string* itemName, double* price, int* stock, int count) {
    ofstream outFile("inventory.txt");
    outFile << count << endl;
    for (int i = 0; i < count; i++) {
        outFile << *(itemID + i) << " " << *(price + i) << " " << *(stock + i) << endl;
        outFile << *(itemName + i) << endl;
    }
    outFile.close();
}

bool loadData(int* itemID, string* itemName, double* price, int* stock, int* soldCount, int& count) {
    ifstream inFile("inventory.txt");
    if (!inFile || !(inFile >> count)) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        inFile >> *(itemID + i) >> *(price + i) >> *(stock + i);
        inFile.ignore();
        getline(inFile, *(itemName + i));
        *(soldCount + i) = 0;
    }
    inFile.close();
    return true;
}

int findItembyID(int* itemID, int count, int searchID) {
    for (int i = 0; i < count; i++) {
        if (*(itemID + i) == searchID) {
            return i;
        }
    }
    return -1;
}

int findItembyName(string* itemName, int count, string searchName) {
    for (int i = 0; i < count; i++) {
        if (*(itemName + i) == searchName) {
            return i;
        }
    }
    return -1;
}

void displayMenu(int* itemID, string* itemName, double* price, int* stock, int count) {
    cout << "\n------------------------------------------------------------\n";
    cout << left << setw(10) << "ID"
         << setw(22) << "Item Name"
         << setw(15) << "Price (PKR)"
         << setw(10) << "Stock" << endl;
    cout << "------------------------------------------------------------\n";

    for (int i = 0; i < count; i++) {
        cout << left << setw(10) << *(itemID + i)
             << setw(22) << *(itemName + i)
             << setw(15) << fixed << setprecision(2) << *(price + i)
             << setw(10) << *(stock + i);

        if (*(stock + i) < 5) {
            cout << " [LOW STOCK]";
        }
        cout << endl;
    }
    cout << "------------------------------------------------------------\n";
}

double calculateDiscount(double subtotal) {
    double discountPercent = 10.0;
    return subtotal * (discountPercent / 100.0);
}

void takeOrder(int* itemID, string* itemName, double* price, int* stock, int* soldCount, int count, int& totalOrders) {
    displayMenu(itemID, itemName, price, stock, count);

    string cartName[30];
    int cartQty[30];
    double cartTotal[30];
    int cartCount = 0;
    double subtotal = 0.0;

    char addMore = 'y';
    while ((addMore == 'y' || addMore == 'Y') && cartCount < 30) {
        int id, qty;
        cout << "\nEnter Item ID to order: ";
        cin >> id;

        int idx = findItembyID(itemID, count, id);
        if (idx == -1) {
            cout << "Error: Item ID not found!\n";
        } else {
            cout << "Enter quantity for " << *(itemName + idx) << ": ";
            cin >> qty;

            if (qty <= 0) {
                cout << "Error: Quantity must be greater than 0.\n";
            } else if (*(stock + idx) < qty) {
                cout << "Sorry! Only " << *(stock + idx) << " units available in stock.\n";
            } else {
                *(stock + idx) -= qty;
                *(soldCount + idx) += qty;

                double lineCost = *(price + idx) * qty;
                subtotal += lineCost;

                cartName[cartCount] = *(itemName + idx);
                cartQty[cartCount] = qty;
                cartTotal[cartCount] = lineCost;
                cartCount++;

                cout << "Added to cart!\n";
            }
        }

        cout << "Add another item? (y/n): ";
        cin >> addMore;
    }

    if (cartCount == 0) {
        cout << "Order cancelled (no items added).\n";
        return;
    }

    double discountAmount = calculateDiscount(subtotal);
    double finalTotal = subtotal - discountAmount;

    double cashTendered = 0.0;
    cout << "\nSubtotal:   PKR " << fixed << setprecision(2) << subtotal << endl;
    cout << "Discount:  -PKR " << discountAmount << endl;
    cout << "Final Bill: PKR " << finalTotal << endl;

    do {
        cout << "Enter Cash Tendered (PKR): ";
        cin >> cashTendered;
        if (cashTendered < finalTotal) {
            cout << "Insufficient cash! Customer still owes PKR " << (finalTotal - cashTendered) << ".\n";
        }
    } while (cashTendered < finalTotal);

    double change = cashTendered - finalTotal;
    totalOrders++;
    saveData(itemID, itemName, price, stock, count);

    cout << "\n============================================\n";
    cout << "              CUSTOMER RECEIPT";
    cout << "\n============================================\n";
    cout << left << setw(20) << "Item" << setw(8) << "Qty" << setw(12) << "Total" << endl;
    cout << "--------------------------------------------\n";
    for (int i = 0; i < cartCount; i++) {
        cout << left << setw(20) << cartName[i]
             << setw(8) << cartQty[i]
             << "PKR " << fixed << setprecision(2) << cartTotal[i] << endl;
    }
    cout << "--------------------------------------------\n";
    cout << "Subtotal:      PKR " << subtotal << endl;
    cout << "Discount:     -PKR " << discountAmount << endl;
    cout << "Grand Total:   PKR " << finalTotal << endl;
    cout << "Cash Paid:     PKR " << cashTendered << endl;
    cout << "Change Back:   PKR " << change << endl;
    cout << "============================================\n";
}

void restockItem(int* itemID, string* itemName, double* price, int* stock, int* soldCount, int& count, int maxSize) {
    int subChoice;
    cout << "\n--- MANAGE INVENTORY ---\n";
    cout << "1. Restock Existing Item\n";
    cout << "2. Update Item Price\n";
    cout << "3. Add New Item\n";
    cout << "Choose option (1-3): ";
    cin >> subChoice;

    if (subChoice == 1 || subChoice == 2) {
        int searchType, idx = -1;
        cout << "1. Search by ID.\n2. Search by name.\nEnter your choice (1-2): ";
        cin >> searchType;

        if (searchType == 1) {
            int id;
            cout << "Enter Item ID: ";
            cin >> id;
            idx = findItembyID(itemID, count, id);
        } else {
            string name;
            cout << "Enter exact Item Name: ";
            cin.ignore();
            getline(cin, name);
            idx = findItembyName(itemName, count, name);
        }

        if (idx == -1) {
            cout << "Item not found!\n";
            return;
        }

        if (subChoice == 1) {
            int addQty;
            cout << "Current stock of " << *(itemName + idx) << ": " << *(stock + idx) << endl;
            cout << "Enter quantity to add: ";
            cin >> addQty;
            if (addQty > 0) {
                *(stock + idx) += addQty;
                saveData(itemID, itemName, price, stock, count);
                cout << "Stock updated! New stock: " << *(stock + idx) << endl;
            }
        } else {
            double newPrice;
            cout << "Current price of " << *(itemName + idx) << ": PKR " << *(price + idx) << endl;
            cout << "Enter new price (PKR): ";
            cin >> newPrice;
            if (newPrice > 0) {
                *(price + idx) = newPrice;
                saveData(itemID, itemName, price, stock, count);
                cout << "Price updated successfully!\n";
            }
        }
    } else if (subChoice == 3) {
        if (count >= maxSize) {
            cout << "Catalog is full! Cannot add more items.\n";
            return;
        }
        int newID, newStock;
        string newName;
        double newPrice;

        cout << "Enter new Item ID: ";
        cin >> newID;
        if (findItembyID(itemID, count, newID) != -1) {
            cout << "Error: This Item ID already exists!\n";
            return;
        }

        cout << "Enter Item Name: ";
        cin.ignore();
        getline(cin, newName);

        cout << "Enter Unit Price (PKR): ";
        cin >> newPrice;

        cout << "Enter Initial Stock: ";
        cin >> newStock;

        *(itemID + count) = newID;
        *(itemName + count) = newName;
        *(price + count) = newPrice;
        *(stock + count) = newStock;
        *(soldCount + count) = 0;
        count++;
        saveData(itemID, itemName, price, stock, count);

        cout << "New item added to menu!\n";
    } else {
        cout << "Invalid option!\n";
    }
}

void dailyReport(int* itemID, string* itemName, double* price, int* stock, int* soldCount, int count, int totalOrders) {
    double totalRevenue = 0.0;
    int bestSellerIdx = 0;
    bool anyLowStock = false;

    cout << "\n============================================\n";
    cout << "        LOW-STOCK ALERTS (< 5 UNITS)\n";
    cout << "============================================\n";

    for (int i = 0; i < count; i++) {
        if (*(stock + i) < 5) {
            cout << "WARNING: " << *(itemName + i) << " (ID: " << *(itemID + i)
                 << ") only has " << *(stock + i) << " left!\n";
            anyLowStock = true;
        }
    }
    if (!anyLowStock) {
        cout << "All items have healthy stock levels.\n";
    }

    cout << "\n============================================\n";
    cout << "          END-OF-DAY SALES SUMMARY";
    cout << "\n============================================\n";
    cout << left << setw(20) << "Item Name" << setw(12) << "Units Sold" << setw(15) << "Revenue (PKR)" << endl;
    cout << "--------------------------------------------\n";

    for (int i = 0; i < count; i++) {
        double itemRevenue = *(price + i) * *(soldCount + i);
        totalRevenue += itemRevenue;

        if (*(soldCount + i) > *(soldCount + bestSellerIdx)) {
            bestSellerIdx = i;
        }

        cout << left << setw(20) << *(itemName + i)
             << setw(12) << *(soldCount + i)
             << fixed << setprecision(2) << itemRevenue << endl;
    }

    cout << "--------------------------------------------\n";
    cout << "Total Orders Completed: " << totalOrders << endl;
    cout << "Total Daily Revenue: PKR " << totalRevenue << endl;

    if (*(soldCount + bestSellerIdx) > 0) {
        cout << "Best-Selling Item: " << *(itemName + bestSellerIdx)
             << " (" << *(soldCount + bestSellerIdx) << " sold)\n";
    } else {
        cout << "Best-Selling Item: No sales recorded yet\n";
    }
    cout << "============================================\n";
}

int main() {
    int count = 10;
    int totalOrders = 0;

    int* itemID = new int[MAX];
    string* itemName = new string[MAX];
    double* price = new double[MAX];
    int* stock = new int[MAX];
    int* soldCount = new int[MAX];

    if (!loadData(itemID, itemName, price, stock, soldCount, count)) {
        *(itemID + 0) = 101;
        *(itemName + 0) = "Americano";
        *(price + 0) = 100;
        *(stock + 0) = 20;
        *(soldCount + 0) = 0;

        *(itemID + 1) = 102;
        *(itemName + 1) = "Zinger Burger";
        *(price + 1) = 450;
        *(stock + 1) = 12;
        *(soldCount + 1) = 0;

        *(itemID + 2) = 103;
        *(itemName + 2) = "Loaded Fries";
        *(price + 2) = 250;
        *(stock + 2) = 4;
        *(soldCount + 2) = 0;

        *(itemID + 3) = 104;
        *(itemName + 3) = "Cold Coffee";
        *(price + 3) = 500;
        *(stock + 3) = 15;
        *(soldCount + 3) = 0;

        *(itemID + 4) = 105;
        *(itemName + 4) = "Shawarma";
        *(price + 4) = 200;
        *(stock + 4) = 7;
        *(soldCount + 4) = 0;

        *(itemID + 5) = 106;
        *(itemName + 5) = "Mango MilkShake";
        *(price + 5) = 150;
        *(stock + 5) = 20;
        *(soldCount + 5) = 0;

        *(itemID + 6) = 107;
        *(itemName + 6) = "Lapeta Burger";
        *(price + 6) = 350;
        *(stock + 6) = 30;
        *(soldCount + 6) = 0;

        *(itemID + 7) = 108;
        *(itemName + 7) = "Dates MilkShake";
        *(price + 7) = 170;
        *(stock + 7) = 15;
        *(soldCount + 7) = 0;

        *(itemID + 8) = 109;
        *(itemName + 8) = "Tea";
        *(price + 8) = 80;
        *(stock + 8) = 50;
        *(soldCount + 8) = 0;

        *(itemID + 9) = 110;
        *(itemName + 9) = "Zinger Paratha";
        *(price + 9) = 300;
        *(stock + 9) = 22;
        *(soldCount + 9) = 0;

        saveData(itemID, itemName, price, stock, count);
    }

    int userChoice;

    do {
        cout << "\n============================================\n";
        cout << "   CAFE & SNACK SHOP BILLING SYSTEM";
        cout << "\n============================================\n";
        cout << "1. View Menu & Stock\n2. New order.\n3. Manage Inventory.\n4. Low-Stock check & Daily Report.\n5. Exit.\nEnter your choice (1-5): ";
        cin >> userChoice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please try again." << endl;
            continue;
        }

        switch (userChoice) {
            case 1:
                displayMenu(itemID, itemName, price, stock, count);
                break;
            case 2:
                takeOrder(itemID, itemName, price, stock, soldCount, count, totalOrders);
                break;
            case 3:
                restockItem(itemID, itemName, price, stock, soldCount, count, MAX);
                break;
            case 4:
                dailyReport(itemID, itemName, price, stock, soldCount, count, totalOrders);
                break;
            case 5:
                saveData(itemID, itemName, price, stock, count);
                cout << "Thank you for visiting. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice! Please select 1 to 5.\n";
        }
    } while (userChoice != 5);

    delete[] itemID;
    delete[] itemName;
    delete[] price;
    delete[] stock;
    delete[] soldCount;

    return 0;
}