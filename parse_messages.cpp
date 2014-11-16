#include "MySAX2Handler.hpp"
#include "MyInputSource.hpp"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
using namespace xercesc;

void parseMessageHtm(const char* filename)
{
    XMLPlatformUtils::Initialize();

    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

    MySAX2Handler* defaultHandler = new MySAX2Handler();
    parser->setContentHandler(defaultHandler);
    parser->setErrorHandler(defaultHandler);

    MyInputSource* stream = new MyInputSource(filename);

    parser->parse(*stream);

    delete stream;
    delete parser;
    delete defaultHandler;

    // should be after delete statements
    XMLPlatformUtils::Terminate();
}

int main (int argc, char* argv[])
{
    if(argc != 2)
    {
        return -1;
    }

    parseMessageHtm(argv[1]);

    return 0;
}
