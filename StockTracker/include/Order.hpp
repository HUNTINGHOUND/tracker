#ifndef Order_h
#define Order_h


struct Order {
    struct Product {
        std::string securityType;
        std::string symbol;
    };
    
    struct Instrument {
        Product product;
        std::string orderAction;
        std::string quantityType = "QUANTITY";
        std::string quantity;
    };
    
    
    std::string orderType = "EQ";
    std::string clientOrderID;
    std::string clientOrderId;
    
    std::string allOrNone = "true";
    std::string priceType;
    std::string orderTerm = "GOOD_FOR_DAY";
    std::string markestSession = "REGULAR";
    std::string stopPrice = "";
    std::string limitPrice;
    
    Instrument instrument;
};

#endif /* Order_h */
