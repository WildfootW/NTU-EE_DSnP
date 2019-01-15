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

    for(const RelatedGate& e:_i_gate_list)
    {
        e.get_gate_p()->print_net_dfs(print_line_no);
    }
    print_net(print_line_no);
}
void
CirGate::reportGate() const
{
    stringstream ss;
    ss << get_type_str() << "(" << get_variable_id() << ")";
    if(!symbolic_name.empty())
        ss << "\"" << symbolic_name << "\"";
    ss << ", line " << get_line_no();

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
    const RelatedGateList& recursive_list = (is_fanin ? _i_gate_list : _o_gate_list);

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

    for(const RelatedGate& e:recursive_list)
    {
        e.get_gate_p()->report_dfs(max_level, level - 1, is_fanin, e.is_inverted());
    }
}

void CirGate::write_aig_dfs(IdList& _aig_list)
{
    if(is_visited() || (get_type() != AIG_GATE))
        return;
    set_visited();
    for(const RelatedGate& e:_i_gate_list)
    {
        e.get_gate_p()->write_aig_dfs(_aig_list);
    }
    _aig_list.push_back(get_variable_id());
}

void
CirGate::read_add_to_inputs_o_list(RelatedGate myself) const
{
    for(const RelatedGate& e:_i_gate_list)
    {
        myself.set_inverted(e.is_inverted());
        e.get_gate_p()->add_output_gate(myself);
    }
}

bool
CirGate::is_floating() const
{
    for(const RelatedGate& e:_i_gate_list)
    {
        if(e.get_gate_p()->get_type() == UNDEF_GATE)
            return true;
    }
    return false;
}

