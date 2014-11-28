#include "util.hpp"
#include <vector>
#include <functional>
#include <stdexcept>
using namespace std;

long Util::timestamp(string data)
{
    static vector<function<void(struct tm&, int&, const smatch&)>> handlers = {
        [this] (struct tm& some_time, int& gmt_offset, const smatch& results) {
            some_time.tm_wday = weekday_map[results[1]];
            some_time.tm_mon = month_map[results[2]];
            some_time.tm_mday = stoi(results[3]);
            some_time.tm_year = stoi(results[4]) - 1900;
            int hour = stoi(results[5]);
            some_time.tm_hour = (hour != 12?hour:0) + (results[7] == "pm"?12:0); // 12:00am => 0:00 and 12:00pm => 12:00
            some_time.tm_min = stoi(results[6]);
            gmt_offset = stoi(results[8]);
        },
        [this] (struct tm& some_time, int& gmt_offset, const smatch& results) {
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
        throw domain_error("Failed to parse time string: " +  data);
    }

    // calculating
    return (boost::posix_time::ptime_from_tm(some_time) - epoch).total_seconds() - gmt_offset*3600;
}
