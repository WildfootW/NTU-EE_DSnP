/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    bool complete = false;
    while(!complete)
    {
        complete = true;
        for(unsigned int i = 0;i < _gate_list.size();++i)
        {
            if(_gate_list[i]->is_not_using() && _gate_list[i]->get_type() == AIG_GATE)
            {
                complete = false;
                CirGate** ori_gate = &_gate_list[i];
                CirGate*  new_gate = new UNDEFGate(i);
                (*ori_gate)->replace_self_in_related_gates(new_gate);
                delete (*ori_gate);
                (*ori_gate) = new_gate;
                cout << "Sweeping: AIG(" << i << ") removed...\n";
                --_header_A;
            }
        }
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
