#include <iostream>
#include <vector>
#include "Account.h"
using namespace std;

int main() {
    vector<Account*> accounts;
    int choice;

    do {
        cout << "1. Create Account\n2. Deposit\n3. Withdraw\n4. Show Account\n5. Save\n6. Load\n7. Exit\n";
        cin >> choice;
        switch(choice){
            case 1: /* Create dynamically using new */ break;
            case 2: /* Deposit */ break;
            case 3: /* Withdraw */ break;
            case 4: /* Show */ break;
            case 5: /* Save */ break;
            case 6: /* Load */ break;
        }
    } while(choice != 7);

    for(Account* acc : accounts) delete acc;
    accounts.clear();
    return 0;
}