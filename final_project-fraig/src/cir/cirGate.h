/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "cirDef.h"
#include "sat.h"

using namespace std;

class CirGate
{
public:
    class RelatedGate
    {
        friend class CirGate;
    public:
        RelatedGate(): gate_p(NULL), inverted(false) {}
        RelatedGate(const CirGate* p, bool inverted = false): gate_p(const_cast<CirGate*>(p)), inverted(inverted) { }

        CirGate* get_gate_p() const { return gate_p; }
        bool is_inverted() const { return inverted; }
        void set_inverted(bool new_inverted) { inverted = new_inverted; }

    private:
        CirGate* gate_p;
        bool inverted;      // [TODO] store in LSB
    };
    using RelatedGateList = vector<RelatedGate>;    // [TODO] change to map

    CirGate() {}
    CirGate(const unsigned int& vid, const unsigned int& lno): _variable_id(vid), _line_no(lno), visited(0) {}
    CirGate(const unsigned int& vid,
            const unsigned int& lno,
            const RelatedGateList& i_list,
            const RelatedGateList& o_list): _i_gate_list(i_list), _o_gate_list(o_list), _variable_id(vid), _line_no(lno), visited(0) {}

    virtual ~CirGate() {}

    string symbolic_name;

    // for various dfs function
    static void reset_visited() { ++visited_ref; }
    bool is_visited() const { return (visited == visited_ref); }
    void set_visited() const { visited = visited_ref; }

    // check status
    bool is_floating() const;
    bool is_not_using() const { return (get_type() == PI_GATE || get_type() == AIG_GATE) && _o_gate_list.empty(); }
    virtual bool isAig() const { return false; }

    // Basic access methods
    virtual GateType get_type() const = 0;
    virtual string get_type_str() const = 0;
    unsigned int get_line_no() const { return _line_no; }
    unsigned int get_variable_id() const { return _variable_id; }
    const RelatedGateList& get_i_list() const { return _i_gate_list; }
    const RelatedGateList& get_o_list() const { return _o_gate_list; }

    // modify class member
    void add_related_gate(const bool is_input, const RelatedGate& rgate);
    void add_related_gate(const bool is_input, const bool inverted, CirGate* r_gate);
    void replace_related_gate(const bool is_input, CirGate* ori_gate_p, CirGate* new_gate_p);

    // for CIRWrite
    void write_aig_dfs(IdList& _aig_list);
    virtual void write_as_aag(ostream& outfile) const { cerr << "write_as_aag(ostream&) in CirGate class should not be called\n"; }
    virtual unsigned int write_get_input_gate_id() const { cerr << "write_get_input_gate_id() in CirGate class should not be called\n"; return 0; }; // for CirMgr::write

    // for CIRRead
    void read_add_to_inputs_o_list(RelatedGate myself) const; // add myself to my input's output list

    // for CIRPrint
    void print_net_dfs(unsigned int& print_line_no) const;
    virtual void print_net(unsigned int& print_line_no) const = 0;
    virtual void printGate() const = 0;

    // for CirGate
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void report_dfs(const int& max_level, int level, const bool is_fanin, bool print_inverted) const;
    void report_print_gate() const { cout << get_type_str() << " " << get_variable_id(); }

protected:
    RelatedGateList _i_gate_list;
    RelatedGateList _o_gate_list;

private:
    unsigned int _variable_id;
    unsigned int _line_no;
    mutable unsigned int visited;
    static unsigned int visited_ref;
};

class PIGate: public CirGate
{
public:
    PIGate(const unsigned int& vid, const unsigned int& lno): CirGate(vid, lno) {}
    PIGate(const unsigned int& vid,
           const unsigned int& lno,
           const RelatedGateList& i_list,
           const RelatedGateList& o_list): CirGate(vid, lno, i_list, o_list) {}

    GateType get_type() const { return PI_GATE; }
    string get_type_str() const { return "PI"; }

    void write_as_aag(ostream& outfile) const { outfile << get_variable_id() * 2 << '\n'; }
    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << get_type_str();
        cout << get_variable_id();
        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const
    {
#ifdef DEBUG
        cout << get_type_str() << " " << get_variable_id();
        if(!symbolic_name.empty())
            cout << " " << symbolic_name;
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
#endif
    }
private:
};

class POGate: public CirGate
{
public:
    POGate(const unsigned int& vid, const unsigned int& lno): CirGate(vid, lno) {}
    POGate(const unsigned int& vid,
           const unsigned int& lno,
           const RelatedGateList& i_list,
           const RelatedGateList& o_list): CirGate(vid, lno, i_list, o_list) {}

    GateType get_type() const { return PO_GATE; }
    string get_type_str() const { return "PO"; }

    void write_as_aag(ostream& outfile) const { outfile << _i_gate_list[0].get_gate_p()->get_variable_id() * 2 + (_i_gate_list[0].is_inverted() ? 1 : 0) << '\n'; }
    unsigned int write_get_input_gate_id() const { return _i_gate_list[0].get_gate_p()->get_variable_id(); } // for CirMgr::write
    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << get_type_str();
        cout << get_variable_id();

        for(size_t i = 0;i < 1;++i)
        {
            cout << " ";
            if(_i_gate_list[i].get_gate_p()->get_type() == UNDEF_GATE)
                cout << "*";
            if(_i_gate_list[i].is_inverted())
                cout << "!";
            cout << _i_gate_list[i].get_gate_p()->get_variable_id();
        }

        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const
    {
#ifdef DEBUG
        cout << get_type_str() << " " << get_variable_id();
        cout << " " << _i_gate_list[0].get_gate_p()->get_variable_id();
        if(!symbolic_name.empty())
            cout << " " << symbolic_name;
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
#endif
    }
private:
};

class AIGGate: public CirGate
{
public:
    AIGGate(const unsigned int& vid, const unsigned int& lno): CirGate(vid, lno) {}
    AIGGate(const unsigned int& vid,
            const unsigned int& lno,
            const RelatedGateList& i_list,
            const RelatedGateList& o_list): CirGate(vid, lno, i_list, o_list) {}

    GateType get_type() const { return AIG_GATE; }
    string get_type_str() const { return "AIG"; }

    void write_as_aag(ostream& outfile) const { outfile << get_variable_id() * 2 << ' '
                                                        << _i_gate_list[0].get_gate_p()->get_variable_id() * 2 + (_i_gate_list[0].is_inverted() ? 1 : 0) << ' '
                                                        << _i_gate_list[1].get_gate_p()->get_variable_id() * 2 + (_i_gate_list[1].is_inverted() ? 1 : 0) << '\n'; }
    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << get_type_str();
        cout << get_variable_id();

        for(size_t i = 0;i < 2;++i)
        {
            cout << " ";
            if(_i_gate_list[i].get_gate_p()->get_type() == UNDEF_GATE)
                cout << "*";
            if(_i_gate_list[i].is_inverted())
                cout << "!";
            cout << _i_gate_list[i].get_gate_p()->get_variable_id();
        }

        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const
    {
#ifdef DEBUG
        cout << get_type_str() << " " << get_variable_id();
        cout << " " << _i_gate_list[0].get_gate_p()->get_variable_id();
        cout << " " << _i_gate_list[1].get_gate_p()->get_variable_id();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
#endif
    }
private:
};

class UNDEFGate: public CirGate
{
public:
    UNDEFGate(const unsigned int& vid = 0, const unsigned int& lno = 0): CirGate(vid, lno) {}
    UNDEFGate(const unsigned int& vid,
              const unsigned int& lno,
              const RelatedGateList& i_list,
              const RelatedGateList& o_list): CirGate(vid, lno, i_list, o_list) {}

    GateType get_type() const { return UNDEF_GATE; }
    string get_type_str() const { return "UNDEF"; }

    void print_net(unsigned int& print_line_no) const {}

    void printGate() const
    {
#ifdef DEBUG
        cout << get_type_str();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
#endif
    }

private:
};

class CONSTGate: public CirGate
{
public:
    CONSTGate(const unsigned int& vid = 0, const unsigned int& lno = 0): CirGate(vid, lno) {}
    CONSTGate(const unsigned int& vid,
              const unsigned int& lno,
              const RelatedGateList& i_list,
              const RelatedGateList& o_list): CirGate(vid, lno, i_list, o_list) {}

    GateType get_type() const { return CONST_GATE; }
    string get_type_str() const { return "CONST"; }

    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << get_type_str();
        cout << get_variable_id();
        cout << endl;
        ++print_line_no;
    }
    void printGate() const
    {
#ifdef DEBUG
        cout << get_type_str();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
#endif
    }
private:
};


#endif // CIR_GATE_H
