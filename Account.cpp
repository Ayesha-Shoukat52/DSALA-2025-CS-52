#include "Account.h"

// Show account details
void Account::showAccount() {
    cout << "ID: " << accountId << "\nName: " << name 
         << "\nBalance: " << balance << "\nPermissions: " << permissions << endl;
}

// ---------------- SavingsAccount ----------------
SavingsAccount::SavingsAccount(int id, string n, double bal, unsigned int perm)
    : Account(id, n, bal, perm) {}

void SavingsAccount::deposit(double amount) {
    if (!(permissions & 2)) { cout << "Deposit not allowed!\n"; return; }
    balance += amount;
    transactions.push_back(amount);
}

void SavingsAccount::withdraw(double amount) {
    if (!(permissions & 1)) { cout << "Withdraw not allowed!\n"; return; }
    if(amount > balance){ cout << "Insufficient balance!\n"; return; }
    balance -= amount;
    transactions.push_back(-amount);
}

void SavingsAccount::saveToFile(ofstream &ofs) {
    ofs << "ACCOUNT Savings\n" << accountId << " " << name << " " << balance << " " << permissions << "\nTRANSACTIONS\n";
    for(double t : transactions) ofs << t << "\n";
}

void SavingsAccount::loadFromFile(ifstream &ifs) {
    transactions.clear();
    string dummy;
    ifs >> accountId >> name >> balance >> permissions;
    ifs >> dummy; // TRANSACTIONS
    double t;
    while (ifs >> t) transactions.push_back(t);
}

// ---------------- CurrentAccount ----------------
CurrentAccount::CurrentAccount(int id, string n, double bal, unsigned int perm)
    : Account(id, n, bal, perm) {}

void CurrentAccount::deposit(double amount) {
    if (!(permissions & 2)) { cout << "Deposit not allowed!\n"; return; }
    balance += amount;
    transactions.push_back(amount);
}

void CurrentAccount::withdraw(double amount) {
    if (!(permissions & 1)) { cout << "Withdraw not allowed!\n"; return; }
    if(amount > balance){ cout << "Insufficient balance!\n"; return; }
    balance -= amount;
    transactions.push_back(-amount);
}

void CurrentAccount::saveToFile(ofstream &ofs) {
    ofs << "ACCOUNT Current\n" << accountId << " " << name << " " << balance << " " << permissions << "\nTRANSACTIONS\n";
    for(double t : transactions) ofs << t << "\n";
}

void CurrentAccount::loadFromFile(ifstream &ifs) {
    transactions.clear();
    string dummy;
    ifs >> accountId >> name >> balance >> permissions;
    ifs >> dummy; // TRANSACTIONS
    double t;
    while (ifs >> t) transactions.push_back(t);
}