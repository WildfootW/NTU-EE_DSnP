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

unsigned int CirGate::visited_ref = 0;
/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::print_net_dfs(unsigned int& print_line_no) const
{
    if(is_visited())
        return;
    set_visited();

    for(const related_gate& e:_i_gate_list)
    {
        e.get_gate()->print_net_dfs(print_line_no);
    }
    print_net(print_line_no);
}
void
CirGate::reportGate() const
{
    stringstream ss;
    ss << getTypeStr() << "(" << getId() << ")";
    if(!symbolic_name.empty())
        ss << "\"" << symbolic_name << "\"";
    ss << ", line " << getLineNo();

    unsigned int times = ss.str().size() + 4;
    //times = max(times, (unsigned int)50);
    times = 50;

    cout << string(times, '=') << endl;
    cout << "= "  << left << setw(times - 4) << ss.str() << " =" << endl;
    cout << string(times, '=') << endl;
}

void
CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    reset_visited();
    report_dfs(level, level, true, false);
}

void
CirGate::reportFanout(int level) const
{
    assert (level >= 0);
    reset_visited();
    report_dfs(level, level, false, false);
}

void
CirGate::report_dfs(const int& max_level, int level, const bool is_fanin, bool print_inverted) const
{
    const vector<related_gate>& recursive_list = (is_fanin ? _i_gate_list : _o_gate_list);

    cout << string((max_level - level) * 2, ' ');
    if(print_inverted)
        cout << '!';
    report_print_gate();

    if(level == 0)
    {
        cout << endl;
        return;
    }
    if(is_visited() && !recursive_list.empty())
    {
        cout << " (*)" << endl;
        return;
    }
    set_visited();
    cout << endl;

    for(const related_gate& e:recursive_list)
    {
        e.get_gate()->report_dfs(max_level, level - 1, is_fanin, e.is_inverted());
    }
}

void
CirGate::update_inputs_output_list(related_gate myself) const
{
    for(const related_gate& e:_i_gate_list)
    {
        //if(e.get_gate()->gate_type() == UNDEF_GATE)
        //    continue;
        myself.set_inverted(e.is_inverted());
        e.get_gate()->add_output_gate(myself);
    }
}

//void
//CirGate::set_floating()
//{
//    floating = true;
//    for(related_gate& e:_o_gate_list)
//    {
//        if(e.get_gate()->is_floating() == false)
//            e.get_gate()->set_floating();
//    }
//}
bool
CirGate::have_floating_fanin() const
{
    for(const related_gate& e:_i_gate_list)
    {
        if(e.get_gate()->is_floating())
            return true;
    }
    return false;
}

