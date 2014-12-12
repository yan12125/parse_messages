#include "MySAX2Handler.hpp"
#include "SQLiteInserter.hpp"
#include "util.hpp"
#include <sqlite/execute.hpp>
#include <string>
#include <thread>
#include <stdexcept>
#include <QApplication>
#include <QMessageBox>

Util util;

typedef SQLiteInsertor<string, int, string, string, int> InserterType;

void parseMessageHtm(const char* filename, InserterType& inserter)
{
    MySAX2Handler::CallbackT callback = [&inserter] (string thread, string meta, string user, string content, int contentIndex) {
        inserter.push_data(thread, util.timestamp(meta), user, content, contentIndex);
    };

    MySAX2Handler handler(callback);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(filename);
    QXmlInputSource source(&file);
    reader.parse(source);
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
            sqlite::execute(con, "CREATE TABLE IF NOT EXISTS messages (thread text, timestamp int, user text, content text, contentIndex int)", true);
            sqlite::execute(con, "CREATE TABLE IF NOT EXISTS meta (`key` text, `value` text, PRIMARY KEY (`key`))", true);
            InserterType inserter(con, "INSERT INTO messages (thread,timestamp,user,content,contentIndex) VALUES (?,?,?,?,?)", 150);

            parseMessageHtm(filename, inserter);
            sqlite::command writeGmtOffset(con, "INSERT INTO meta (`key`, `value`) VALUES ('gmt_offset',?)");
            writeGmtOffset % util.get_last_gmt_offset();
            writeGmtOffset.emit();
            util.finished();
        }
        catch(std::exception &e)
        {
            util.errorOccurred(e.what());
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
