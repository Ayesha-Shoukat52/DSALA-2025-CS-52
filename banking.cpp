#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

/* PERMISSION FLAGS */
#define CAN_WITHDRAW 1
#define CAN_DEPOSIT 2
#define CAN_TRANSFER 4
#define VIP 8

/* TRANSACTION TYPES */
#define DEPOSIT_T 1
#define WITHDRAW_T 2
#define TRANSFER_T 3


/* TRANSACTION COMPRESSION */
unsigned int encodeTransaction(int type, int amount)
{
    return (type << 28) | amount;
}

int decodeType(unsigned int code)
{
    return code >> 28;
}

int decodeAmount(unsigned int code)
{
    return code & 0x0FFFFFFF;
}


/* ABSTRACT ACCOUNT CLASS */
class Account
{
protected:
    int accountId;
    string name;
    double balance;
    unsigned int permissions;

    vector<unsigned int> transactions;

public:

    Account() {}

    Account(int id,string n,double b,unsigned int p)
    {
        accountId=id;
        name=n;
        balance=b;
        permissions=p;
    }

    virtual void deposit(double amount)=0;
    virtual void withdraw(double amount)=0;
    virtual void saveToFile(ofstream& file)=0;

    virtual void show()
    {
        cout<<"\nAccount ID: "<<accountId<<endl;
        cout<<"Name: "<<name<<endl;
        cout<<"Balance: "<<balance<<endl;
        cout<<"Permissions: "<<permissions<<endl;

        cout<<"Transactions:\n";

        for(int i=0;i<transactions.size();i++)
        {
            int type=decodeType(transactions[i]);
            int amount=decodeAmount(transactions[i]);

            if(type==DEPOSIT_T)
                cout<<"Deposit: ";

            if(type==WITHDRAW_T)
                cout<<"Withdraw: ";

            if(type==TRANSFER_T)
                cout<<"Transfer: ";

            cout<<amount<<endl;
        }
    }

    int getId()
    {
        return accountId;
    }

    vector<unsigned int>& getTransactions()
    {
        return transactions;
    }

    virtual ~Account(){}
};


/* SAVINGS ACCOUNT */

class SavingsAccount:public Account
{
public:

    SavingsAccount(int id,string n,double b,unsigned int p)
        :Account(id,n,b,p){}

    void deposit(double amount)
    {
        if(!(permissions & CAN_DEPOSIT))
        {
            cout<<"Deposit permission denied\n";
            return;
        }

        balance+=amount;

        transactions.push_back(encodeTransaction(DEPOSIT_T,amount));

        cout<<"Deposit successful\n";
    }

    void withdraw(double amount)
    {
        if(!(permissions & CAN_WITHDRAW))
        {
            cout<<"Withdraw permission denied\n";
            return;
        }

        if(balance<amount)
        {
            cout<<"Insufficient balance\n";
            return;
        }

        balance-=amount;

        transactions.push_back(encodeTransaction(WITHDRAW_T,amount));

        cout<<"Withdraw successful\n";
    }

    void saveToFile(ofstream& file)
    {
        file<<"ACCOUNT Savings\n";

        file<<accountId<<" "<<name<<" "<<balance<<" "<<permissions<<endl;

        file<<"TRANSACTIONS\n";

        for(int i=0;i<transactions.size();i++)
        {
            file<<transactions[i]<<endl;
        }

        file<<"END\n";
    }
};



/* CURRENT ACCOUNT */

class CurrentAccount:public Account
{
public:

    CurrentAccount(int id,string n,double b,unsigned int p)
        :Account(id,n,b,p){}

    void deposit(double amount)
    {
        if(!(permissions & CAN_DEPOSIT))
        {
            cout<<"Deposit permission denied\n";
            return;
        }

        balance+=amount;

        transactions.push_back(encodeTransaction(DEPOSIT_T,amount));

        cout<<"Deposit successful\n";
    }

    void withdraw(double amount)
    {
        if(!(permissions & CAN_WITHDRAW))
        {
            cout<<"Withdraw permission denied\n";
            return;
        }

        balance-=amount;

        transactions.push_back(encodeTransaction(WITHDRAW_T,amount));

        cout<<"Withdraw successful\n";
    }

    void saveToFile(ofstream& file)
    {
        file<<"ACCOUNT Current\n";

        file<<accountId<<" "<<name<<" "<<balance<<" "<<permissions<<endl;

        file<<"TRANSACTIONS\n";

        for(int i=0;i<transactions.size();i++)
        {
            file<<transactions[i]<<endl;
        }

        file<<"END\n";
    }
};



/* BANK SYSTEM */

class BankSystem
{
private:

    vector<Account*> accounts;

public:

    Account* findAccount(int id)
    {
        for(int i=0;i<accounts.size();i++)
        {
            if(accounts[i]->getId()==id)
                return accounts[i];
        }

        return NULL;
    }

    void createAccount()
    {
        int type,id;
        string name;
        double balance;
        unsigned int permissions;

        cout<<"1 Savings\n2 Current\n";
        cin>>type;

        cout<<"Account ID: ";
        cin>>id;

        cout<<"Name: ";
        cin>>name;

        cout<<"Initial Balance: ";
        cin>>balance;

        cout<<"Permissions number: ";
        cin>>permissions;

        Account* acc;

        if(type==1)
            acc=new SavingsAccount(id,name,balance,permissions);
        else
            acc=new CurrentAccount(id,name,balance,permissions);

        accounts.push_back(acc);

        cout<<"Account created\n";
    }

    void deposit()
    {
        int id;
        double amount;

        cout<<"Account ID: ";
        cin>>id;

        Account* acc=findAccount(id);

        if(acc==NULL)
        {
            cout<<"Account not found\n";
            return;
        }

        cout<<"Amount: ";
        cin>>amount;

        acc->deposit(amount);
    }

    void withdraw()
    {
        int id;
        double amount;

        cout<<"Account ID: ";
        cin>>id;

        Account* acc=findAccount(id);

        if(acc==NULL)
        {
            cout<<"Account not found\n";
            return;
        }

        cout<<"Amount: ";
        cin>>amount;

        acc->withdraw(amount);
    }

    void showAccount()
    {
        int id;

        cout<<"Account ID: ";
        cin>>id;

        Account* acc=findAccount(id);

        if(acc)
            acc->show();
        else
            cout<<"Account not found\n";
    }


    /* SAVE ALL ACCOUNTS */

    void save()
    {
        ofstream file("bank.txt");

        for(int i=0;i<accounts.size();i++)
        {
            accounts[i]->saveToFile(file);
        }

        file.close();

        cout<<"Saved to file\n";
    }



    /* LOAD FROM FILE */

    void load()
    {
        ifstream file("bank.txt");

        string word;

        while(file>>word)
        {
            if(word=="ACCOUNT")
            {
                string type;

                file>>type;

                int id;
                string name;
                double balance;
                unsigned int perm;

                file>>id>>name>>balance>>perm;

                Account* acc;

                if(type=="Savings")
                    acc=new SavingsAccount(id,name,balance,perm);
                else
                    acc=new CurrentAccount(id,name,balance,perm);

                file>>word;

                while(true)
                {
                    unsigned int t;

                    file>>t;

                    if(file.fail())
                        break;

                    if(t==0)
                        break;

                    acc->getTransactions().push_back(t);

                    if(file.peek()=='E')
                        break;
                }

                accounts.push_back(acc);
            }
        }

        file.close();

        cout<<"Data loaded\n";
    }



    /* MONTHLY SUMMARY USING ARRAY */

    void monthlySummary()
    {
        double monthlyTotals[12]={0};

        for(int i=0;i<accounts.size();i++)
        {
            vector<unsigned int>& t=accounts[i]->getTransactions();

            for(int j=0;j<t.size();j++)
            {
                int amount=decodeAmount(t[j]);

                int month=j%12;

                monthlyTotals[month]+=amount;
            }
        }

        cout<<"\nMonthly Totals\n";

        for(int i=0;i<12;i++)
        {
            cout<<"Month "<<i+1<<": "<<monthlyTotals[i]<<endl;
        }
    }



    ~BankSystem()
    {
        for(int i=0;i<accounts.size();i++)
        {
            delete accounts[i];
        }
    }

};



/* MAIN PROGRAM */

int main()
{
    BankSystem bank;

    int choice;

    while(true)
    {
        cout<<"\n1 Create Account\n";
        cout<<"2 Deposit\n";
        cout<<"3 Withdraw\n";
        cout<<"4 Show Account\n";
        cout<<"5 Save to File\n";
        cout<<"6 Load from File\n";
        cout<<"7 Monthly Summary\n";
        cout<<"8 Exit\n";

        cin>>choice;

        if(choice==1)
            bank.createAccount();

        else if(choice==2)
            bank.deposit();

        else if(choice==3)
            bank.withdraw();

        else if(choice==4)
            bank.showAccount();

        else if(choice==5)
            bank.save();

        else if(choice==6)
            bank.load();

        else if(choice==7)
            bank.monthlySummary();

        else if(choice==8)
            break;
    }

    return 0;
} 