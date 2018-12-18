/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    stringstream ss;
    ss << getTypeStr() << "(" << getId() << ")";
    if(!symbolic_name.empty())
        ss << "\"" << symbolic_name << "\"";
    ss << ", line " << getLineNo();

    unsigned int times = ss.str().size() + 4;
    times = max(times, (unsigned int)50);

    cout << string(times, '=') << endl;
    cout << "= "  << left << setw(times - 4) << ss.str() << " =" << endl;
    cout << string(times, '=') << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

