#include "MySAX2Handler.hpp"
#include "util.hpp"
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <iostream>
using namespace std;

ostream& operator<<(ostream& os, const XMLCh* str)
{
    char* nativeStr = XMLString::transcode(str);
    os << nativeStr;
    XMLString::release(&nativeStr);
    return os;
}

MySAX2Handler::MySAX2Handler(sqlite::connection* con): 
    inserter(con, "INSERT INTO messages (thread,timestamp,user,content) VALUES (?,?,?,?)", 200)
{
    initXMLStrings();
    state = NONE;
    debug = false;
}

MySAX2Handler::~MySAX2Handler()
{
    cleanupXmlStrings();
    XMLString::release(&meta);
    XMLString::release(&user);
    XMLString::release(&thread);
}

bool MySAX2Handler::xmlStringEquals(const XMLCh* l, const char* r)
{
    auto it = xmlStrings.find(r);
    if(it == xmlStrings.end())
    {
        throw "Unexpected string";
    }
    return XMLString::equals(l, xmlStrings[r]);
}

void MySAX2Handler::startElement(const XMLCh* const /*uri*/, const XMLCh* const localname, const XMLCh* const /*qname*/, const Attributes& attrs)
{
    const XMLCh* htmlClass = attrs.getValue(xmlStrings["class"]);
    if(debug)
    {
        cout << "<" << localname;
        if(htmlClass)
        {
            cout << "." << htmlClass;
        }
        cout << ">\n";
    }
    if(xmlStringEquals(localname, "div"))
    {
        if(xmlStringEquals(htmlClass, "thread"))
        {
            state = NEW_THREAD;
        }
        else if(xmlStringEquals(htmlClass, "footer"))
        {
            state = NONE;
        }
    }
    else if(xmlStringEquals(localname, "span"))
    {
        if(xmlStringEquals(htmlClass, "user"))
        {
            state = MSG_USER;
        }
        else if(xmlStringEquals(htmlClass, "meta"))
        {
            state = MSG_META;
        }
    }
    else if(xmlStringEquals(localname, "p"))
    {
        state = MSG_CONTENT;
    }
}

void MySAX2Handler::endElement(const XMLCh* const /*uri*/, const XMLCh* const localname, const XMLCh* const /*qname*/)
{
    if(debug)
    {
        cout << "</" << localname << ">\n";
    }
}

void MySAX2Handler::characters(const XMLCh *const chars, const XMLSize_t /*length*/)
{
    if(debug)
    {
        cout << "Characters: " << chars << "\n";
    }
    switch(state)
    {
        case NEW_THREAD:
            XMLString::release(&thread);
            thread = XMLString::transcode(chars);
            break;
        case MSG_META:
            XMLString::release(&meta);
            meta = XMLString::transcode(chars);
            break;
        case MSG_USER:
            XMLString::release(&user);
            user = XMLString::transcode(chars);
            break;
        case MSG_CONTENT:
        {
            char* content = XMLString::transcode(chars);
            inserter.push_data(thread, Util::timestamp(meta), user, content);
            XMLString::release(&content);
            break;
        }
        case NONE:
            break;
    }
}

void MySAX2Handler::fatalError(const SAXParseException& ex)
{
    cout << "Fatal Error: " << ex.getMessage()
        << " at line: "    << ex.getLineNumber()
        << endl;
}

void MySAX2Handler::initXMLStrings()
{
    addXmlString("div");
    addXmlString("span");
    addXmlString("p");
    addXmlString("class");

    addXmlString("user");
    addXmlString("meta");
    addXmlString("thread");
    addXmlString("message");
    addXmlString("footer");
}

void MySAX2Handler::addXmlString(const char* str)
{
    // copy char* to unsigned char*
    size_t N = strlen(str)+1;
    unsigned char* ptr = new unsigned char[N];
    memcpy(ptr, str, N);

    // transform unsigned char* (XMLByte*) to XMLCh*
    TranscodeFromStr transcoder(ptr, N, "UTF8");

    // save XMLCh* results
    XMLCh* data = new XMLCh[transcoder.length()+1];
    memcpy(data, transcoder.str(), sizeof(XMLCh) * (transcoder.length()+1));
    xmlStrings[str] = data;
    delete [] ptr;
}

void MySAX2Handler::cleanupXmlStrings()
{
    for(pair<const char*, const XMLCh*> xmlStringPair : xmlStrings)
    {
        delete [] xmlStringPair.second;
    }
}
