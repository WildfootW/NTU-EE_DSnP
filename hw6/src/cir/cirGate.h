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
    class related_gate;

    CirGate() {}
    CirGate(const unsigned int& vid, const unsigned int& lno, bool floating = false): _variable_id(vid), _line_no(lno), floating(floating), visited(0) {}
    CirGate(const unsigned int& vid, const unsigned int& lno, const vector<related_gate>& src, bool floating = false): _i_gate_list(src), _variable_id(vid), _line_no(lno), floating(floating), visited(0) {}
    virtual ~CirGate() {}

    string symbolic_name;
    static unsigned int visited_ref;
    void add_output_gate(const related_gate& gate_p) { _o_gate_list.push_back(gate_p); }
    void update_inputs_output_list(const related_gate& myself) const; // add myself to my input's output list
    bool is_floating() const { return floating; }
//    void set_floating();
    bool have_floating_fanin() const;
    bool not_used() const { return (gate_type() == PI_GATE || gate_type() == AIG_GATE) && _o_gate_list.empty(); }
    static void reset_visited() { ++visited_ref; }
    bool is_visited() const { return (visited == visited_ref); }
    void set_visited() const { visited = visited_ref; }

    // Basic access methods
    virtual GateType gate_type() const = 0;
    virtual string getTypeStr() const = 0;
    unsigned int getLineNo() const { return _line_no; }
    unsigned int getId() const { return _variable_id; }

    // Printing functions
    void print_net_dfs(unsigned int& print_line_no) const;
    virtual void print_net(unsigned int& print_line_no) const = 0;
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void report_dfs(const int& max_level, int level, const bool is_fanin, bool print_inverted) const;
    void report_print_gate() const { cout << getTypeStr() << " " << getId(); }

    class related_gate
    {
    public:
        related_gate(CirGate** p, bool inverted = false): gate_p(p), inverted(inverted) {}
        related_gate(): gate_p(NULL), inverted(false) {}

        CirGate* get_gate() const { return *gate_p; }
        bool is_inverted() const { return inverted; }

    private:
        friend class CirGate;

        CirGate** gate_p;
        bool inverted;
    };

protected:
    vector<related_gate> _i_gate_list;
    vector<related_gate> _o_gate_list;

private:
    unsigned int _variable_id;
    unsigned int _line_no;
    bool floating;
    mutable unsigned int visited;
};

class PIGate: public CirGate
{
public:
    PIGate(const unsigned int& vid, const unsigned int& lno): CirGate(vid, lno) {}

    GateType gate_type() const { return PI_GATE; }
    string getTypeStr() const { return "PI"; }

    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << getTypeStr();
        cout << getId();
        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const {
        cout << getTypeStr() << " " << getId();
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
    POGate(const unsigned int& vid, const unsigned int& lno, const vector<related_gate>& src): CirGate(vid, lno, src) {}

    GateType gate_type() const { return PO_GATE; }
    string getTypeStr() const { return "PO"; }

    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << getTypeStr();
        cout << getId();

        for(size_t i = 0;i < 1;++i)
        {
            cout << " ";
            if(_i_gate_list[i].get_gate()->gate_type() == UNDEF_GATE)
                cout << "*";
            if(_i_gate_list[i].is_inverted())
                cout << "!";
            cout << _i_gate_list[i].get_gate()->getId();
        }

        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const {
        cout << getTypeStr() << " " << getId();
        cout << " " << _i_gate_list[0].get_gate()->getId();
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
    AIGGate(const unsigned int& vid, const unsigned int& lno, const vector<related_gate>& src): CirGate(vid, lno, src) {}

    GateType gate_type() const { return AIG_GATE; }
    string getTypeStr() const { return "AIG"; }

    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << setw(4) << left << getTypeStr();
        cout << getId();

        for(size_t i = 0;i < 2;++i)
        {
            cout << " ";
            if(_i_gate_list[i].get_gate()->gate_type() == UNDEF_GATE)
                cout << "*";
            if(_i_gate_list[i].is_inverted())
                cout << "!";
            cout << _i_gate_list[i].get_gate()->getId();
        }

        if(!symbolic_name.empty())
            cout << " (" << symbolic_name << ")";
        cout << endl;
        ++print_line_no;
    }
    void printGate() const {
        cout << getTypeStr() << " " << getId();
        cout << " " << _i_gate_list[0].get_gate()->getId();
        cout << " " << _i_gate_list[1].get_gate()->getId();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};

class UNDEFGate: public CirGate
{
public:
    UNDEFGate(): CirGate(0, 0, true) {} // true for floating
    UNDEFGate(const unsigned int& vid): CirGate(vid, 0, true) {} // true for floating

    GateType gate_type() const { return UNDEF_GATE; }
    string getTypeStr() const { return "UNDEF"; }

    void print_net(unsigned int& print_line_no) const
    {
        return;
    }

    void printGate() const {
        cout << getTypeStr();
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

    GateType gate_type() const { return CONST_GATE; }
    string getTypeStr() const { return "CONST"; }

    void print_net(unsigned int& print_line_no) const
    {
        cout << "[" << print_line_no << "] ";
        cout << getTypeStr();
        cout << getId();
        cout << endl;
        ++print_line_no;
    }
    void printGate() const {
        cout << getTypeStr();
        cout << " related o: " << _o_gate_list.size();
        cout << " f: " << is_floating();
        cout << endl;
    }
private:
};


#endif // CIR_GATE_H
