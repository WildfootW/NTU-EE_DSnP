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
#include <array>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
    CirGate() {}
    CirGate(unsigned int vid, unsigned int lno): _variable_id(vid), _line_no(lno) {}
    virtual ~CirGate() {}

    // Basic access methods
    virtual GateType gate_type() const = 0;
    virtual string getTypeStr() const = 0;
    unsigned int getLineNo() const { return _line_no; }
    unsigned int getId() const { return _variable_id; }

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;

    class related_gate
    {
    public:
        related_gate(CirGate** p, bool inverted): gate_p(p), inverted(inverted) {}
        related_gate(): gate_p(NULL), inverted(false) {}

        CirGate* get_gate() const { return *gate_p; }
        bool is_inverted() const { return inverted; }

    private:
        friend class CirGate;

        CirGate** gate_p;
        bool inverted;
    };

protected:
private:
    unsigned int _variable_id;
    unsigned int _line_no;
};

class PIGate: public CirGate
{
public:
    PIGate(unsigned int vid, unsigned int lno): CirGate(vid, lno) {}

    GateType gate_type() const { return PI_GATE; }
    string getTypeStr() const { return "PI"; }

    void printGate() const { cout << getTypeStr() << " " << getId() << endl; }
private:
};

class UNDEFGate: public CirGate
{
public:
    UNDEFGate(): CirGate(0, 0) {
    }

    GateType gate_type() const { return UNDEF_GATE; }
    string getTypeStr() const { return "UNDEF"; }

    void printGate() const {
        cout << getTypeStr();
        cout << endl;
    }
private:
};

class CONSTGate: public CirGate
{
public:
    CONSTGate(): CirGate(0, 0) {
    }

    GateType gate_type() const { return CONST_GATE; }
    string getTypeStr() const { return "CONST"; }

    void printGate() const {
        cout << getTypeStr();
        cout << endl;
    }
private:
};

class POGate: public CirGate
{
public:
    POGate(unsigned int vid, unsigned int lno, const vector<related_gate>& src): CirGate(vid, lno) {
        for(size_t i = 0;i < src_gate.size();++i)
            src_gate[i] = src[i];
    }

    GateType gate_type() const { return PO_GATE; }
    string getTypeStr() const { return "PO"; }

    void printGate() const {
        cout << getTypeStr() << " " << getId();
        cout << " " << src_gate[0].get_gate()->getId();
        cout << endl;
    }
private:
    array<related_gate, 1> src_gate;
};

class AIGGate: public CirGate
{
public:
    AIGGate(unsigned int vid, unsigned int lno, const vector<related_gate>& src): CirGate(vid, lno) {
        for(size_t i = 0;i < src_gate.size();++i)
            src_gate[i] = src[i];
    }

    GateType gate_type() const { return AIG_GATE; }
    string getTypeStr() const { return "AIG"; }

    void printGate() const {
        cout << getTypeStr() << " " << getId();
        cout << " " << src_gate[0].get_gate()->getId();
        cout << " " << src_gate[1].get_gate()->getId();
        cout << endl;
    }
private:
    array<related_gate, 2> src_gate;
};

#endif // CIR_GATE_H
