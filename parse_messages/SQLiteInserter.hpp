#pragma once

#include <sqlite/connection.hpp>
#include <sqlite/command.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <string>
#include <vector>
using namespace std;

template<typename... Types>
class SQLiteInsertor
{
public:
    SQLiteInsertor(sqlite::connection& con, string sql, unsigned int length): con(con), buffer_size(length)
    {
        size_t pos = sql.find_last_of(' ');
        string head = sql.substr(0, pos+1);
        string tail = sql.substr(pos+1);
        insert = new sqlite::command(con, head + boost::algorithm::join(vector<string>(length, tail), ","));
    }

    ~SQLiteInsertor()
    {
        delete insert;
    }

    void push_data(Types... args)
    {
        data_buffer.push_back(boost::tuple<Types...>(args...));
        if(data_buffer.size() == buffer_size)
        {
            for(boost::tuple<Types...> item : data_buffer)
            {
                boost::fusion::for_each(item, [this] (auto& t) {
                    insert->operator%(t);
                });
            }
            insert->emit();
            insert->clear();
            data_buffer.clear();
        }
    }
private:
    sqlite::connection& con;
    sqlite::command* insert;
    vector<boost::tuple<Types...>> data_buffer;
    size_t buffer_size;
};

