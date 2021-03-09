#include "../include/Oauth.hpp"
#include "../include/AccountList.hpp"
#include <iostream>
#include <Poco/URI.h>
#include <Poco/Net/HTTPSession.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


int Oauth::getRequestToken() {
    try{
        
        Poco::Net::OAuth10Credentials oauth(SANDBOX_KEY, SANDBOX_SECRETE);
        oauth.setCallback("oob");
        Poco::URI uri("https://api.etrade.com/oauth/request_token");
        
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
        
        std::string path(uri.getPathAndQuery());
        if(path.empty()) path = "/";
        
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPRequest::HTTP_1_1);
        
        oauth.authenticate(req, uri);
        
        session.sendRequest(req);
        
        Poco::Net::HTTPResponse res;
        std::cout << res.getStatus() << " " << res.getReason() << std::endl;
        
        std::istream &is = session.receiveResponse(res);
        
        this->request_token.read(is);
        

        
    }catch (Poco::Exception &ex){
        std::cerr << ex.displayText() << std::endl;
        return 1;
    }
    
    return 0;
}

int Oauth::authorization() {
    Poco::URI uri("https://us.etrade.com/e/t/etws/authorize");
    std::vector<std::pair<std::string, std::string>> parameters;
    
    parameters.push_back(std::pair<std::string, std::string>("key", SANDBOX_KEY));
    parameters.push_back(std::pair<std::string, std::string>("token", this->request_token["oauth_token"]));
    
    uri.setQueryParameters(parameters);
    
    std::string command = "open -a safari \"" + uri.toString() + "\"";
    
    system(command.c_str());
    
    std::cout << "Enter the verfication code: ";
    std::cin >> verificationCode;
    std::cout << std::endl;
    
    return 0;
}


int Oauth::getAccessToken(){
    try {
        
        Poco::Net::OAuth10Credentials oauth(SANDBOX_KEY, SANDBOX_SECRETE, this->request_token["oauth_token"], this->request_token["oauth_token_secret"]);
        
        Poco::URI uri("https://api.etrade.com/oauth/access_token");
        
        
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
        
        std::string path(uri.getPathAndQuery());
        if(path.empty()) path = "/";
        
        Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPRequest::HTTP_1_1);
        req.setContentType("application/x-www-form-urlencoded");
        
        Poco::Net::HTMLForm form;
        form.add("oauth_verifier", this->verificationCode);
        
        oauth.authenticate(req, uri, form);
        
        std::string v = ", oauth_verifier=\"" + this->verificationCode + "\"";
        std::string a = req["Authorization"];
        a += v;
        req.set("Authorization", a);
        
        session.sendRequest(req);
        
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse(response);
        this->access_token.read(responseStream);
        
    } catch(Poco::Exception& ex) {
        std::cerr << ex.displayText() << std::endl;
        return 1;
    }
    return 0;
}

void Oauth::setUp() {
    this->oauth.setConsumerKey(SANDBOX_KEY);
    this->oauth.setConsumerSecret(SANDBOX_SECRETE);
    this->oauth.setToken(this->access_token["oauth_token"]);
    this->oauth.setTokenSecret(this->access_token["oauth_token_secret"]);
    
}

void Oauth::setAccountID() {
    Poco::URI uri("https://apisb.etrade.com/v1/accounts/list");
    
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
    
    std::string path(uri.getPathAndQuery());
    if(path.empty()) path = "/";
    
    Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPRequest::HTTP_1_1);
    
    oauth.authenticate(req, uri);
    
    session.sendRequest(req);
    
    Poco::Net::HTTPResponse response;
    
    std::istream& responseStream = session.receiveResponse(response);
    std::cout << response.getStatus() << " " << response.getReason() << std::endl;
    
    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml(responseStream, pt);
    
    AccountList list;
    
    for(boost::property_tree::ptree::value_type &v : pt.get_child("AccountListResponse.Accounts")) {
        AccountList::Account ac;
        ac.accountName = v.second.get<std::string>("accountName");
        ac.accountId = v.second.get<std::string>("accountId");
        ac.accountIdKey = v.second.get<std::string>("accountIdKey");
        list.accountList.push_back(ac);
    }
    
    std::cout << "Choose account: " << std::endl;
    for(int i = 1; i <= list.accountList.size(); i++) {
        std::cout << i << "." << list.accountList[i - 1].accountName << std::endl;
    }
    
    int choice;
    std::cin >> choice;
    
    this->account = list.accountList[choice - 1];
}

std::string Oauth::generateID(Order &order) {
    std::string id;
    id += order.orderType;
    time_t epochStamp = std::time(nullptr);
    return "";
}
