#include <xercesc/util/BinInputStream.hpp>
#include <string>
#include <fstream>
using namespace xercesc;
using namespace std;

class MyBinInputStream: public BinInputStream
{
public:
    MyBinInputStream(string filename);
    virtual XMLFilePos curPos() const;
    virtual XMLSize_t readBytes(XMLByte* const toFill , const XMLSize_t maxToRead);
    virtual const XMLCh* getContentType() const;
private:
    ifstream file;
    XMLFilePos _curPos;
};
