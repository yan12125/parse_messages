#ifndef _util_hpp
#define _util_hpp
#include <unordered_map>
#include <string>
#include <regex>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/locale/time_zone.hpp>
using namespace std;

class Util
{
private:
    static vector<string> weekdays;
    static vector<string> months;
    static unordered_map<string, int> weekday_map;
    static unordered_map<string, int> month_map;
    static regex pattern;
    static boost::posix_time::ptime epoch;
    static class init
    {
    public:
        init()
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
    } initializer;
public:
    static long timestamp(string data);
};

#endif
