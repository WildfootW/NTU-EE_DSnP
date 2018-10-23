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

   // TODO
   // Insert what you want to test here by calling DBJson's member functions
    size_t db_size;
    cout << "max: " << dbjson.max(db_size) << endl;
    cout << "min: " << dbjson.min(db_size) << endl;
    cout << "sum: " << dbjson.sum() << endl;
    cout << "ave: " << fixed << setprecision(2) << dbjson.ave() << endl;

    cout << "sort by key:\n";
    DBSortKey sortkey;
    dbjson.sort(sortkey);
    cout << dbjson << endl;

    cout << "sort by value:\n";
    DBSortValue sortvalue;
    dbjson.sort(sortvalue);
    cout << dbjson << endl;

    return 0;
}
