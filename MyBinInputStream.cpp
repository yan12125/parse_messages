#include "MyBinInputStream.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>
using namespace std;

MyBinInputStream::MyBinInputStream(string filename)
{
    file.open(filename, ios::in|ios::binary);
    _curPos = file.tellg();
}

XMLFilePos MyBinInputStream::curPos() const
{
    return _curPos;
}

XMLSize_t MyBinInputStream::readBytes( XMLByte* const toFill , const XMLSize_t maxToRead)
{
    char* buf = new char[maxToRead+1];
    file.read(buf, maxToRead);
    ssize_t len = file.gcount();
    for(int i = 0; i < len; i++)
    {
        if(0 <= buf[i] && buf[i] < ' ' && buf[i] != '\t' && buf[i] != '\r' && buf[i] != '\n') // invalid
        {
            cerr << "Invalid char detected: 0x" << setfill('0') << setw(2) << hex << static_cast<unsigned short>(buf[i]) << endl;
            toFill[i] = ' ';
        }
        else
        {
            toFill[i] = buf[i];
        }
    }
    delete [] buf;
    _curPos = file.tellg();
    return len;
}

const XMLCh* MyBinInputStream::getContentType() const
{
    return NULL;
}
