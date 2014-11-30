#include "MySAX2Handler.hpp"
#include "MyInputSource.hpp"
#include "SQLiteInserter.hpp"
#include "util.hpp"
#include <sqlite/execute.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <string>
#include <thread>
#include <stdexcept>
#include <QApplication>
#include <QMessageBox>
using namespace xercesc;

Util util;

void parseMessageHtm(const char* filename, SQLiteInsertor<string, int, string, string>& inserter)
{
    XMLPlatformUtils::Initialize();

    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

    MySAX2Handler::CallbackT callback = [&inserter] (string thread, string meta, string user, string content) {
        inserter.push_data(thread, util.timestamp(meta), user, content);
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

    QApplication app(argc, argv);

    qRegisterMetaType<string>("string");

    const char* filename = argv[1];
    thread worker([filename] () {
        try
        {
            sqlite::connection con("output.db");
            sqlite::execute(con, "CREATE TABLE IF NOT EXISTS messages (thread text, timestamp int, user text, content text)", true);
            sqlite::execute(con, "CREATE TABLE IF NOT EXISTS meta (`key` text, `value` text, PRIMARY KEY (`key`))", true);
            SQLiteInsertor<string, int, string, string> inserter(con, "INSERT INTO messages (thread,timestamp,user,content) VALUES (?,?,?,?)", 200);

            parseMessageHtm(filename, inserter);
            sqlite::command writeGmtOffset(con, "INSERT INTO meta (`key`, `value`) VALUES ('gmt_offset',?)");
            (writeGmtOffset % util.get_last_gmt_offset())();
            emit util.finished();
        }
        catch(std::exception &e)
        {
            emit util.errorOccurred(e.what());
        }
        catch(int n) // xerces-c throws integers
        {
            emit util.errorOccurred(std::to_string(n));
        }
        // TODO: I don't know how to handle boost::exception
    });
    QObject::connect(&util, &Util::errorOccurred, &app, [] (string data) {
        QMessageBox::critical(nullptr, "Fatal error", data.c_str());
        QApplication::exit(-1);
    });
    QObject::connect(&util, &Util::finished, &app, &QApplication::quit);

    int ret = app.exec();

    worker.join();

    return ret;
}
