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
#include "cirDef.h"

using namespace std;

class CirGate
{
public:
    class RelatedGate
    {
        friend class CirGate;
    public:
        RelatedGate(CirGate** p, bool inverted = false): gate_pp(p), inverted(inverted) {}
        RelatedGate(): gate_pp(NULL), inverted(false) {}

        CirGate* get_gate_p() const { return *gate_pp; }
        bool is_inverted() const { return inverted; }
        void set_inverted(bool new_inverted) { inverted = new_inverted; }

    private:
        CirGate** gate_pp;
        bool inverted;      // [TODO] store in LSB
    };
    using RelatedGateList = vector<RelatedGate>;

    CirGate() {}
    CirGate(const unsigned int& vid, const unsigned int& lno): _variable_id(vid), _line_no(lno), visited(0) {}
    CirGate(const unsigned int& vid, const unsigned int& lno, const RelatedGateList& src): _i_gate_list(src), _variable_id(vid), _line_no(lno), visited(0) {}
    virtual ~CirGate() {}

    string symbolic_name;

    // for various dfs function
    static void reset_visited() { ++visited_ref; }
    bool is_visited() const { return (visited == visited_ref); }
    void set_visited() const { visited = visited_ref; }

    // check status
    bool is_floating() const;
    bool is_not_using() const { return (get_type() == PI_GATE || get_type() == AIG_GATE) && _o_gate_list.empty(); }

    // Basic access methods
    virtual GateType get_type() const = 0;
    virtual string get_type_str() const = 0;
    unsigned int get_line_no() const { return _line_no; }
    unsigned int get_variable_id() const { return _variable_id; }

    // modify class member
    void add_output_gate(const RelatedGate& r_gate) { _o_gate_list.push_back(r_gate); }
    void add_input_gate (const RelatedGate& r_gate) { _i_gate_list.push_back(r_gate); }

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
    void printGate() const {
        cout << get_type_str() << " " << get_variable_id();
        if(!symbolic_name.empty())
            cout << " " << symbolic_name;
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};

class POGate: public CirGate
{
public:
    POGate(const unsigned int& vid, const unsigned int& lno, const RelatedGateList& src): CirGate(vid, lno, src) {}

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
    void printGate() const {
        cout << get_type_str() << " " << get_variable_id();
        cout << " " << _i_gate_list[0].get_gate_p()->get_variable_id();
        if(!symbolic_name.empty())
            cout << " " << symbolic_name;
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};

class AIGGate: public CirGate
{
public:
    AIGGate(const unsigned int& vid, const unsigned int& lno, const RelatedGateList& src): CirGate(vid, lno, src) {}

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
    void printGate() const {
        cout << get_type_str() << " " << get_variable_id();
        cout << " " << _i_gate_list[0].get_gate_p()->get_variable_id();
        cout << " " << _i_gate_list[1].get_gate_p()->get_variable_id();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};

class UNDEFGate: public CirGate
{
public:
    UNDEFGate(): CirGate(0, 0) {} // true for floating
    UNDEFGate(const unsigned int& vid): CirGate(vid, 0) {} // true for floating

    GateType get_type() const { return UNDEF_GATE; }
    string get_type_str() const { return "UNDEF"; }

    void print_net(unsigned int& print_line_no) const
    {
        return;
    }

    void printGate() const {
        cout << get_type_str();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }

private:
};

class CONSTGate: public CirGate
{
public:
    CONSTGate(): CirGate(0, 0) {}

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
    void printGate() const {
        cout << get_type_str();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};


#endif // CIR_GATE_H
