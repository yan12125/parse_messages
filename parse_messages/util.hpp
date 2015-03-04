#pragma once

#include <unordered_map>
#include <string>
#include <regex>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/locale/time_zone.hpp>
#include <QObject>

class Util: public QObject
{
    Q_OBJECT
private:
    // static data
    std::vector<std::string> weekdays;
    std::vector<std::string> months;
    std::unordered_map<std::string, int> weekday_map;
    std::unordered_map<std::string, int> month_map;
    std::vector<std::regex> patterns;
    boost::posix_time::ptime epoch;

    int last_gmt_offset;
public:
    Util();
    long timestamp(std::string data);
    int get_last_gmt_offset() const
    {
        return last_gmt_offset;
    }
    static bool debug;
signals:
    void errorOccurred(std::string time_str);
    void finished();
};
