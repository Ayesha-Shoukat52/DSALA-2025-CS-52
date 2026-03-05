#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

class Account {
protected:
    int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<double> transactions;

public:
    Account(int id, string n, double bal, unsigned int perm)
        : accountId(id), name(n), balance(bal), permissions(perm) {}

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void saveToFile(ofstream &ofs) = 0;
    virtual void loadFromFile(ifstream &ifs) = 0;
    virtual void showAccount();
    virtual ~Account() { transactions.clear(); }
};

// Derived classes
class SavingsAccount : public Account {
public:
    SavingsAccount(int id, string n, double bal, unsigned int perm);
    void deposit(double amount) override;
    void withdraw(double amount) override;
    void saveToFile(ofstream &ofs) override;
    void loadFromFile(ifstream &ifs) override;
};

class CurrentAccount : public Account {
public:
    CurrentAccount(int id, string n, double bal, unsigned int perm);
    void deposit(double amount) override;
    void withdraw(double amount) override;
    void saveToFile(ofstream &ofs) override;
    void loadFromFile(ifstream &ifs) override;
};

#endif