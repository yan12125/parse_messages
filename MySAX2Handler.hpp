#pragma once

#include "SQLiteInserter.hpp"
#include <xercesc/sax2/DefaultHandler.hpp>
#include <unordered_map>
using namespace std;
using namespace xercesc;

class MySAX2Handler : public DefaultHandler
{
public:
    MySAX2Handler(sqlite::connection& con);
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
    char* user;
    unordered_map<const char*, const XMLCh*> xmlStrings;
    bool debug;
    SQLiteInsertor<string, int, string, string> inserter;

};

