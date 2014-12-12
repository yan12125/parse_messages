#include "MySAX2Handler.hpp"
#include "util.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;

ostream& operator<<(ostream& os, const QString& str)
{
    os << str.toUtf8().constData();
    return os;
}

MySAX2Handler::MySAX2Handler(CallbackT& callback): callback(callback), contentIndex(0)
{
    state = NONE;
}

MySAX2Handler::~MySAX2Handler()
{
}

bool MySAX2Handler::startElement(const QString& /*uri*/, const QString& localname, const QString& /*qname*/, const QXmlAttributes& attrs)
{
    QString htmlClass = attrs.value("class");
    if(Util::debug)
    {
        cout << "<" << localname;
        if(!htmlClass.isEmpty())
        {
            cout << "." << htmlClass;
        }
        cout << ">\n";
    }
    if(localname == "div")
    {
        if(htmlClass == "thread")
        {
            state = NEW_THREAD;
            thread.clear();
        }
        else if(htmlClass == "footer")
        {
            state = NONE;
        }
    }
    else if(localname == "span")
    {
        if(htmlClass == "user")
        {
            state = MSG_USER;
        }
        else if(htmlClass == "meta")
        {
            state = MSG_META;
        }
    }
    else if(localname == "p")
    {
        state = MSG_CONTENT;
    }
    return true;
}

bool MySAX2Handler::endElement(const QString& /*uri*/, const QString& localname, const QString& /*qname*/)
{
    if(Util::debug)
    {
        cout << "</" << localname << ">\n";
    }
    if(state == MSG_CONTENT && localname == "p")
    {
        callback(thread, meta, user, content, contentIndex);
        contentIndex++;
        user.clear();
        content.clear();
    }
    return true;
}

bool MySAX2Handler::characters(const QString& chars)
{
    string chars2 = chars.toStdString();
    if(Util::debug)
    {
        cout << "Characters: " << chars2 << "\n";
    }
    switch(state)
    {
        case NEW_THREAD:
            thread += chars2;
            break;
        case MSG_META:
            meta = chars2;
            if(meta != last_meta)
            {
                contentIndex = 0;
                last_meta = meta;
            }
            break;
        case MSG_USER:
            user += chars2;
            break;
        case MSG_CONTENT:
            content += chars2;
            break;
        case NONE:
            break;
    }
    return true;
}

bool MySAX2Handler::fatalError(const QXmlParseException& ex)
{
    stringstream ss;
    ss << "Fatal Error: " << ex.message()
       << " at line " << ex.lineNumber()
       << ", column " << ex.columnNumber();
    throw runtime_error(ss.str());
}
