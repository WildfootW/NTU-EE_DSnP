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
                CirGate** ori_gate = &_gate_list[i];
                CirGate*  new_gate = new UNDEFGate(i);
                (*ori_gate)->replace_self_in_related_gates(new_gate);
                delete (*ori_gate);
                (*ori_gate) = new_gate;
                cout << "Sweeping: AIG(" << i << ") removed...\n";
                --_header_A;
                complete = false;
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
    bool complete = false;
    while(!complete)
    {
        complete = true;
        IdList dfs_list;
        CirGate::reset_visited();
        for(auto &e:_po_list)
            _gate_list[e]->get_dfs_list(dfs_list);
        for(auto &e:dfs_list)
        {
            if(_gate_list[e]->get_type() != AIG_GATE)
                continue;

            CirGate** ori_gate = &_gate_list[e];
            CirGate::RelatedGate new_relation;

            CirGate::RelatedGate ia = (*ori_gate)->get_i_list()[0];
            CirGate::RelatedGate ib = (*ori_gate)->get_i_list()[1];
            if(ib.get_gate_p()->get_type() == CONST_GATE)
                swap(ia, ib);

            if(ia.get_gate_p() == ib.get_gate_p())
            {
                if(ia.is_inverted() == ib.is_inverted()) // type c
                {
                    if(ia.is_inverted())
                        new_relation = CirGate::RelatedGate(ia.get_gate_p(), true);
                    else
                        new_relation = CirGate::RelatedGate(ia.get_gate_p(), false);
                }
                else                                     // type d
                    new_relation = CirGate::RelatedGate(_gate_list[0], false); // constant false
            }
            else if(ia.get_gate_p()->get_type() == CONST_GATE)
            {
                if(ia.is_inverted()) // type a
                    new_relation = CirGate::RelatedGate(ib.get_gate_p(), false);
                else // type b
                    new_relation = CirGate::RelatedGate(_gate_list[0], false); // constant false
            }
            else
            {
                continue;
            }
            (*ori_gate)->replace_self_in_related_gates(false, new_relation);
            delete (*ori_gate);
            (*ori_gate) = new UNDEFGate(e);
            cout << "Simplifying: " << new_relation.get_gate_p()->get_variable_id() << " merging ";
            if(new_relation.is_inverted())
                cout << "!";
            cout << e << "...\n";
            --_header_A;
            complete = false;
        }
    }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
