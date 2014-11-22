#pragma once

#include <xercesc/sax/InputSource.hpp>
#include <string>
using namespace xercesc;
using namespace std;

class MyInputSource : public InputSource
{
public:
    MyInputSource(string filename);
    virtual BinInputStream* makeStream() const;
private:
    string filename;
};
