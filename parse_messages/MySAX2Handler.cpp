#include "MySAX2Handler.hpp"
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;

ostream& operator<<(ostream& os, const XMLCh* str)
{
    char* nativeStr = XMLString::transcode(str);
    os << nativeStr;
    XMLString::release(&nativeStr);
    return os;
}

MySAX2Handler::MySAX2Handler(CallbackT& callback): callback(callback)
{
    initXMLStrings();
    state = NONE;
    XMLCh encoding[] = { 'U', 'T', 'F', '8', 0 };
    transcoder = new XMLUTF8Transcoder(encoding, 1024);
#ifdef MY_DEBUG
    debug = true;
#else
    debug = false;
#endif
}

MySAX2Handler::~MySAX2Handler()
{
    delete transcoder;
    cleanupXmlStrings();
}

bool MySAX2Handler::xmlStringEquals(const XMLCh* l, const char* r) const
{
    auto it = xmlStrings.find(r);
    if(it == xmlStrings.end())
    {
        throw "Unexpected string";
    }
    return XMLString::equals(l, it->second);
}

string MySAX2Handler::toUTF8(const XMLCh* str, XMLSize_t len) const
{
    if(debug)
    {
        cout << "toUTF8 str = " << str << ", len = " << len << endl;
    }

    XMLSize_t got = 0;
    XMLByte* buf = new XMLByte[len * 3 + 1]; // assume 3 is the maximum length of UTF-8 characters
    XMLSize_t bytesFilled = transcoder->transcodeTo(str, len, buf, len * 3, got, XMLUTF8Transcoder::UnRep_Throw);
    buf[bytesFilled] = '\0';
    string ret(reinterpret_cast<const char*>(buf));
    delete [] buf;

    if(debug)
    {
        cout << "result = " << ret << endl;
    }

    return ret;
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
            thread.clear();
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
    if(state == MSG_CONTENT && xmlStringEquals(localname, "p"))
    {
        callback(thread, meta, user, content);
        user.clear();
        content.clear();
    }
}

void MySAX2Handler::characters(const XMLCh *const chars, const XMLSize_t length)
{
    if(debug)
    {
        cout << "Characters: " << chars << "\n";
    }
    switch(state)
    {
        case NEW_THREAD:
            thread += toUTF8(chars, length);
            break;
        case MSG_META:
            meta = toUTF8(chars, length);
            break;
        case MSG_USER:
            user += toUTF8(chars, length);
            break;
        case MSG_CONTENT:
            content += toUTF8(chars, length);
            break;
        case NONE:
            break;
    }
}

void MySAX2Handler::fatalError(const SAXParseException& ex)
{
    stringstream ss;
    ss << "Fatal Error: " << ex.getMessage()
       << " at line " << ex.getLineNumber()
       << ", column " << ex.getColumnNumber();
    throw runtime_error(ss.str());
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
