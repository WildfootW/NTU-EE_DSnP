/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{
public:
    CirMgr();
    ~CirMgr();

    string comments;

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const;

    // Member functions about circuit construction
    bool readCircuit(const string&);

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream&) const;

private:
    GateList _gate_list;
    unsigned int _header_M;
    unsigned int _header_I;
    unsigned int _header_L;
    unsigned int _header_O;
    unsigned int _header_A;
    unsigned int _new_header_A; // for write, look at spec. it will be calc at confirm_circuit()
    CirGate* _dummy_udf_gate;
    IdList _pi_list;
    IdList _po_list;
    //IdList _latch_list;
    IdList _float_fanin_list;
    IdList _not_used_list;

    // Help function for read
    bool confirm_circuit(); // find floating, complete o_gate_list...
    bool read_symbol_parser(string input, CirGate*& target, string& symbolic_name) const;
    bool read_interger_parser(string input, vector<int>& tokens, unsigned int number_num) const;
    bool read_header_parser(const string& input, vector<int>& tokens) const;
    bool read_gate_parser(const string& input, vector<int>& tokens, GateType type) const;
    unsigned int literal_to_variable(int literal_id, bool& inverted) const;
    unsigned int literal_to_variable(int literal_id) const;
    void set_header(const vector<int>& tokens);
    void set_gate(const vector<int>& tokens, GateType type, unsigned int lno);

};

#endif // CIR_MGR_H
