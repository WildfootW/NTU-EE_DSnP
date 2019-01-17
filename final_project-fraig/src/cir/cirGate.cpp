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
    times = 80;

    cout << string(times, '=') << endl;
    cout << "= "  << left << setw(times - 4) << ss.str() << endl;
    cout << "= FECs:" << endl;
    cout << "= Value: 00000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000" << endl;
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
void CirGate::append_related_gate_list(const bool is_input, const RelatedGateList& rgate_list, bool reverse) // one way
{
    RelatedGateList& the_list = (is_input ? _i_gate_list : _o_gate_list);
    for(auto rgate:rgate_list)
    {
        if(reverse)
            rgate.set_inverted_reverse();
        the_list.push_back(rgate);
    }
}
void CirGate::add_related_gate(const bool is_input, const RelatedGate& rgate) // double side
{
    if(is_input)
    {
        rgate.get_gate_p()->_o_gate_list.push_back( RelatedGate(this, rgate.is_inverted()) );
        _i_gate_list.push_back(rgate);
    }
    else
    {
        rgate.get_gate_p()->_i_gate_list.push_back( RelatedGate(this, rgate.is_inverted()) );
        _o_gate_list.push_back(rgate);
    }
}
void CirGate::add_related_gate(const bool is_input, const bool inverted, CirGate* r_gate) { add_related_gate(is_input, RelatedGate(r_gate, inverted)); }
void CirGate::replace_self_in_related_gates(const CirGate* new_gate_p) const
{
    for(auto it = _i_gate_list.begin(); it != _i_gate_list.end();++it)
    {
        CirGate* r_gate = (*it).get_gate_p();
        RelatedGate new_relation(new_gate_p, (*it).is_inverted());
        r_gate->replace_relation(false, this, new_relation);
    }
    for(auto it = _o_gate_list.begin(); it != _o_gate_list.end();++it)
    {
        CirGate* r_gate = (*it).get_gate_p();
        RelatedGate new_relation(new_gate_p, (*it).is_inverted());
        r_gate->replace_relation(true , this, new_relation);
    }
}
void CirGate::replace_self_in_related_gates(bool is_input, const RelatedGate& new_relation) const
{
    if(is_input)
    {
        for(auto it = _i_gate_list.begin(); it != _i_gate_list.end();++it)
        {
            RelatedGate new_relation_for_this_relatedgate = new_relation;
            if((*it).is_inverted()) // if origin relationship is inverted, reverse it
                new_relation_for_this_relatedgate.set_inverted_reverse();
            CirGate* r_gate = (*it).get_gate_p();
            r_gate->replace_relation(false, this, new_relation_for_this_relatedgate);
        }
    }
    else
    {
        for(auto it = _o_gate_list.begin(); it != _o_gate_list.end();++it)
        {
            RelatedGate new_relation_for_this_relatedgate = new_relation;
            if((*it).is_inverted()) // if origin relationship is inverted, reverse it
                new_relation_for_this_relatedgate.set_inverted_reverse();
            CirGate* r_gate = (*it).get_gate_p();
            r_gate->replace_relation(true , this, new_relation_for_this_relatedgate);
        }
    }
}
void CirGate::replace_relation(const bool in_i_gate_list, const CirGate* ori_gate_p, const RelatedGate& new_relation)
{
    RelatedGateList& the_list = (in_i_gate_list ? _i_gate_list : _o_gate_list);
    for(auto it = the_list.begin(); it != the_list.end();++it)
    {
        if((*it).get_gate_p() == ori_gate_p)
        {
            if(new_relation.get_gate_p()->get_type() == UNDEF_GATE)
                the_list.erase(it);
            else
                (*it) = new_relation;
            break;
        }
    }
}

ostream& operator << (ostream& os, const CirGate::RelatedGate& rgate)
{
    if(rgate.is_inverted())
        os << "!";
    os << rgate.get_gate_p()->get_type_str();
    os << "(" << rgate.get_gate_p()->get_variable_id() << ")";
    return os;
}

