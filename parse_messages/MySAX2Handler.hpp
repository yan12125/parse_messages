#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>
#include <unordered_map>
#include <functional>
using namespace std;
using namespace xercesc;

class MySAX2Handler : public DefaultHandler
{
public:
    typedef function<void(string, string, string, string, int)> CallbackT;
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

    // helpers
    bool xmlStringEquals(const XMLCh* l, const char* r) const;
    string toUTF8(const XMLCh* str, XMLSize_t len) const;
    XMLUTF8Transcoder* transcoder;

    enum { NONE, NEW_THREAD, MSG_META, MSG_USER, MSG_CONTENT } state;
    string thread;
    string meta;
    string user;
    string content;
    string last_meta;
    unordered_map<const char*, const XMLCh*> xmlStrings;
    bool debug;
    CallbackT& callback;
    int contentIndex; // to differentiate messages in the same minute
};

