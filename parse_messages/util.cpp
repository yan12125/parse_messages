#include "util.hpp"
#include <vector>
#include <functional>
#include <stdexcept>

#ifdef MY_DEBUG
bool Util::debug = true;
#else
bool Util::debug = false;
#endif

Util::Util():
    weekdays({ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }), 
    months({ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }), 
    patterns({
        std::regex("^([A-Z][a-z]+), ([A-Z][a-z]+) (\\d{1,2}), (\\d{4}) at (\\d{1,2}):(\\d{2})([ap]m) UTC([+-]\\d{2})"), 
        std::regex("^(\\d{4})年(\\d{1,2})月(\\d{1,2})日 (\\d{1,2}):(\\d{2}) UTC([+-]\\d{2})")
    }), 
    epoch(boost::gregorian::date(1970,1,1))
{
    boost::locale::time_zone::global("GMT");
    for(size_t i = 0; i < weekdays.size(); i++)
    {
        weekday_map[weekdays[i]] = i;
    }
    for(size_t i = 0; i < months.size(); i++)
    {
        month_map[months[i]] = i;
    }
}

long Util::timestamp(std::string data)
{
    static std::vector<std::function<void(struct tm&, int&, const std::smatch&)>> handlers = {
        [this] (struct tm& some_time, int& gmt_offset, const std::smatch& results) {
            some_time.tm_wday = weekday_map[results[1]];
            some_time.tm_mon = month_map[results[2]];
            some_time.tm_mday = stoi(results[3]);
            some_time.tm_year = stoi(results[4]) - 1900;
            int hour = stoi(results[5]);
            some_time.tm_hour = (hour != 12?hour:0) + (results[7] == "pm"?12:0); // 12:00am => 0:00 and 12:00pm => 12:00
            some_time.tm_min = stoi(results[6]);
            gmt_offset = stoi(results[8]);
        },
        [this] (struct tm& some_time, int& gmt_offset, const std::smatch& results) {
            some_time.tm_year = stoi(results[1]) - 1900;
            some_time.tm_mon = stoi(results[2]) - 1;
            some_time.tm_mday = stoi(results[3]);
            some_time.tm_hour = stoi(results[4]);
            some_time.tm_min = stoi(results[5]);
            gmt_offset = stoi(results[6]);
        }
    };
    std::smatch results;
    struct tm some_time;
    bool parsed = false;
    for(unsigned int i = 0; i < patterns.size(); i++)
    {
        if(std::regex_search(data, results, patterns[i]))
        {
            memset(&some_time, 0, sizeof(some_time));
            handlers[i](some_time, last_gmt_offset, results);
            parsed = true;
            break;
        }
    }
    if(!parsed)
    {
        throw std::domain_error("Failed to parse time string: " +  data);
    }

    if(debug)
    {
        std::cout << "year = " << some_time.tm_year
             << ", month = " << some_time.tm_mon
             << ", day = " << some_time.tm_mday << "\n"
             << "hour = " << some_time.tm_hour
             << ", minute = " << some_time.tm_min
             << ", gmt_offset = " << last_gmt_offset << "\n";
    }

    // calculating
    return (boost::posix_time::ptime_from_tm(some_time) - epoch).total_seconds() - last_gmt_offset*3600;
}
