#ifndef Email_hpp
#define Email_hpp

#include <vector>
#include <string>
#include <curl/curl.h>


class Email {
    std::vector<std::string> message;

    CURL* curl;

    struct upload {
        int counter;
        std::vector<std::string> content;
    };

    static size_t writeFunction(char* ptr, size_t size, size_t nmemb, void* userp);

    //smtpInformation
    std::string smtpPort;
    std::string port;

    //header information
    std::string date;
    std::string id;
    std::string from;
    std::string recipient;
    std::string cc;
    std::string subject;
    std::string body;

    void constructMessage();
    std::string giveTime();
    std::string generateMessageID();

public:

    ~Email();

    /// <summary>
    /// Email constructor
    /// </summary>
    /// <param name="smtpHost">the smtp server of the email</param>
    /// <param name="port">the port for the smtp server</param>
    /// <param name="username">username for login</param>
    /// <param name="password">password for login</param>
    Email(std::string& smtpHost, int port, std::string& username, std::string& password);

    /// <summary>
    /// Set the sender for the message
    /// </summary>
    /// <param name="address">email address of the sender</param>
    void setSender(std::string& address);

    /// <summary>
    /// Set the recipient of the message, cc will default to this unless it was set already
    /// </summary>
    /// <param name="address">email address of the recipient</param>
    void setRecipient(std::string& address);

    /// <summary>
    /// Set the cc of the message
    /// </summary>
    /// <param name="address">email address of the cc</param>
    void setCc(std::string& address);

    /// <summary>
    /// Set the subject of the message
    /// </summary>
    /// <param name="subject">content</param>
    void setSubject(std::string& subject);

    /// <summary>
    /// Set the body of the message
    /// </summary>
    /// <param name="body">content</param>
    void setBody(std::string& body);

    /// <summary>
    /// Assumes that all neccessary components of the email was set, return the response code.
    /// This method will generate the date and message id element automatically. This method
    /// will establish curl connection EVERY time it is used. It will not maintain the connection
    /// to avoid over extending the connection.
    /// </summary>
    /// <returns>response code</returns>
    int send();

};


#endif /* Email_hpp */
