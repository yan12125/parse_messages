#pragma once

#include <unordered_map>
#include <string>
#include <regex>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/locale/time_zone.hpp>
#include <QObject>
using namespace std;

class Util: public QObject
{
    Q_OBJECT
private:
    vector<string> weekdays;
    vector<string> months;
    unordered_map<string, int> weekday_map;
    unordered_map<string, int> month_map;
    vector<regex> patterns;
    boost::posix_time::ptime epoch;
public:
    Util():
        weekdays({ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }), 
        months({ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }), 
        patterns({
            regex("([A-Z][a-z]+), ([A-Z][a-z]+) (\\d{1,2}), (\\d{4}) at (\\d{1,2}):(\\d{2})([ap]m) UTC([+-]\\d{2})"),
            regex("(\\d{4})年(\\d{1,2})月(\\d{1,2})日 (\\d{1,2}):(\\d{2}) UTC([+-]\\d{2})")
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
    long timestamp(string data);
signals:
    void errorOccurred(string time_str);
    void finished();
};
