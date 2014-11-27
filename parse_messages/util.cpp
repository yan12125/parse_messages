#include "util.hpp"
#include <QMessageBox>
#include <vector>
#include <functional>
using namespace std;

vector<string> Util::weekdays = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
vector<string> Util::months = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
unordered_map<string, int> Util::weekday_map;
unordered_map<string, int> Util::month_map;
vector<regex> Util::patterns({
    regex("([A-Z][a-z]+), ([A-Z][a-z]+) (\\d{1,2}), (\\d{4}) at (\\d{1,2}):(\\d{2})([ap]m) UTC([+-]\\d{2})"),
    regex("(\\d{4})年(\\d{1,2})月(\\d{1,2})日 (\\d{1,2}):(\\d{2}) UTC([+-]\\d{2})")
});
boost::posix_time::ptime Util::epoch(boost::gregorian::date(1970,1,1));
Util::init Util::initializer;

long Util::timestamp(string data)
{
    vector<function<void(struct tm&, int&, const smatch&)>> handlers = {
        [] (struct tm& some_time, int& gmt_offset, const smatch& results) {
            some_time.tm_wday = weekday_map[results[1]];
            some_time.tm_mon = month_map[results[2]];
            some_time.tm_mday = stoi(results[3]);
            some_time.tm_year = stoi(results[4]) - 1900;
            int hour = stoi(results[5]);
            some_time.tm_hour = (hour != 12?hour:0) + (results[7] == "pm"?12:0); // 12:00am => 0:00 and 12:00pm => 12:00
            some_time.tm_min = stoi(results[6]);
            gmt_offset = stoi(results[8]);
        },
        [] (struct tm& some_time, int& gmt_offset, const smatch& results) {
            some_time.tm_year = stoi(results[1]) - 1900;
            some_time.tm_mon = stoi(results[2]);
            some_time.tm_mday = stoi(results[3]);
            some_time.tm_hour = stoi(results[4]);
            some_time.tm_min = stoi(results[5]);
            gmt_offset = stoi(results[6]);
        }
    };
    smatch results;
    struct tm some_time;
    int gmt_offset = 0;
    bool parsed = false;
    for(unsigned int i = 0; i < patterns.size(); i++)
    {
        if(regex_match(data, results, patterns[i]))
        {
            memset(&some_time, 0, sizeof(some_time));
            handlers[i](some_time, gmt_offset, results);
            parsed = true;
            break;
        }
    }
    if(!parsed)
    {
        QMessageBox::critical(nullptr, "Fatal error", "Unable to parse time string");
    }

    // calculating
    return (boost::posix_time::ptime_from_tm(some_time) - epoch).total_seconds() - gmt_offset*3600;
}
