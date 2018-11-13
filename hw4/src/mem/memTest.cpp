/****************************************************************************
  FileName     [ memTest.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test static data members and global variable ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include "memTest.h"

using namespace std;

#ifdef MEM_MGR_H
MEM_MGR_INIT(MemTestObj);   // initialize static data member
#endif // MEM_MGR_H

MemTest mtest;

