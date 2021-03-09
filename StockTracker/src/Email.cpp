#define USERNAME "STOCKALERT_NO_REPLY@outlook.com"
#define PASSWORD "aabbccddeefgh888" //actual password
#define SMTPSERVER "smtp.office365.com"
#define SMTPPORT ":587"

#define FROM "STOCKALERT_NO_REPLY@outlook.com"

#include "../include/Email.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>
#include <functional>


Email::~Email() {
    curl_easy_cleanup(curl);
}

Email::Email(std::string& smtpHost, int port, std::string& username, std::string& password) {
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

        std::string smtpurl = smtpHost + ":" + std::to_string(port);
        curl_easy_setopt(curl, CURLOPT_URL, smtpurl.c_str());

        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    }
    else
        throw "curl initialize error";
}

void Email::setSender(std::string& address) {
    this->from = "<" + address + ">";
}

void Email::setRecipient(std::string& address) {
    this->recipient = "<" + address + ">";
    if (this->cc.empty()) {
        this->setCc(address);
    }
}

void Email::setCc(std::string& address) {
    this->cc = "<" + address + ">";
}

void Email::setSubject(std::string& subject) {
    this->subject = subject;
}

void Email::setBody(std::string& body) {
    this->body = body;
}

int Email::send() {
    CURLcode res = CURLE_OK;
    curl_slist* recipients = NULL;
    upload upload_ctx;

    this->date = this->giveTime();
    this->id = this->generateMessageID();

    this->constructMessage();
    upload_ctx.counter = 0;
    upload_ctx.content = message;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, this->from.c_str());
    recipients = curl_slist_append(recipients, this->recipient.c_str());
    recipients = curl_slist_append(recipients, this->cc.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, this->writeFunction);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failure: %s\n", curl_easy_strerror(res));
    }

    curl_slist_free_all(recipients);

    return (int)res;
}

size_t Email::writeFunction(char* ptr, size_t size, size_t nmemb, void* userp) {
    upload* upload_ctx = (upload*)userp;
    const char* data;

    if (size == 0 || nmemb == 0 || (size * nmemb) < 1) {
        return 0;
    }

    if (upload_ctx->counter != upload_ctx->content.size()) {
        data = upload_ctx->content[upload_ctx->counter].c_str();
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->counter++;

        return len;
    }

    return 0;
}

void Email::constructMessage() {
    std::string end = "\r\n";

    std::string d = "Date: " + this->date + end;
    this->message.push_back(d);

    std::string t = "To: " + this->recipient + end;
    this->message.push_back(t);

    std::string f = "From: " + this->from + end;
    this->message.push_back(f);

    std::string c = "Cc: " + this->cc + end;
    this->message.push_back(c);

    std::string i = "Message-ID: " + this->id + end;
    this->message.push_back(i);

    std::string s = "Subject: " + this->subject + end;
    this->message.push_back(s);
    this->message.push_back(end);

    this->message.push_back(body + end);

    //empty string to show termination
    this->message.push_back(std::string());
}

std::string Email::giveTime() {
    time_t epochStamp = std::time(nullptr);
    std::tm* time = std::localtime(&epochStamp);
    std::stringstream ss;
    ss << std::put_time(time, "%a, %d %b %Y %T %z");
    return ss.str();
}

std::string Email::generateMessageID() {
    size_t length = 46;
    time_t epochStamp = std::time(nullptr);
    std::string message_id = std::string(std::to_string(epochStamp));

    const char* choices{
        "1234567890"
        "abcdefghijklmnopqrstuvwxyz"
    };


    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 35);
    auto dice = std::bind(distribution, generator);
    while (message_id.size() < length) {
        message_id += choices[dice()];
    }

    return "<" + message_id + "@outlook.com>";
}
