#ifndef AccountList_h
#define AccountList_h

#include <vector>
#include <string>

struct AccountList{
    struct Account {
        std::string accountId;
        std::string accountIdKey;
        std::string accountName;
    };

    std::vector<Account> accountList;
};

#endif /* AccountList_h */
