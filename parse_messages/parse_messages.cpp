#include "MySAX2Handler.hpp"
#include "MyInputSource.hpp"
#include "SQLiteInserter.hpp"
#include <sqlite/execute.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <string>
#include <iostream>
using namespace xercesc;

void parseMessageHtm(const char* filename, SQLiteInsertor<string, int, string, string>& inserter)
{
    XMLPlatformUtils::Initialize();

    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

    MySAX2Handler::CallbackT callback = [&inserter] (string thread, int timestamp, string user, string content) {
        cout << thread << "," << timestamp << "," << user << "," << content << "\n";
        inserter.push_data(thread, timestamp, user, content);
    };
    MySAX2Handler* defaultHandler = new MySAX2Handler(callback);
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
    sqlite::connection con("output.db");
    sqlite::execute(con, "CREATE TABLE IF NOT EXISTS messages (thread text, timestamp int, user text, content text)", true);
    SQLiteInsertor<string, int, string, string> inserter(con, "INSERT INTO messages (thread,timestamp,user,content) VALUES (?,?,?,?)", 200);

    parseMessageHtm(argv[1], inserter);

    return 0;
}
