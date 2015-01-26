#pragma once

#include <QXmlDefaultHandler>
#include <unordered_map>
#include <functional>
using namespace std;

class MySAX2Handler : public QXmlDefaultHandler
{
public:
    typedef function<void(string, string, string, string, int)> CallbackT;
    MySAX2Handler(CallbackT& callback);
    ~MySAX2Handler();

    bool startElement(const QString& uri, const QString& localname, const QString& qname, const QXmlAttributes& attrs);
    bool endElement(const QString& uri, const QString& localname, const QString& qname);
    bool characters(const QString& chars);
    bool fatalError(const QXmlParseException& ex);

private:
    enum { NONE, NEW_THREAD, MSG_META, MSG_USER, MSG_CONTENT, WARNING } state;
    string thread;
    string meta;
    string user;
    string content;
    string last_meta;
    CallbackT& callback;
    int contentIndex; // to differentiate messages in the same minute
};
