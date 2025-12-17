#include<iostream>
#include<fstream>
#include<conio.h>
#include<ctime>
#include<string>
#include<windows.h>
#include<vector>
#include<iomanip>
#include<algorithm>
#include<map>
using namespace std;

// Color definitions for enhanced UI
enum ConsoleColor {
    BLACK = 0,
    DARK_BLUE = 1,
    DARK_GREEN = 2,
    DARK_CYAN = 3,
    DARK_RED = 4,
    DARK_MAGENTA = 5,
    DARK_YELLOW = 6,
    GREY = 7,
    DARK_GREY = 8,
    BLUE = 9,
    GREEN = 10,
    CYAN = 11,
    RED = 12,
    MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};

// UI Enhancement Functions

// Set console text color with optional background
void setColor(int textColor, int bgColor = BLACK) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bgColor * 16 + textColor);
}

// Reset console color to default
void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREY);
}

// Set cursor position in console
void setCursorPosition(int x, int y) {
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Draw a decorative box
void drawBox(int width, int height, int x = 0, int y = 0, int color = CYAN) {
    setColor(color);

    // Top border
    setCursorPosition(x, y);
    cout << char(201);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(187);

    // Sides
    for (int i = 1; i < height - 1; i++) {
        setCursorPosition(x, y + i);
        cout << char(186);
        setCursorPosition(x + width - 1, y + i);
        cout << char(186);
    }

    // Bottom border
    setCursorPosition(x, y + height - 1);
    cout << char(200);
    for (int i = 0; i < width - 2; i++) cout << char(205);
    cout << char(188);

    resetColor();
}

// Print centered text with optional color
void centerText(string text, int width, int y, int textColor = WHITE) {
    int x = (width - text.length()) / 2;
    setCursorPosition(x, y);
    setColor(textColor);
    cout << text;
    resetColor();
}

// Draw a stylish input box
void drawInputBox(int x, int y, int width, string label = "") {
    setColor(CYAN);
    if (!label.empty()) {
        setCursorPosition(x, y);
        cout << label;
        y++;
    }

    // Input box outline
    setCursorPosition(x, y);
    cout << char(218) << string(width, char(196)) << char(191);

    setCursorPosition(x, y + 1);
    cout << char(179) << string(width, ' ') << char(179);

    setCursorPosition(x, y + 2);
    cout << char(192) << string(width, char(196)) << char(217);

    // Position cursor inside the box for input
    setCursorPosition(x + 2, y + 1);
    resetColor();
}

// Draw an animated progress bar
void drawProgressBar(int x, int y, int width, int durationMs, int barColor = GREEN) {
    int barWidth = width - 8; // Allow space for percentage
    int stepTime = durationMs / barWidth;

    for (int i = 0; i <= barWidth; i++) {
        float percentage = (float)i / barWidth * 100.0f;

        setCursorPosition(x, y);
        cout << "[";
        setColor(barColor);
        cout << string(i, '#');
        resetColor();
        cout << string(barWidth - i, ' ');
        cout << "] " << fixed << setprecision(1) << percentage << "%";

        Sleep(stepTime);
    }
}

// Draw a static progress bar (non-animated)
void drawStaticProgressBar(float percentage, int width, int x, int y, int barColor = GREEN) {
    int barWidth = width - 8; // Allow space for percentage
    int filledWidth = static_cast<int>(percentage * barWidth / 100.0);

    setCursorPosition(x, y);
    cout << "[";
    setColor(barColor);
    cout << string(filledWidth, '#');
    resetColor();
    cout << string(barWidth - filledWidth, ' ');
    cout << "] " << fixed << setprecision(1) << percentage << "%";
}

// Get password with asterisk masking
string getPassword() {
    string password = "";
    char ch;

    while (true) {
        ch = _getch();

        if (ch == 13) { // Enter key
            break;
        }
        else if (ch == 8) { // Backspace key
            if (password.length() > 0) {
                cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password += ch;
            cout << '*';
        }
    }

    return password;
}

// Global variables
string username, role;
float spend, bud, lmoney;
time_t now = time(0);
static char* dt = ctime(&now);
int l = strlen(dt);
bool flag = 0;
const string DATA_DIR = "budget_data/"; // Directory for storing data files

// Forward declarations
void next();
void intro();
void review();
float income();
void newf();
bool authenticate();
void registerUser();
void createDataDirectory();

// Base class for all user types
class User {
protected:
    string username;
    string userRole;
    float budget;

public:
    User(string uname, string role) : username(uname), userRole(role) {
        loadBudget();
    }

    virtual void displayDashboard() {
        system("cls");
        cout << "\n==========================================\n";
        cout << "Welcome, " << username << " (" << userRole << ")\n";
        cout << "Budget Left: RS" << fixed << setprecision(2) << budget << "\n";
        cout << "==========================================\n";
    }

    virtual void showSpecialFeatures() = 0; // Pure virtual function

    void loadBudget() {
        ifstream fp(DATA_DIR + username + "_budget.txt");
        if (fp) {
            fp >> budget;
        } else {
            budget = 0;
        }
        fp.close();
    }

    void saveBudget() {
        ofstream fp(DATA_DIR + username + "_budget.txt");
        fp << budget;
        fp.close();
    }

    void updateBudget(float amount) {
        budget = amount;
        saveBudget();
    }

    float getBudget() const { return budget; }
    string getUsername() const { return username; }
    string getRole() const { return userRole; }
};

// Utility functions
inline void back() {
    cout << "\nPress 'b' to go back.";
    char ch = getch();
    if (ch == 'b' || ch == 'B') intro();
}

void createDataDirectory() {
    system("mkdir budget_data 2>nul");
}

// Expense category management class
class Expense {
public:
    float ifood(float a);
    float icloth(float a);
    float iliving(float a);
    float itrans(float a);
    float ieduc(float a);
    float imed(float a);

    // New specialized expense categories
    float iinventory(float a); // For shopkeepers
    float iutilities(float a); // For all
    float iemployees(float a); // For shopkeepers and accountants
    float imaintenance(float a); // For shopkeepers
    float iofficesupplies(float a); // For accountants
    float itaxes(float a); // For accountants
    float itoolsequipment(float a); // For labour
    float imaterials(float a); // For labour
    float igroceries(float a); // For housewife
    float ihousehold(float a); // For housewife

    vector<string> getCategoriesForRole(string role);
} e;

// Role-specific user classes
class Housewife : public User {
public:
    Housewife(string uname) : User(uname, "Housewife") {}

    void displayDashboard() override {
        User::displayDashboard();
        cout << "Housewife Dashboard\n";
        cout << "1. View grocery expenses\n";
        cout << "2. Manage meal planner\n";
        cout << "3. Track household budget\n";
        cout << "4. Family expense allocation\n";
    }

    void showSpecialFeatures() override {
        system("cls");
        cout << "\n=== Housewife Special Features ===\n";
        cout << "1. Meal Planning\n";
        cout << "2. Family Budget Distribution\n";
        cout << "3. Grocery Shopping List\n";
        cout << "4. Household Inventory\n";
        cout << "5. Back to main menu\n";

        char choice = getch();
        switch(choice) {
            case '1': { mealPlanner(); break; }
            case '2': { familyBudget(); break; }
            case '3': { groceryList(); break; }
            case '4': { householdInventory(); break; }
            case '5': { return; }
            default: { showSpecialFeatures(); }
        }
    }

    void mealPlanner() {
        system("cls");
        cout << "\n=== Weekly Meal Planner ===\n";
        cout << "This feature will help you plan meals for the week\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void familyBudget() {
        system("cls");
        cout << "\n=== Family Budget Distribution ===\n";
        cout << "Allocate your budget across family members and needs\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void groceryList() {
        system("cls");
        cout << "\n=== Grocery Shopping List ===\n";
        cout << "Plan your grocery shopping efficiently\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void householdInventory() {
        system("cls");
        cout << "\n=== Household Inventory ===\n";
        cout << "Track household supplies and items\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }
};

class Shopkeeper : public User {
public:
    Shopkeeper(string uname) : User(uname, "Shopkeeper") {}

    void displayDashboard() override {
        User::displayDashboard();
        cout << "Shopkeeper Dashboard\n";
        cout << "1. View inventory costs\n";
        cout << "2. Track daily sales\n";
        cout << "3. Manage supplier expenses\n";
        cout << "4. Employee payments\n";
    }

    void showSpecialFeatures() override {
        system("cls");
        cout << "\n=== Shopkeeper Special Features ===\n";
        cout << "1. Inventory Management\n";
        cout << "2. Sales Tracker\n";
        cout << "3. Supplier Database\n";
        cout << "4. Profit Calculator\n";
        cout << "5. Back to main menu\n";

        char choice = getch();
        switch(choice) {
            case '1': { inventoryManagement(); break; }
            case '2': { salesTracker(); break; }
            case '3': { supplierDatabase(); break; }
            case '4': { profitCalculator(); break; }
            case '5': { return; }
            default: { showSpecialFeatures(); }
        }
    }

    void inventoryManagement() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("INVENTORY MANAGEMENT", screenWidth, boxY + 2, YELLOW);

        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Current Inventory Items:";

        setCursorPosition(boxX + 5, boxY + 6);
        setColor(GREEN);
        cout << "Item 1: ";
        setColor(WHITE);
        cout << "Produce - 25 units - RS1250.00";

        setCursorPosition(boxX + 5, boxY + 7);
        setColor(GREEN);
        cout << "Item 2: ";
        setColor(WHITE);
        cout << "Electronics - 10 units - RS5000.00";

        setCursorPosition(boxX + 5, boxY + 8);
        setColor(GREEN);
        cout << "Item 3: ";
        setColor(WHITE);
        cout << "Stationery - 100 units - RS500.00";

        centerText("Feature under development. More options coming soon!", screenWidth, boxY + 11, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }

    void salesTracker() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("SALES TRACKER", screenWidth, boxY + 2, YELLOW);

        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Today's Sales Summary:";

        // Draw sales progress bar
        setCursorPosition(boxX + 5, boxY + 6);
        cout << "Sales Target: ";
        setColor(YELLOW);
        cout << "RS10,000.00";

        setCursorPosition(boxX + 5, boxY + 7);
        cout << "Current Sales: ";
        setColor(GREEN);
        cout << "RS7,500.00";

        setCursorPosition(boxX + 5, boxY + 9);
        cout << "Progress: ";
        drawStaticProgressBar(75.0f, 30, boxX + 15, boxY + 9, GREEN);

        centerText("Feature under development. More options coming soon!", screenWidth, boxY + 11, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }

    void supplierDatabase() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("SUPPLIER DATABASE", screenWidth, boxY + 2, YELLOW);

        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Your Suppliers:";

        setCursorPosition(boxX + 5, boxY + 6);
        setColor(GREEN);
        cout << "1. ";
        setColor(WHITE);
        cout << "ABC Distributors - Electronics - Contact: 555-1234";

        setCursorPosition(boxX + 5, boxY + 7);
        setColor(GREEN);
        cout << "2. ";
        setColor(WHITE);
        cout << "Fresh Farms Inc. - Produce - Contact: 555-5678";

        setCursorPosition(boxX + 5, boxY + 8);
        setColor(GREEN);
        cout << "3. ";
        setColor(WHITE);
        cout << "Office Supplies Ltd. - Stationery - Contact: 555-9012";

        centerText("Feature under development. More options coming soon!", screenWidth, boxY + 11, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }

    void profitCalculator() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("PROFIT CALCULATOR", screenWidth, boxY + 2, YELLOW);

        // Sample calculation
        float revenue = 10000.0f;
        float expenses = 7500.0f;
        float profit = revenue - expenses;
        float profitMargin = (profit / revenue) * 100.0f;

        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Sample Calculation:";

        setCursorPosition(boxX + 5, boxY + 6);
        cout << "Revenue: ";
        setColor(GREEN);
        cout << "RS" << fixed << setprecision(2) << revenue;

        setCursorPosition(boxX + 5, boxY + 7);
        resetColor();
        cout << "Expenses: ";
        setColor(RED);
        cout << "RS" << fixed << setprecision(2) << expenses;

        setCursorPosition(boxX + 5, boxY + 8);
        resetColor();
        cout << "Profit: ";
        setColor(YELLOW);
        cout << "RS" << fixed << setprecision(2) << profit;

        setCursorPosition(boxX + 5, boxY + 9);
        resetColor();
        cout << "Profit Margin: ";
        setColor(GREEN);
        cout << fixed << setprecision(1) << profitMargin << "%";

        resetColor();
        centerText("Feature under development. More options coming soon!", screenWidth, boxY + 11, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }
};

class Accountant : public User {
public:
    Accountant(string uname) : User(uname, "Accountant") {}

    void displayDashboard() override {
        User::displayDashboard();
        cout << "Accountant Dashboard\n";
        cout << "1. Client financial records\n";
        cout << "2. Tax preparation\n";
        cout << "3. Financial analysis\n";
        cout << "4. Business expense tracking\n";
    }

    void showSpecialFeatures() override {
        system("cls");
        cout << "\n=== Accountant Special Features ===\n";
        cout << "1. Tax Calculator\n";
        cout << "2. Financial Reports\n";
        cout << "3. Client Database\n";
        cout << "4. Expense Categorization\n";
        cout << "5. Back to main menu\n";

        char choice = getch();
        switch(choice) {
            case '1': { taxCalculator(); break; }
            case '2': { financialReports(); break; }
            case '3': { clientDatabase(); break; }
            case '4': { expenseCategorization(); break; }
            case '5': { return; }
            default: { showSpecialFeatures(); }
        }
    }

    void taxCalculator() {
        system("cls");
        cout << "\n=== Tax Calculator ===\n";
        cout << "Calculate taxes for yourself or clients\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void financialReports() {
        system("cls");
        cout << "\n=== Financial Reports ===\n";
        cout << "Generate financial reports and analysis\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void clientDatabase() {
        system("cls");
        cout << "\n=== Client Database ===\n";
        cout << "Manage your client information and finances\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void expenseCategorization() {
        system("cls");
        cout << "\n=== Expense Categorization ===\n";
        cout << "Categorize expenses for better financial management\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }
};

class Labour : public User {
public:
    Labour(string uname) : User(uname, "Labour") {}

    void displayDashboard() override {
        User::displayDashboard();
        cout << "Labour Dashboard\n";
        cout << "1. Track work hours\n";
        cout << "2. Record material use\n";
        cout << "3. Tool maintenance\n";
        cout << "4. Expense claims\n";
    }

    void showSpecialFeatures() override {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, BLUE);
        centerText("LABOUR SPECIAL FEATURES", screenWidth, boxY + 2, YELLOW);

        setCursorPosition(boxX + 10, boxY + 5);
        setColor(GREEN); cout << "1. "; resetColor();
        cout << "Work Log";

        setCursorPosition(boxX + 10, boxY + 6);
        setColor(GREEN); cout << "2. "; resetColor();
        cout << "Tool Maintenance";

        setCursorPosition(boxX + 10, boxY + 7);
        setColor(GREEN); cout << "3. "; resetColor();
        cout << "Materials Usage";

        setCursorPosition(boxX + 10, boxY + 8);
        setColor(GREEN); cout << "4. "; resetColor();
        cout << "Expense Claims";

        setCursorPosition(boxX + 10, boxY + 9);
        setColor(RED); cout << "5. "; resetColor();
        cout << "Back to Main Menu";

        centerText("Select an option (1-5)", screenWidth, boxY + 12, WHITE);

        char choice = getch();
        switch(choice) {
            case '1': { workLog(); break; }
            case '2': { toolMaintenance(); break; }
            case '3': { materialsUsage(); break; }
            case '4': { expenseClaims(); break; }
            case '5': { return; }
            default: { showSpecialFeatures(); }
        }
    }

    void workLog() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("WORK LOG", screenWidth, boxY + 2, YELLOW);

        // Sample work log data
        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Recent Work Hours:";

        setCursorPosition(boxX + 5, boxY + 6);
        setColor(WHITE);
        cout << "May 1, 2025: ";
        setColor(GREEN);
        cout << "8 hours - Site A - Plumbing work";

        setCursorPosition(boxX + 5, boxY + 7);
        setColor(WHITE);
        cout << "May 2, 2025: ";
        setColor(GREEN);
        cout << "6 hours - Site B - Electrical work";

        setCursorPosition(boxX + 5, boxY + 8);
        setColor(WHITE);
        cout << "May 3, 2025: ";
        setColor(GREEN);
        cout << "7 hours - Site A - Finishing work";

        // Total hours this week
        setCursorPosition(boxX + 5, boxY + 10);
        setColor(CYAN);
        cout << "Total hours this week: ";
        setColor(YELLOW);
        cout << "21 hours";
        resetColor();

        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }

    void toolMaintenance() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("TOOL MAINTENANCE LOG", screenWidth, boxY + 2, YELLOW);

        // Sample tool maintenance data
        setCursorPosition(boxX + 5, boxY + 4);
        cout << "Tool Status:";

        setCursorPosition(boxX + 5, boxY + 6);
        setColor(GREEN);
        cout << "Power Drill: ";
        setColor(WHITE);
        cout << "Good condition - Last serviced: Apr 15, 2025";

        setCursorPosition(boxX + 5, boxY + 7);
        setColor(YELLOW);
        cout << "Circular Saw: ";
        setColor(WHITE);
        cout << "Needs blade replacement";

        setCursorPosition(boxX + 5, boxY + 8);
        setColor(RED);
        cout << "Generator: ";
        setColor(WHITE);
        cout << "Requires service - Last checked: Mar 10, 2025";

        centerText("Feature under development. More options coming soon!", screenWidth, boxY + 11, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 13, WHITE);

        getch();
        showSpecialFeatures();
    }

    void materialsUsage() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("MATERIALS USAGE", screenWidth, boxY + 2, YELLOW);

        centerText("Feature under development. Coming soon!", screenWidth, boxY + 8, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 10, WHITE);

        getch();
        showSpecialFeatures();
    }

    void expenseClaims() {
        system("cls");
        int screenWidth = 80;
        int boxWidth = 60;
        int boxHeight = 15;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = 3;

        drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
        centerText("EXPENSE CLAIMS", screenWidth, boxY + 2, YELLOW);

        centerText("Feature under development. Coming soon!", screenWidth, boxY + 8, CYAN);
        centerText("Press any key to return...", screenWidth, boxY + 10, WHITE);

        getch();
        showSpecialFeatures();
    }

    void workHourTracker() {
        system("cls");
        cout << "\n=== Work Hour Tracker ===\n";
        cout << "Track your work hours and calculate earnings\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void materialsInventory() {
        system("cls");
        cout << "\n=== Materials Inventory ===\n";
        cout << "Manage your construction materials and costs\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void projectBudgetPlanner() {
        system("cls");
        cout << "\n=== Project Budget Planner ===\n";
        cout << "Plan and track budgets for different projects\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }

    void toolMaintenanceLog() {
        system("cls");
        cout << "\n=== Tool Maintenance Log ===\n";
        cout << "Track tool maintenance and replacement costs\n";
        cout << "Feature under development.\n";
        cout << "\nPress any key to return...";
        getch();
        showSpecialFeatures();
    }
};

// Factory function to create appropriate user type
User* createUser(string username, string role) {
    if (role == "Housewife") return new Housewife(username);
    if (role == "Shopkeeper") return new Shopkeeper(username);
    if (role == "Accountant") return new Accountant(username);
    if (role == "Labour") return new Labour(username);
    return nullptr; // Should never happen
}

// Utility function to read total expenses from file
float readTotal(string filename) {
    float b, c = 0;
    ifstream fp(DATA_DIR + filename);
    while (fp >> b) c += b;
    return c;
}

// Basic expense category functions
float cloth() { return readTotal(username + "_cloth.txt"); }
float med()   { return readTotal(username + "_med.txt"); }
float trans() { return readTotal(username + "_trans.txt"); }
float food()  { return readTotal(username + "_food.txt"); }
float educ()  { return readTotal(username + "_educ.txt"); }
float liv()   { return readTotal(username + "_liv.txt"); }

// Extended expense category functions
float inventory() { return readTotal(username + "_inventory.txt"); }
float utilities() { return readTotal(username + "_utilities.txt"); }
float employees() { return readTotal(username + "_employees.txt"); }
float maintenance() { return readTotal(username + "_maintenance.txt"); }
float officesupplies() { return readTotal(username + "_officesupplies.txt"); }
float taxes() { return readTotal(username + "_taxes.txt"); }
float toolsequipment() { return readTotal(username + "_toolsequipment.txt"); }
float materials() { return readTotal(username + "_materials.txt"); }
float groceries() { return readTotal(username + "_groceries.txt"); }
float household() { return readTotal(username + "_household.txt"); }

// Transaction saving with improved formatting and categorization
void saveTransaction(string file, float a, string description = "") {
    // Create directory if it doesn't exist
    createDataDirectory();

    // Save transaction with timestamp, amount, and optional description
    ofstream data(DATA_DIR + username + "_" + file + "_log.txt", ios::app);
    data << dt << " | RS" << fixed << setprecision(2) << a;
    if (!description.empty()) {
        data << " | " << description;
    }
    data << "\n";

    // Save amount for total calculations
    ofstream fp(DATA_DIR + username + "_" + file + ".txt", ios::app);
    fp << a << " ";

    // Save to general transaction history
    ofstream history(DATA_DIR + username + "_transaction_history.txt", ios::app);
    history << dt << " | " << file << " | RS" << fixed << setprecision(2) << a;
    if (!description.empty()) {
        history << " | " << description;
    }
    history << "\n";
}

float Expense::ifood(float a) {
    system("CLS");
    cout << "\n=== FOOD EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("food", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal food expenses: RS" << fixed << setprecision(2) << food();
    back();
    return food();
}
float Expense::icloth(float a) {
    system("CLS");
    cout << "\n=== CLOTHING EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("cloth", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal clothing expenses: RS" << fixed << setprecision(2) << cloth();
    back();
    return cloth();
}
float Expense::iliving(float a) {
    system("CLS");
    cout << "\n=== LIVING EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("liv", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal living expenses: RS" << fixed << setprecision(2) << liv();
    back();
    return liv();
}
float Expense::itrans(float a) {
    system("CLS");
    cout << "\n=== TRANSPORTATION EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("trans", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal transportation expenses: RS" << fixed << setprecision(2) << trans();
    back();
    return trans();
}
float Expense::ieduc(float a) {
    system("CLS");
    cout << "\n=== EDUCATION EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("educ", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal education expenses: RS" << fixed << setprecision(2) << educ();
    back();
    return educ();
}
float Expense::imed(float a) {
    system("CLS");
    cout << "\n=== MEDICAL EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("med", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal medical expenses: RS" << fixed << setprecision(2) << med();
    back();
    return med();
}

// New expense category methods
float Expense::iinventory(float a) {
    system("CLS");
    cout << "\n=== INVENTORY EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("inventory", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal inventory expenses: RS" << fixed << setprecision(2) << inventory();
    back();
    return inventory();
}

float Expense::iutilities(float a) {
    system("CLS");
    cout << "\n=== UTILITIES EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("utilities", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal utilities expenses: RS" << fixed << setprecision(2) << utilities();
    back();
    return utilities();
}

float Expense::iemployees(float a) {
    system("CLS");
    cout << "\n=== EMPLOYEE EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("employees", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal employee expenses: RS" << fixed << setprecision(2) << employees();
    back();
    return employees();
}

float Expense::imaintenance(float a) {
    system("CLS");
    cout << "\n=== MAINTENANCE EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("maintenance", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal maintenance expenses: RS" << fixed << setprecision(2) << maintenance();
    back();
    return maintenance();
}

float Expense::iofficesupplies(float a) {
    system("CLS");
    cout << "\n=== OFFICE SUPPLIES EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("officesupplies", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal office supplies expenses: RS" << fixed << setprecision(2) << officesupplies();
    back();
    return officesupplies();
}

float Expense::itaxes(float a) {
    system("CLS");
    cout << "\n=== TAX EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("taxes", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal tax expenses: RS" << fixed << setprecision(2) << taxes();
    back();
    return taxes();
}

float Expense::itoolsequipment(float a) {
    system("CLS");
    cout << "\n=== TOOLS & EQUIPMENT EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("toolsequipment", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal tools & equipment expenses: RS" << fixed << setprecision(2) << toolsequipment();
    back();
    return toolsequipment();
}

float Expense::imaterials(float a) {
    system("CLS");
    cout << "\n=== MATERIALS EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("materials", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal materials expenses: RS" << fixed << setprecision(2) << materials();
    back();
    return materials();
}

float Expense::igroceries(float a) {
    system("CLS");
    cout << "\n=== GROCERIES EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("groceries", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal groceries expenses: RS" << fixed << setprecision(2) << groceries();
    back();
    return groceries();
}

float Expense::ihousehold(float a) {
    system("CLS");
    cout << "\n=== HOUSEHOLD EXPENSES ===\n";
    string description;
    cout << "Enter description (optional): ";
    cin.ignore(1, '\n');
    getline(cin, description);

    saveTransaction("household", a, description);

    cout << "\nTransaction saved successfully!";
    cout << "\nTotal household expenses: RS" << fixed << setprecision(2) << household();
    back();
    return household();
}

// Get categories relevant to user role
vector<string> Expense::getCategoriesForRole(string role) {
    vector<string> categories;

    // Common categories for all roles
    categories.push_back("Food");
    categories.push_back("Clothing");
    categories.push_back("Living");
    categories.push_back("Transportation");
    categories.push_back("Education");
    categories.push_back("Medical");
    categories.push_back("Utilities");

    // Role-specific categories
    if (role == "Housewife") {
        categories.push_back("Groceries");
        categories.push_back("Household");
    }
    else if (role == "Shopkeeper") {
        categories.push_back("Inventory");
        categories.push_back("Employees");
        categories.push_back("Maintenance");
    }
    else if (role == "Accountant") {
        categories.push_back("Office Supplies");
        categories.push_back("Taxes");
        categories.push_back("Employees");
    }
    else if (role == "Labour") {
        categories.push_back("Tools & Equipment");
        categories.push_back("Materials");
    }

    return categories;
}

void intro() {
    system("CLS");
    cout << dt << endl;
    cout << "\n===== EXPENSE TRACKER =====\n";
    cout << "\nWelcome, " << username << " (" << role << ")";
    cout << "\nMoney left: RS" << fixed << setprecision(2) << lmoney << endl;

    vector<string> categories = e.getCategoriesForRole(role);

    label:
    cout << "\nSelect expense category:\n";
    for (int i = 0; i < categories.size(); i++) {
        cout << (i+1) << ".) " << categories[i] << "\n";
    }
    cout << "Press 'b' to go back\n";

    char ch = getch();
    if (ch == 'b' || ch == 'B') {
        next();
        return;
    }

    int choice = ch - '0';
    if (choice > 0 && choice <= categories.size()) {
        do {
            cout << "\nEnter amount: RS";
            cin >> spend;
        } while (spend > lmoney || spend < 0);

        lmoney -= spend;

        string category = categories[choice-1];
        // Call appropriate expense function based on category
        if (category == "Food") { e.ifood(spend); }
        else if (category == "Clothing") { e.icloth(spend); }
        else if (category == "Living") { e.iliving(spend); }
        else if (category == "Transportation") { e.itrans(spend); }
        else if (category == "Education") { e.ieduc(spend); }
        else if (category == "Medical") { e.imed(spend); }
        else if (category == "Utilities") { e.iutilities(spend); }
        else if (category == "Groceries") { e.igroceries(spend); }
        else if (category == "Household") { e.ihousehold(spend); }
        else if (category == "Inventory") { e.iinventory(spend); }
        else if (category == "Employees") { e.iemployees(spend); }
        else if (category == "Maintenance") { e.imaintenance(spend); }
        else if (category == "Office Supplies") { e.iofficesupplies(spend); }
        else if (category == "Taxes") { e.itaxes(spend); }
        else if (category == "Tools & Equipment") { e.itoolsequipment(spend); }
        else if (category == "Materials") { e.imaterials(spend); }
    }
    else {
        system("cls");
        cout << "Invalid selection, please try again.\n";
        goto label;
    }
}

void displayTransactionHistory() {
    system("cls");
    cout << "\n===== TRANSACTION HISTORY =====\n";

    ifstream history(DATA_DIR + username + "_transaction_history.txt");
    if (!history) {
        cout << "\nNo transaction history available.\n";
    } else {
        string line;
        int count = 0;
        while (getline(history, line) && count < 20) {
            cout << line << endl;
            count++;
        }

        if (count == 20) {
            cout << "\n(Showing last 20 transactions. Full history in file.)\n";
        }
    }

    cout << "\nPress any key to return to main menu...";
    getch();
    next();
}

void review() {
    system("cls");
    cout << "\n===== EXPENSE SUMMARY =====\n";
    cout << "User: " << username << " (" << role << ")\n";
    cout << "Current budget: RS" << fixed << setprecision(2) << lmoney << "\n\n";

    vector<string> categories = e.getCategoriesForRole(role);
    vector<pair<string, float>> expenses;
    float total = 0;

    // Calculate total for each category
    for (string& category : categories) {
        float amount = 0;
        if (category == "Food") amount = food();
        else if (category == "Clothing") amount = cloth();
        else if (category == "Living") amount = liv();
        else if (category == "Transportation") amount = trans();
        else if (category == "Education") amount = educ();
        else if (category == "Medical") amount = med();
        else if (category == "Utilities") amount = utilities();
        else if (category == "Groceries") amount = groceries();
        else if (category == "Household") amount = household();
        else if (category == "Inventory") amount = inventory();
        else if (category == "Employees") amount = employees();
        else if (category == "Maintenance") amount = maintenance();
        else if (category == "Office Supplies") amount = officesupplies();
        else if (category == "Taxes") amount = taxes();
        else if (category == "Tools & Equipment") amount = toolsequipment();
        else if (category == "Materials") amount = materials();

        expenses.push_back(make_pair(category, amount));
        total += amount;
    }

    // Sort expenses in descending order
    sort(expenses.begin(), expenses.end(),
         [](const pair<string, float>& a, const pair<string, float>& b) {
             return a.second > b.second;
         });

    // Display table header
    cout << left << setw(20) << "CATEGORY" << right << setw(15) << "AMOUNT" << setw(10) << "% OF TOTAL" << endl;
    cout << string(45, '-') << endl;

    // Display table rows
    for (auto& expense : expenses) {
        if (expense.second > 0) {
            float percentage = (total > 0) ? (expense.second / total * 100) : 0;
            cout << left << setw(20) << expense.first
                 << right << setw(15) << "RS" + to_string(expense.second).substr(0, to_string(expense.second).find(".") + 3)
                 << setw(9) << fixed << setprecision(1) << percentage << "%" << endl;
        }
    }

    cout << string(45, '-') << endl;
    cout << left << setw(20) << "TOTAL EXPENSES"
         << right << setw(15) << "RS" + to_string(total).substr(0, to_string(total).find(".") + 3) << endl;
    cout << left << setw(20) << "REMAINING BUDGET"
         << right << setw(15) << "RS" + to_string(lmoney).substr(0, to_string(lmoney).find(".") + 3) << endl;

    cout << "\nOptions:\n";
    cout << "1. View detailed transaction history\n";
    cout << "2. Return to main menu\n";

    char ch = getch();
    if (ch == '1') {
        displayTransactionHistory();
    } else {
        next();
    }
}

float income() {
    system("cls");
    cout << "\n===== BUDGET MANAGEMENT =====\n";
    cout << "Current budget: RS" << fixed << setprecision(2) << lmoney << "\n\n";

    label:
    cout << "1.) Subtract money from budget\n";
    cout << "2.) Add money to budget\n";
    cout << "3.) Set budget alarm\n";
    cout << "4.) Back to main menu\n";

    char ch = getch();
    float a;

    switch (ch) {
    case '1': {
        do {
            cout << "\nEnter amount to subtract: RS";
            cin >> a;
            if (a < 0) {
                cout << "Please enter a positive amount.\n";
            } else if (a > lmoney) {
                cout << "Amount exceeds current budget.\n";
            }
        } while (a < 0 || a > lmoney);

        lmoney -= a;

        // Log the budget reduction as a transaction
        string description;
        cout << "Enter reason for budget reduction (optional): ";
        cin.ignore(1, '\n');
        getline(cin, description);
        saveTransaction("budget_adjustment", -a, description);
        break;
    }

    case '2': {
        do {
            cout << "\nEnter amount to add: RS";
            cin >> a;
            if (a < 0) {
                cout << "Please enter a positive amount.\n";
            }
        } while (a < 0);

        lmoney += a;

        // Log the budget increase as a transaction
        string description;
        cout << "Enter source of additional funds (optional): ";
        cin.ignore(1, '\n');
        getline(cin, description);
        saveTransaction("budget_adjustment", a, description);
        break;
    }

    case '3': {
        float threshold;
        cout << "\nSet budget alarm when balance falls below: RS";
        cin >> threshold;

        if (threshold > 0 && threshold <= lmoney) {
            ofstream alarm(DATA_DIR + username + "_budget_alarm.txt");
            alarm << threshold;
            alarm.close();
            cout << "\nBudget alarm set to RS" << fixed << setprecision(2) << threshold;
        } else {
            cout << "\nInvalid threshold amount.";
        }
        Sleep(1500);
        income();
        return lmoney;
    }

    case '4':
        next();
        return lmoney;

    default:
        system("cls");
        cout << "Invalid selection. Please try again.\n";
        goto label;
    }

    cout << "\nBudget updated successfully!";
    cout << "\nNew budget: RS" << fixed << setprecision(2) << lmoney;

    // Save updated budget to file
    ofstream fp(DATA_DIR + username + "_budget.txt");
    fp << lmoney;
    fp.close();

    Sleep(1500);
    next();
    return lmoney;
}

void newf() {
    system("cls");
    cout << "\n===== INITIAL BUDGET SETUP =====\n";
    cout << "Welcome to your new budget tracker, " << username << "!\n\n";

    // Create data directory if it doesn't exist
    createDataDirectory();

    float initial_budget;
    do {
        cout << "Enter your initial budget amount: RS";
        cin >> initial_budget;

        if (initial_budget <= 0) {
            cout << "Please enter a positive budget amount.\n";
        }
    } while (initial_budget <= 0);

    lmoney = initial_budget;

    // Save budget to file
    ofstream fp(DATA_DIR + username + "_budget.txt");
    fp << lmoney;
    fp.close();

    cout << "\nBudget of RS" << fixed << setprecision(2) << lmoney << " has been set.\n";
    cout << "\nPress any key to continue to your dashboard...";
    getch();
    next();
}

// ---------- AUTH SYSTEM -------------
bool authenticate() {
    system("cls");
    cout << "\n===== AUTHENTICATION =====\n";

    cout << "Enter username: ";
    cin >> username;

    string pass, stored_pass, stored_role;
    cout << "Enter password: ";
    char ch;
    while ((ch = _getch()) != '\r') {
        pass += ch;
        cout << '*';
    }
    cout << endl;

    // Create the data directory if it doesn't exist
    createDataDirectory();

    // Check if user file exists
    ifstream userfile(DATA_DIR + "users.dat");
    if (!userfile) {
        cout << "\nNo registered users found.\n";
        return false;
    }

    string uname;
    bool userFound = false;
    int failedAttempts = 0;

    while (userfile >> uname >> stored_pass >> stored_role) {
        if (uname == username) {
            userFound = true;
            if (stored_pass == pass) {
                role = stored_role;
                cout << "\nLogin successful! Welcome, " << username << "!\n";
                Sleep(1500);

                // Check if budget file exists and load budget
                ifstream budgetFile(DATA_DIR + username + "_budget.txt");
                if (budgetFile) {
                    budgetFile >> lmoney;
                    budgetFile.close();
                }

                // Check for budget alarm
                ifstream alarmFile(DATA_DIR + username + "_budget_alarm.txt");
                if (alarmFile) {
                    float threshold;
                    alarmFile >> threshold;
                    alarmFile.close();

                    if (lmoney <= threshold) {
                        cout << "\n⚠ BUDGET ALERT: Your budget ($" << fixed << setprecision(2) << lmoney
                             << ") is below your alarm threshold ($" << threshold << ")\n";
                        cout << "Press any key to continue...";
                        getch();
                    }
                }

                // Log login activity
                ofstream logFile(DATA_DIR + username + "_activity_log.txt", ios::app);
                logFile << dt << " | Login successful\n";
                logFile.close();

                return true;
            } else {
                failedAttempts++;
                if (failedAttempts >= 3) {
                    cout << "\nToo many failed login attempts.\n";
                    Sleep(1500);
                    return false;
                }
                cout << "\nInvalid password. Please try again (" << (3-failedAttempts) << " attempts remaining): ";
                pass = "";
                while ((ch = _getch()) != '\r') {
                    pass += ch;
                    cout << '*';
                }
                cout << endl;
                userfile.seekg(0); // Reset to start of file
            }
        }
    }

    if (!userFound) {
        cout << "\nUsername not found.\n";
        Sleep(1500);
    }

    return false;
}

void registerUser() {
    system("cls");
    cout << "\n===== NEW USER REGISTRATION =====\n\n";

    string newUsername, pass, confirm;
    bool usernameAvailable = false;

    // Create the data directory
    createDataDirectory();

    while (!usernameAvailable) {
        cout << "Enter username: ";
        cin >> newUsername;

        // Check if username already exists
        ifstream checkUser(DATA_DIR + "users.dat");
        string existingUser, _;  // _ is a placeholder
        bool userExists = false;

        if (checkUser) {
            while (checkUser >> existingUser >> _ >> _) {
                if (existingUser == newUsername) {
                    userExists = true;
                    break;
                }
            }
        }

        if (userExists) {
            cout << "Username already exists. Please choose another.\n";
        } else {
            usernameAvailable = true;
            username = newUsername;
        }
    }

    // Get and confirm password
    bool passwordMatch = false;
    while (!passwordMatch) {
        cout << "Enter password: ";
        pass = "";
        char ch;
        while ((ch = _getch()) != '\r') {
            pass += ch;
            cout << '*';
        }
        cout << endl;

        cout << "Confirm password: ";
        confirm = "";
        while ((ch = _getch()) != '\r') {
            confirm += ch;
            cout << '*';
        }
        cout << endl;

        if (pass != confirm) {
            cout << "Passwords do not match. Please try again.\n";
        } else {
            passwordMatch = true;
        }
    }

    // Select role
    cout << "\nSelect your role:\n";
    cout << "1) Housewife - For household budget management\n";
    cout << "2) Shopkeeper - For store and inventory management\n";
    cout << "3) Accountant - For financial and client management\n";
    cout << "4) Labour - For project and work expense tracking\n";

    int choice;
    do {
        cout << "Enter choice (1-4): ";
        cin >> choice;
        if (choice < 1 || choice > 4) {
            cout << "Invalid choice. Please select 1-4.\n";
        }
    } while (choice < 1 || choice > 4);

    string roles[] = { "Housewife", "Shopkeeper", "Accountant", "Labour" };
    role = roles[choice - 1];

    // Save user information
    ofstream userfile(DATA_DIR + "users.dat", ios::app);
    userfile << username << " " << pass << " " << role << endl;
    userfile.close();

    // Log registration activity
    ofstream logFile(DATA_DIR + username + "_activity_log.txt", ios::app);
    logFile << dt << " | User registered as " << role << "\n";
    logFile.close();

    cout << "\nRegistration successful!\n";
    cout << "Welcome, " << username << "! You are registered as a " << role << ".\n";
    Sleep(1500);

    // Initialize budget for new user
    newf();
}
void viewActivityLog() {
    system("cls");
    cout << "\n===== ACTIVITY LOG =====\n";

    ifstream logFile(DATA_DIR + username + "_activity_log.txt");
    if (!logFile) {
        cout << "\nNo activity log found.\n";
    } else {
        string line;
        int count = 0;
        while (getline(logFile, line) && count < 20) {
            cout << line << endl;
            count++;
        }

        if (count == 20) {
            cout << "\n(Showing most recent 20 activities. Full history in file.)\n";
        }
    }

    cout << "\nPress any key to return...";
    getch();
}

void backup() {
    system("cls");
    int screenWidth = 80;
    int boxWidth = 60;
    int boxHeight = 15;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = 3;

    drawBox(boxWidth, boxHeight, boxX, boxY, BLUE);
    centerText("BACKUP & RESTORE", screenWidth, boxY + 2, YELLOW);

    setCursorPosition(boxX + 10, boxY + 5);
    setColor(GREEN); cout << "1. "; resetColor();
    cout << "Create backup";

    setCursorPosition(boxX + 10, boxY + 6);
    setColor(GREEN); cout << "2. "; resetColor();
    cout << "Restore from backup";

    setCursorPosition(boxX + 10, boxY + 7);
    setColor(RED); cout << "3. "; resetColor();
    cout << "Back to main menu";

    centerText("Select an option (1-3)", screenWidth, boxY + 10, WHITE);

    char choice = getch();
    switch(choice) {
        case '1': {
            // Show progress animation
            system("cls");
            drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
            centerText("CREATING BACKUP", screenWidth, boxY + 2, YELLOW);
            centerText("Please wait while we back up your data...", screenWidth, boxY + 4, WHITE);

            // Draw progress bar
            drawProgressBar(boxX + 10, boxY + 6, 40, 1500, GREEN);

            // Execute backup command
            string backupCmd = "mkdir " + DATA_DIR + "backup_" + username + " 2>nul && copy " +
                               DATA_DIR + username + "*. " + DATA_DIR + "backup_" + username + " /Y >nul";
            system(backupCmd.c_str());

            // Show success message
            setCursorPosition(boxX + 5, boxY + 8);
            setColor(GREEN);
            cout << "Backup created successfully in " << DATA_DIR << "backup_" << username;
            resetColor();

            centerText("Press any key to return...", screenWidth, boxY + 10, WHITE);
            getch();
            break;
        }

        case '2': {
            // Show progress animation
            system("cls");
            drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
            centerText("RESTORING FROM BACKUP", screenWidth, boxY + 2, YELLOW);
            centerText("Please wait while we restore your data...", screenWidth, boxY + 4, WHITE);

            // Draw progress bar
            drawProgressBar(boxX + 10, boxY + 6, 40, 1500, YELLOW);

            // Execute restore command
            string restoreCmd = "copy " + DATA_DIR + "backup_" + username + "\\*. " + DATA_DIR + " /Y >nul";
            system(restoreCmd.c_str());

            // Show success message
            setCursorPosition(boxX + 5, boxY + 8);
            setColor(GREEN);
            cout << "Data restored successfully from backup";
            resetColor();

            centerText("Press any key to return...", screenWidth, boxY + 10, WHITE);
            getch();
            break;
        }

        case '3':
            return;

        default: {
            setCursorPosition(boxX + 10, boxY + 12);
            setColor(RED);
            cout << "Invalid choice. Press any key to try again.";
            resetColor();
            getch();
            backup(); // Recursively call backup to try again
        }
    }
}

// Enhanced next() function with modern UI
void next() {
    User* currentUser = nullptr;

    if (role == "Housewife") {
        currentUser = new Housewife(username);
    }
    else if (role == "Shopkeeper") {
        currentUser = new Shopkeeper(username);
    }
    else if (role == "Accountant") {
        currentUser = new Accountant(username);
    }
    else if (role == "Labour") {
        currentUser = new Labour(username);
    }

    while(true) {
        system("cls");

        // Draw dashboard container
        int screenWidth = 80;
        int dashWidth = 70;
        int dashHeight = 22;
        int dashX = (screenWidth - dashWidth) / 2;
        int dashY = 2;

        drawBox(dashWidth, dashHeight, dashX, dashY, BLUE);

        // Draw header with current date and time
        setColor(CYAN, DARK_BLUE);
        setCursorPosition(dashX, dashY - 1);
        cout << " BUDGET TRACKER - DASHBOARD ";
        setColor(WHITE);
        setCursorPosition(dashX + dashWidth - 25, dashY - 1);
        cout << dt;
        resetColor();

        // User info section
        int userBoxWidth = 25;
        int userBoxHeight = 6;
        int userBoxX = dashX + 2;
        int userBoxY = dashY + 2;

        drawBox(userBoxWidth, userBoxHeight, userBoxX, userBoxY, CYAN);

        setCursorPosition(userBoxX + 2, userBoxY + 1);
        setColor(YELLOW);
        cout << "User Profile";
        resetColor();

        setCursorPosition(userBoxX + 2, userBoxY + 3);
        cout << "Username: ";
        setColor(WHITE);
        cout << username;

        setCursorPosition(userBoxX + 2, userBoxY + 4);
        resetColor();
        cout << "Role: ";
        setColor(GREEN);
        cout << role;
        resetColor();

        // Budget status section
        int budgetBoxWidth = 25;
        int budgetBoxHeight = 6;
        int budgetBoxX = dashX + dashWidth - budgetBoxWidth - 2;
        int budgetBoxY = dashY + 2;

        drawBox(budgetBoxWidth, budgetBoxHeight, budgetBoxX, budgetBoxY, CYAN);

        setCursorPosition(budgetBoxX + 2, budgetBoxY + 1);
        setColor(YELLOW);
        cout << "Budget Status";
        resetColor();

        setCursorPosition(budgetBoxX + 2, budgetBoxY + 3);
        cout << "Current: ";

        // Set color based on budget amount
        int budgetColor = GREEN;
        if (lmoney < 1000) {
            budgetColor = RED;
        } else if (lmoney < 5000) {
            budgetColor = YELLOW;
        }

        setColor(budgetColor);
        cout << "RS" << fixed << setprecision(2) << lmoney;
        resetColor();

        // Draw visual budget indicator
        float percentage = min(100.0f, (lmoney / 10000.0f) * 100.0f); // Assuming 10000 is max budget
        int barColor = (percentage < 30.0f) ? RED : (percentage < 60.0f) ? YELLOW : GREEN;
        setCursorPosition(budgetBoxX + 2, budgetBoxY + 4);
        cout << "Status: ";
        drawStaticProgressBar(percentage, 15, budgetBoxX + 10, budgetBoxY + 4, barColor);

        // Menu options
        int menuStartY = dashY + 10;

        centerText("MAIN MENU", screenWidth, menuStartY, YELLOW);

        // Draw menu in a two column layout
        int col1X = dashX + 5;
        int col2X = dashX + dashWidth/2 + 5;
        int menuStartRow = menuStartY + 2;

        // Column 1 options
        setCursorPosition(col1X, menuStartRow);
        setColor(GREEN); cout << "1. "; resetColor();
        cout << "View Expense Summary";

        setCursorPosition(col1X, menuStartRow + 1);
        setColor(GREEN); cout << "2. "; resetColor();
        cout << "Add New Expense";

        setCursorPosition(col1X, menuStartRow + 2);
        setColor(GREEN); cout << "3. "; resetColor();
        cout << "Manage Budget";

        setCursorPosition(col1X, menuStartRow + 3);
        setColor(GREEN); cout << "4. "; resetColor();
        cout << "Special Features";

        // Column 2 options
        setCursorPosition(col2X, menuStartRow);
        setColor(GREEN); cout << "5. "; resetColor();
        cout << "Transaction History";

        setCursorPosition(col2X, menuStartRow + 1);
        setColor(GREEN); cout << "6. "; resetColor();
        cout << "Activity Log";

        setCursorPosition(col2X, menuStartRow + 2);
        setColor(GREEN); cout << "7. "; resetColor();
        cout << "Backup & Restore";

        setCursorPosition(col2X, menuStartRow + 3);
        setColor(RED); cout << "8. "; resetColor();
        cout << "Logout";

        // Footer
        centerText("Select an option using number keys (1-8)", screenWidth, dashY + dashHeight - 2, WHITE);

        // Get user selection
        char ch = getch();

        // Log user activity
        ofstream logFile(DATA_DIR + username + "_activity_log.txt", ios::app);

        switch (ch) {
            case '1': {
                logFile << dt << " | Viewed expense summary\n";
                logFile.close();
                review();
                break;
            }

            case '2': {
                logFile << dt << " | Added new expense\n";
                logFile.close();
                intro();
                break;
            }

            case '3': {
                logFile << dt << " | Modified budget\n";
                logFile.close();
                income();
                break;
            }

            case '4': {
                logFile << dt << " | Accessed role-specific features\n";
                logFile.close();
                currentUser->showSpecialFeatures();
                break;
            }

            case '5': {
                logFile << dt << " | Viewed transaction history\n";
                logFile.close();
                displayTransactionHistory();
                break;
            }

            case '6': {
                logFile << dt << " | Viewed activity log\n";
                logFile.close();
                viewActivityLog();
                break;
            }

            case '7': {
                logFile << dt << " | Used backup/restore feature\n";
                logFile.close();
                backup();
                break;
            }

            case '8': {
                logFile << dt << " | Logged out\n";
                logFile.close();

                // Animated logout
                int boxWidth = 40;
                int boxHeight = 5;
                int boxX = (screenWidth - boxWidth) / 2;
                int boxY = 10;

                system("cls");
                drawBox(boxWidth, boxHeight, boxX, boxY, CYAN);
                centerText("Logging out...", screenWidth, boxY + 1, YELLOW);
                centerText("Thank you for using Budget Tracker!", screenWidth, boxY + 2, WHITE);
                drawProgressBar(boxX + 5, boxY + 3, 30, 1000, RED);

                delete currentUser; // Clean up user object
                return; // Return to main function/login screen
            }

            default: {
                logFile.close();
                setCursorPosition(dashX + 5, dashY + dashHeight - 3);
                setColor(RED);
                cout << "Invalid choice. Try again.";
                resetColor();
                Sleep(1000);
            }
        }

        // Update user's budget from global variable
        currentUser->updateBudget(lmoney);
    }
}

// -------- MAIN -------------
int main() {
    // Remove newline from datetime string
    dt[--l] = ' ';

    // Set console window and buffer size for better display
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT windowSize = {0, 0, 79, 29}; // Width: 80, Height: 30
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

    // Display title with enhanced visuals
    system("cls");

    // Draw welcome box
    int boxWidth = 50;
    int boxHeight = 10;
    int startX = (80 - boxWidth) / 2;
    int startY = 5;

    drawBox(boxWidth, boxHeight, startX, startY, CYAN);

    // Display title with animation and color
    centerText("BUDGET TRACKER", 80, startY + 2, YELLOW);
    centerText("ROLE-BASED SYSTEM", 80, startY + 3, WHITE);

    // Show version info
    centerText("Developed by: GROUP 2", 80, startY + 5, GREEN);
    centerText("Version:OOPS 2.0", 80, startY + 6, GREEN);

    // Draw loading bar
    centerText("Loading application...", 80, startY + 8, CYAN);
    drawProgressBar((80 - 40) / 2, startY + 9, 40, 1500, YELLOW);

    // Wait for user input
    centerText("Press any key to continue...", 80, startY + 11, WHITE);
    getch();

    // Create data directory if it doesn't exist
    createDataDirectory();

    while (true) {
        system("cls");

        // Draw main menu box
        int menuWidth = 60;
        int menuHeight = 15;
        int menuX = (80 - menuWidth) / 2;
        int menuY = 5;

        drawBox(menuWidth, menuHeight, menuX, menuY, BLUE);

        // Display title and current date
        centerText("BUDGET TRACKER", 80, menuY + 2, YELLOW);
        setColor(CYAN);
        setCursorPosition(menuX + 3, menuY + 4);
        cout << "Date: " << dt;
        resetColor();

        // Display menu options with colors
        setCursorPosition(menuX + 15, menuY + 6);
        setColor(WHITE);
        cout << "Select an option:";
        resetColor();

        setCursorPosition(menuX + 15, menuY + 8);
        setColor(GREEN);
        cout << "1. ";
        setColor(WHITE);
        cout << "Login to your account";

        setCursorPosition(menuX + 15, menuY + 9);
        setColor(CYAN);
        cout << "2. ";
        setColor(WHITE);
        cout << "Create a new account";

        setCursorPosition(menuX + 15, menuY + 10);
        setColor(RED);
        cout << "3. ";
        setColor(WHITE);
        cout << "Exit Program";
        resetColor();

        // Draw a decorative footer
        centerText("© 2025 Budget Tracker - Version 2.0", 80, menuY + 13, DARK_CYAN);

        char ch = getch();

        if (ch == '3') {
            // Show exit animation
            system("cls");
            drawBox(40, 5, 20, 10, CYAN);
            centerText("Thank you for using Budget Tracker!", 80, 12, YELLOW);
            centerText("Exiting program...", 80, 13, WHITE);
            drawProgressBar(20, 14, 40, 1000, RED);
            return 0;
        }
        else if (ch == '2') {
            registerUser();
            next(); // Go to dashboard after registration
        }
        else if (ch == '1') {
            bool success = authenticate();
            if (success) {
                // Show success animation
                system("cls");
                drawBox(40, 5, 20, 10, GREEN);
                centerText("Login successful!", 80, 12, GREEN);
                centerText("Loading your dashboard...", 80, 13, WHITE);
                drawProgressBar(20, 14, 40, 1000, GREEN);
                next(); // Go to dashboard after successful login
            } else {
                // Show error message
                system("cls");
                drawBox(40, 5, 20, 10, RED);
                centerText("Login failed!", 80, 12, RED);
                centerText("Press any key to return to main menu...", 80, 13, WHITE);
                getch();
                // Continue to show login screen
            }
        }
        else {
            // Show error message
            setCursorPosition(menuX + 15, menuY + 11);
            setColor(RED);
            cout << "Invalid input. Please try again.";
            resetColor();
            Sleep(1000);
        }
    }

    return 0;
}
