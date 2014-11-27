#include <sqlite/connection.hpp>
#include <sqlite/query.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <string>
#include <locale>
#include <vector>
using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        return -1;
    }
    locale loc(locale::classic(), new posix_time::time_facet("%Y/%m/%d %H:%M")); // seems Facebook does not record seconds info
    stringstream ss;
    ss.imbue(loc);

    vector<string> threads;
    sqlite::connection con(argv[1]);
    sqlite::query q(con, "SELECT DISTINCT thread FROM messages");
    boost::shared_ptr<sqlite::result> results = q.get_result();
    while(results->next_row())
    {
        string a_thread = results->get_string(0);
        threads.push_back(a_thread);
    }

    filesystem::path output_dir("output");
    filesystem::create_directory(output_dir);
    ofstream f;
    for(string a_thread : threads)
    {
        filesystem::path full_path = output_dir / filesystem::path(a_thread+".txt");
        f.open(full_path.string());
        sqlite::query q2(con, "SELECT timestamp,user,content FROM messages WHERE thread=? ORDER BY timestamp DESC");
        q2.bind(1, a_thread);
        boost::shared_ptr<sqlite::result> result2 = q2.get_result();
        while(result2->next_row())
        {
            int timestamp = result2->get_int(0);
            string user = result2->get_string(1);
            string content = result2->get_string(2);
            ss << posix_time::from_time_t(timestamp);
            f << ss.str() << "," << user << "," << content << "\n";
            ss.str(string());
        }
        f.close();
    }
    return 0;
}