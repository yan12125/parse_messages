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
    // static data
    vector<string> weekdays;
    vector<string> months;
    unordered_map<string, int> weekday_map;
    unordered_map<string, int> month_map;
    vector<regex> patterns;
    boost::posix_time::ptime epoch;

    int last_gmt_offset;
public:
    Util();
    long timestamp(string data);
    int get_last_gmt_offset() const
    {
        return last_gmt_offset;
    }
    static bool debug;
signals:
    void errorOccurred(string time_str);
    void finished();
};
