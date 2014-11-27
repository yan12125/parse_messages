#include "MyInputSource.hpp"
#include "MyBinInputStream.hpp"

MyInputSource::MyInputSource(string filename): filename(filename)
{
}

BinInputStream* MyInputSource::makeStream() const
{
    return new MyBinInputStream(filename);
}
