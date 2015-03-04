#include "MySAX2Handler.hpp"
#include "util.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

std::ostream& operator<<(std::ostream& os, const QString& str)
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
        std::cout << "<" << localname;
        if(!htmlClass.isEmpty())
        {
            std::cout << "." << htmlClass;
        }
        std::cout << ">\n";
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
        else if(htmlClass == "warning")
        {
            state = WARNING;
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
        std::cout << "</" << localname << ">\n";
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
    std::string chars2 = chars.toStdString();
    if(Util::debug)
    {
        std::cout << "Characters: " << chars2 << "\n";
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
        case WARNING:
        {
            std::stringstream ss;
            ss << "Warning: " << chars2 << "\nPlease download your facebook archive again.";
            throw std::runtime_error(ss.str());
        }
        case NONE:
            break;
    }
    return true;
}

bool MySAX2Handler::fatalError(const QXmlParseException& ex)
{
    std::stringstream ss;
    ss << "Fatal Error: " << ex.message()
       << " at line " << ex.lineNumber()
       << ", column " << ex.columnNumber();
    throw std::runtime_error(ss.str());
}
