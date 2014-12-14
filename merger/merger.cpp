#include "MergerDialog.hpp"
#include <QApplication>
#include <vector>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    vector<string> positionalArgs;
    string output_file;
    if(argc >= 2)
    {
        output_file = argv[1];
    }
    for(int i = 2; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            positionalArgs.push_back(argv[i]);
        }
    }

    QApplication a(argc, argv);
    MergerDialog lw;
    if(!positionalArgs.empty())
    {
        lw.setFiles(positionalArgs, output_file);
    }
    lw.resize(400, 200);
    lw.show();
    return a.exec();
}
