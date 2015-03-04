#pragma once

#include <string>
#include <functional>
#include <QXmlDefaultHandler>

class MySAX2Handler : public QXmlDefaultHandler
{
public:
    typedef std::function<void(std::string, std::string, std::string, std::string, int)> CallbackT;
    MySAX2Handler(CallbackT& callback);
    ~MySAX2Handler();

    bool startElement(const QString& uri, const QString& localname, const QString& qname, const QXmlAttributes& attrs);
    bool endElement(const QString& uri, const QString& localname, const QString& qname);
    bool characters(const QString& chars);
    bool fatalError(const QXmlParseException& ex);

private:
    enum { NONE, NEW_THREAD, MSG_META, MSG_USER, MSG_CONTENT, WARNING } state;
    std::string thread;
    std::string meta;
    std::string user;
    std::string content;
    std::string last_meta;
    CallbackT& callback;
    int contentIndex; // to differentiate messages in the same minute
};
