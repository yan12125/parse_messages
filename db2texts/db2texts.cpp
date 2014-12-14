#include <sqlite/connection.hpp>
#include <sqlite/query.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <string>
#include <locale>
#include <vector>
#include <QString>
#include <QDir>
using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        return -1;
    }
    locale loc(locale::classic(), new boost::posix_time::time_facet("%Y/%m/%d %H:%M")); // seems Facebook does not record seconds info
    stringstream ss;
    ss.imbue(loc);

    vector<string> threads;
    sqlite::connection con(QString::fromLocal8Bit(argv[1]).toStdString());
    sqlite::query threadQuery(con, "SELECT DISTINCT thread FROM messages");
    sqlite::result_type threadResults = threadQuery.get_result();
    while(threadResults->next_row())
    {
        string a_thread = threadResults->get_string(0);
        threads.push_back(a_thread);
    }

    sqlite::query gmtQuery(con, "SELECT `value` FROM meta WHERE `key` = 'gmt_offset'");
    sqlite::result_type gmtResult = gmtQuery.get_result();
    gmtResult->next_row();
    int gmt_offset = stoi(gmtResult->get_string(0));

    QDir().mkdir("output");
    QDir output_dir("output");
    ofstream f;
    for(string a_thread : threads)
    {
        QString filename = output_dir.filePath(QString::fromStdString(a_thread+".txt"));
        f.open(filename.toLocal8Bit().constData());
        sqlite::query getMessages(con, "SELECT timestamp,contentIndex,user,content FROM messages WHERE thread=? ORDER BY timestamp ASC, contentIndex DESC"); // facebook put newer messages first
        getMessages.bind(1, a_thread);
        boost::shared_ptr<sqlite::result> result2 = getMessages.get_result();
        while(result2->next_row())
        {
            int timestamp = result2->get_int(0);
            string user = result2->get_string(2);
            string content = result2->get_string(3);
            ss << boost::posix_time::from_time_t(timestamp + gmt_offset * 3600);
            f << ss.str() << "," << user << "," << content << "\n";
            ss.str(string());
        }
        f.close();
    }
    return 0;
}
