/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include "dbJson.h"

using namespace std;

extern DBJson dbjson;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

void simple_test();

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <jsonfile>
      cerr << "Error: using testdb <jsonfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbjson) {
      cout << "Table is resetting..." << endl;
      dbjson.reset();
   }
   if (!(inf >> dbjson)) {
      cerr << "Error in reading JSON file!!" << endl;
      exit(-1);
   }

    cout << "========================" << endl;
    cout << " Print JSON object" << endl;
    cout << "========================" << endl;

    cout << dbjson << endl;
    simple_test();

    cout << "\nsort by key:\n";
    DBSortKey sortkey;
    dbjson.sort(sortkey);
    cout << dbjson << endl;
    simple_test();

    cout << "\nsort by value:\n";
    DBSortValue sortvalue;
    dbjson.sort(sortvalue);
    cout << dbjson << endl;
    simple_test();

    return 0;
}

void simple_test()
{
    size_t idx;
    cout << "max: " << setw(10) << dbjson.max(idx);
    cout << " idx: " << idx << endl;
    cout << "min: " << setw(10) << dbjson.min(idx);
    cout << " idx: " << idx << endl;
    cout << "sum: " << setw(10) << dbjson.sum() << endl;
    cout << "ave: " << setw(10) << fixed << setprecision(2) << dbjson.ave() << endl;
}
