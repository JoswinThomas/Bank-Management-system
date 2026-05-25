/*
 * ============================================================
 *   BANK OPERATIONS SYSTEM
 *   Developed in C++ -- Debit, Credit & Account Management
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;

// --- Utility: get current timestamp ---
string getCurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// --- Transaction Record ---
struct Transaction {
    string type;
    double amount;
    double balanceAfter;
    string timestamp;
    string description;
};

// --- Account Class ---
class Account {
private:
    string accountNumber;
    string holderName;
    double balance;
    string accountType;
    bool isActive;
    vector<Transaction> history;
    static int accountCounter;

public:
    Account(const string& name, double initialDeposit, const string& type = "SAVINGS")
        : holderName(name), balance(initialDeposit), accountType(type), isActive(true) {
        accountCounter++;
        ostringstream ss;
        ss << "ACC" << setw(5) << setfill('0') << accountCounter;
        accountNumber = ss.str();

        Transaction t;
        t.type         = "CREDIT";
        t.amount       = initialDeposit;
        t.balanceAfter = balance;
        t.timestamp    = getCurrentTimestamp();
        t.description  = "Account opened - initial deposit";
        history.push_back(t);
    }

    bool credit(double amount, const string& desc = "Deposit") {
        if (!isActive) {
            cout << "  [ERROR] Account is inactive.\n";
            return false;
        }
        if (amount <= 0) {
            cout << "  [ERROR] Credit amount must be positive.\n";
            return false;
        }
        balance += amount;
        Transaction t;
        t.type         = "CREDIT";
        t.amount       = amount;
        t.balanceAfter = balance;
        t.timestamp    = getCurrentTimestamp();
        t.description  = desc;
        history.push_back(t);
        cout << "  [SUCCESS] Credited Rs. " << fixed << setprecision(2) << amount
             << " -- New Balance: Rs. " << balance << "\n";
        return true;
    }

    bool debit(double amount, const string& desc = "Withdrawal") {
        if (!isActive) {
            cout << "  [ERROR] Account is inactive.\n";
            return false;
        }
        if (amount <= 0) {
            cout << "  [ERROR] Debit amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            cout << "  [ERROR] Insufficient funds. Available: Rs. "
                 << fixed << setprecision(2) << balance << "\n";
            return false;
        }
        balance -= amount;
        Transaction t;
        t.type         = "DEBIT";
        t.amount       = amount;
        t.balanceAfter = balance;
        t.timestamp    = getCurrentTimestamp();
        t.description  = desc;
        history.push_back(t);
        cout << "  [SUCCESS] Debited Rs. " << fixed << setprecision(2) << amount
             << " -- New Balance: Rs. " << balance << "\n";
        return true;
    }

    bool transfer(Account& target, double amount) {
        cout << "  Initiating transfer of Rs. " << fixed << setprecision(2) << amount
             << " from " << accountNumber << " to " << target.getAccountNumber() << "\n";
        if (debit(amount, "Transfer to " + target.getAccountNumber())) {
            target.credit(amount, "Transfer from " + accountNumber);
            return true;
        }
        return false;
    }

    void closeAccount() {
        isActive = false;
        cout << "  Account " << accountNumber << " has been closed.\n";
    }

    void displayDetails() const {
        cout << "\n";
        cout << "  =========================================\n";
        cout << "           ACCOUNT DETAILS\n";
        cout << "  =========================================\n";
        cout << "  Account No : " << accountNumber << "\n";
        cout << "  Holder     : " << holderName    << "\n";
        cout << "  Type       : " << accountType   << "\n";
        cout << "  Balance    : Rs. " << fixed << setprecision(2) << balance << "\n";
        cout << "  Status     : " << (isActive ? "ACTIVE" : "INACTIVE") << "\n";
        cout << "  =========================================\n";
    }

    void printStatement(int last = 5) const {
        cout << "\n";
        cout << "  Statement for " << accountNumber << " (" << holderName << ")\n";
        cout << "  " << string(70, '-') << "\n";
        cout << "  " << left << setw(22) << "Date/Time"
             << setw(10) << "Type"
             << setw(14) << "Amount (Rs.)"
             << setw(14) << "Balance (Rs.)"
             << "Description\n";
        cout << "  " << string(70, '-') << "\n";

        int start = max(0, (int)history.size() - last);
        for (int i = start; i < (int)history.size(); ++i) {
            const Transaction& t = history[i];
            cout << "  " << left << setw(22) << t.timestamp
                 << setw(10) << t.type
                 << setw(14) << fixed << setprecision(2) << t.amount
                 << setw(14) << t.balanceAfter
                 << t.description << "\n";
        }
        cout << "  " << string(70, '-') << "\n";
        cout << "  Total transactions: " << history.size() << "\n";
    }

    string getAccountNumber() const { return accountNumber; }
    string getHolderName()    const { return holderName;    }
    double getBalance()       const { return balance;       }
    bool   getIsActive()      const { return isActive;      }
};

int Account::accountCounter = 0;

// --- Bank Class ---
class Bank {
private:
    string bankName;
    vector<Account*> accounts;

    Account* findAccount(const string& accNo) {
        for (Account* a : accounts)
            if (a->getAccountNumber() == accNo) return a;
        return nullptr;
    }

public:
    Bank(const string& name) : bankName(name) {}

    ~Bank() {
        for (Account* a : accounts) delete a;
    }

    void printHeader() const {
        cout << "\n";
        cout << "  ==========================================\n";
        cout << "     Welcome to " << bankName << "\n";
        cout << "  ==========================================\n\n";
    }

    string createAccount(const string& name, double deposit, const string& type = "SAVINGS") {
        Account* acc = new Account(name, deposit, type);
        accounts.push_back(acc);
        cout << "  [SUCCESS] Account created: " << acc->getAccountNumber()
             << " for " << name << "\n";
        return acc->getAccountNumber();
    }

    bool creditAccount(const string& accNo, double amount, const string& desc = "Deposit") {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found: " << accNo << "\n"; return false; }
        return acc->credit(amount, desc);
    }

    bool debitAccount(const string& accNo, double amount, const string& desc = "Withdrawal") {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found: " << accNo << "\n"; return false; }
        return acc->debit(amount, desc);
    }

    bool transferFunds(const string& fromAcc, const string& toAcc, double amount) {
        Account* src = findAccount(fromAcc);
        Account* dst = findAccount(toAcc);
        if (!src) { cout << "  [ERROR] Source account not found: " << fromAcc << "\n"; return false; }
        if (!dst) { cout << "  [ERROR] Destination account not found: " << toAcc << "\n"; return false; }
        return src->transfer(*dst, amount);
    }

    void showAccount(const string& accNo) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        acc->displayDetails();
    }

    void showStatement(const string& accNo, int last = 10) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "  [ERROR] Account not found.\n"; return; }
        acc->printStatement(last);
    }

    void listAllAccounts() const {
        cout << "\n  All Accounts in " << bankName << ":\n";
        cout << "  " << string(55, '-') << "\n";
        cout << "  " << left << setw(12) << "Acc No"
             << setw(22) << "Holder"
             << setw(14) << "Balance (Rs.)"
             << "Status\n";
        cout << "  " << string(55, '-') << "\n";
        for (const Account* a : accounts) {
            cout << "  " << left << setw(12) << a->getAccountNumber()
                 << setw(22) << a->getHolderName()
                 << setw(14) << fixed << setprecision(2) << a->getBalance()
                 << (a->getIsActive() ? "ACTIVE" : "INACTIVE") << "\n";
        }
        cout << "  " << string(55, '-') << "\n";
        cout << "  Total accounts: " << accounts.size() << "\n";
    }

    void runMenu() {
        printHeader();
        int choice;
        string acc1, acc2, name;
        double amount;

        do {
            cout << "\n  ========= MAIN MENU =========\n";
            cout << "  1. Create New Account\n";
            cout << "  2. Credit (Deposit)\n";
            cout << "  3. Debit (Withdraw)\n";
            cout << "  4. Transfer Funds\n";
            cout << "  5. View Account Details\n";
            cout << "  6. Print Statement\n";
            cout << "  7. List All Accounts\n";
            cout << "  0. Exit\n";
            cout << "  -----------------------------\n";
            cout << "  Enter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case 1:
                cout << "  Account Holder Name: ";
                getline(cin, name);
                cout << "  Initial Deposit (Rs.): ";
                cin >> amount;
                cin.ignore();
                createAccount(name, amount);
                break;

            case 2:
                cout << "  Account Number: ";
                getline(cin, acc1);
                cout << "  Amount to Deposit (Rs.): ";
                cin >> amount;
                cin.ignore();
                creditAccount(acc1, amount);
                break;

            case 3:
                cout << "  Account Number: ";
                getline(cin, acc1);
                cout << "  Amount to Withdraw (Rs.): ";
                cin >> amount;
                cin.ignore();
                debitAccount(acc1, amount);
                break;

            case 4:
                cout << "  From Account: ";
                getline(cin, acc1);
                cout << "  To Account:   ";
                getline(cin, acc2);
                cout << "  Amount (Rs.): ";
                cin >> amount;
                cin.ignore();
                transferFunds(acc1, acc2, amount);
                break;

            case 5:
                cout << "  Account Number: ";
                getline(cin, acc1);
                showAccount(acc1);
                break;

            case 6:
                cout << "  Account Number: ";
                getline(cin, acc1);
                showStatement(acc1);
                break;

            case 7:
                listAllAccounts();
                break;

            case 0:
                cout << "  Thank you for banking with " << bankName << ". Goodbye!\n\n";
                break;

            default:
                cout << "  Invalid option. Try again.\n";
            }
        } while (choice != 0);
    }
};

// --- Demo / Test run ---
void runDemo(Bank& bank) {
    cout << "\n  =================================\n";
    cout << "  AUTOMATED DEMO\n";
    cout << "  =================================\n";

    string a1 = bank.createAccount("Priya Sharma", 50000.0, "SAVINGS");
    string a2 = bank.createAccount("Arjun Mehta",  30000.0, "CURRENT");
    string a3 = bank.createAccount("Kavitha Nair", 75000.0, "SAVINGS");

    cout << "\n  -- Credit Operations --\n";
    bank.creditAccount(a1, 15000.0, "Salary credit");
    bank.creditAccount(a2,  8000.0, "Client payment");

    cout << "\n  -- Debit Operations --\n";
    bank.debitAccount(a1,  5000.0, "Rent payment");
    bank.debitAccount(a2,  3500.0, "Utility bills");
    bank.debitAccount(a3, 90000.0, "Excess withdrawal attempt");

    cout << "\n  -- Transfer Operation --\n";
    bank.transferFunds(a1, a3, 10000.0);

    cout << "\n  -- Account Details --\n";
    bank.showAccount(a1);

    cout << "\n  -- Mini Statement --\n";
    bank.showStatement(a1, 10);

    cout << "\n  -- All Accounts --\n";
    bank.listAllAccounts();
}

// --- Entry Point ---
int main() {
    Bank myBank("ClaudeBank National Ltd.");

    int mode;
    cout << "\n  ===================================\n";
    cout << "    BANK OPERATIONS SYSTEM v1.0\n";
    cout << "  ===================================\n";
    cout << "  1. Run Automated Demo\n";
    cout << "  2. Interactive Menu\n";
    cout << "  -----------------------------------\n";
    cout << "  Select mode: ";
    cin >> mode;
    cin.ignore();

    if (mode == 1) {
        runDemo(myBank);
    } else {
        myBank.runMenu();
    }

    return 0;
}
