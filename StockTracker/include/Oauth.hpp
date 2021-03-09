//
//  Oauth.hpp
//  Stocktracker
//
//  Created by Morgan Xu on 1/14/21.
//

#ifndef Oauth_hpp
#define Oauth_hpp

#include <Poco/Net/OAuth10Credentials.h>
#include <Poco/Net/HTMLForm.h>
#include "AccountList.hpp"
#include "CSVFile.hpp"
#include "Order.hpp"

#define SANDBOX_KEY "key"
#define SANDBOX_SECRETE "secrete"

enum Ordertype {
    MARKET,
    LIMIT
};

class Oauth {
    Poco::Net::HTMLForm request_token;
    Poco::Net::HTMLForm access_token;
    std::string verificationCode;
    
    CSVFile orderHistory();
    
    Poco::Net::OAuth10Credentials oauth;
    AccountList::Account account;
    
    std::string previewOrder(Order &);
    int placeOrder(Order &, std::string &);
    
    std::string generateID(Order &);
    
public:
    int getRequestToken();
    int authorization();
    int getAccessToken();
    
    void setUp();
    void setAccountID();
    
    int executeOrder(Order &);
};

#endif /* Oauth_hpp */
