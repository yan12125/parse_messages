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
using namespace boost;

template<typename... Types>
class SQLiteInsertor
{
public:
    SQLiteInsertor(sqlite::connection& con, string sql, unsigned int length): con(con), buffer_size(length)
    {
        size_t pos = sql.find_last_of(' ');
        head = sql.substr(0, pos+1);
        tail = sql.substr(pos+1);
        insert = new sqlite::command(con, head + algorithm::join(vector<string>(length, tail), ","));
    }

    ~SQLiteInsertor()
    {
        flush_data(true);
    }

    void push_data(Types... args)
    {
        data_buffer.push_back(boost::tuple<Types...>(args...));
        if(data_buffer.size() == buffer_size)
        {
            flush_data(false);
        }
    }
private:
    void flush_data(bool isTail)
    {
        sqlite::command* insert_real = nullptr;
        bool hasData = !data_buffer.empty();

        if(isTail)
        {
            delete insert;
            insert = nullptr;
            if(hasData)
            {
                insert_real = new sqlite::command(con, head + algorithm::join(vector<string>(data_buffer.size(), tail), ","));
            }
        }
        else
        {
            insert_real = insert;
        }
        for(boost::tuple<Types...> item : data_buffer)
        {
            boost::fusion::for_each(item, [this, insert_real] (auto& t) {
                insert_real->operator%(t);
            });
        }
        if(hasData)
        {
            insert_real->emit();
            insert_real->clear();
        }
        data_buffer.clear();

        if(isTail)
        {
            delete insert_real;
        }
    }
    sqlite::connection& con;
    sqlite::command* insert;
    vector<boost::tuple<Types...>> data_buffer;
    size_t buffer_size;
    // re-generating SQL queries
    string head;
    string tail;
};

