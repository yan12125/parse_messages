#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include <unordered_map>
#include <functional>
using namespace std;
using namespace xercesc;

class MySAX2Handler : public DefaultHandler
{
public:
    typedef function<void(string, int, string, string)> CallbackT;
    MySAX2Handler(CallbackT& callback);
    ~MySAX2Handler();

    void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);
    void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    void characters(const XMLCh *const chars, const XMLSize_t length);
    void fatalError(const SAXParseException& ex);

private:
    void initXMLStrings();
    void addXmlString(const char* str);
    void cleanupXmlStrings();
    bool xmlStringEquals(const XMLCh* l, const char* r);
    enum { NONE, NEW_THREAD, MSG_META, MSG_USER, MSG_CONTENT } state;
    char* thread;
    char* meta;
    string user;
    string content;
    unordered_map<const char*, const XMLCh*> xmlStrings;
    bool debug;
    CallbackT& callback;
};

