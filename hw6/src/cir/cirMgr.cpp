/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/* access */
CirGate* CirMgr::getGate(unsigned gid) const
{
//    if(_gate_list[gid]->gate_type() == UNDEF_GATE)
//        return NULL;
    return _gate_list[gid];
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::CirMgr(): _new_header_A(0)
{
    _dummy_udf_gate = new UNDEFGate;
}
CirMgr::~CirMgr()
{
    for(size_t i = 0;i < _gate_list.size();++i)
    {
        if(_gate_list[i] != _dummy_udf_gate)
            delete _gate_list[i];
    }
    delete _dummy_udf_gate;
}
bool
CirMgr::readCircuit(const string& fileName)
{
    ifstream aag_file(fileName);
    if(!aag_file.is_open())
    {
        aag_file.close();
        cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
        return false;
    }

    vector<int> tokens;
    string input_line;
    unsigned int line_no = 0;

    // header
    //clog << "READ HEADER" << endl;
    getline(aag_file, input_line);
    if(!read_header_parser(input_line, tokens)) // TODO error
    {
        return false;
    }
    ++line_no;
    set_header(tokens);

    // Inputs
    //clog << "READ INPUTS" << endl;
    for(unsigned int i = 0;i < _header_I;++i)
    {
        tokens.clear();
        getline(aag_file, input_line);
        if(!read_gate_parser(input_line, tokens, PI_GATE)) // TODO error
        {
            return false;
        }
        ++line_no;
        set_gate(tokens, PI_GATE, line_no);
    }

    // Latches
    // no implement

    // Outputs
    //clog << "READ OUTPUTS" << endl;
    for(unsigned int i = 0;i < _header_O;++i)
    {
        tokens.clear();
        getline(aag_file, input_line);
        if(!read_gate_parser(input_line, tokens, PO_GATE)) // TODO error
        {
            return false;
        }
        ++line_no;
        tokens.push_back(_header_M + i + 1); // origin output tokens don't know PO's variable ID
        set_gate(tokens, PO_GATE, line_no);
    }

    // ANDs
    //clog << "READ ANDS" << endl;
    for(unsigned int i = 0;i < _header_A;++i)
    {
        tokens.clear();
        getline(aag_file, input_line);
        if(!read_gate_parser(input_line, tokens, AIG_GATE)) // TODO error
        {
            return false;
        }
        ++line_no;
        set_gate(tokens, AIG_GATE, line_no);
    }

    // Symbols
    while(getline(aag_file, input_line) && (input_line[0] == 'i' || input_line[0] == 'o'))
    {
        CirGate* target;
        string symbolic_name;
        read_symbol_parser(input_line, target, symbolic_name);
        target->symbolic_name = symbolic_name;
    }

    // Comments
    if(input_line[0] == 'c' && input_line.size() == 1)
    {
        while(getline(aag_file, input_line))
        {
            comments.append(input_line + '\n');
        }
    }

    aag_file.close();
    confirm_circuit();
    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout << endl;
    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << "  PI   " << setw(9) << right << _header_I << endl;
    cout << "  PO   " << setw(9) << right << _header_O << endl;
    cout << "  AIG  " << setw(9) << right << _header_A << endl;
    cout << "------------------" << endl;
    cout << "  Total" << setw(9) << right << _header_I + _header_O + _header_A << endl;

// for debug
//    for(size_t i = 0;i < _gate_list.size();++i)
//    {
//        cout << i << ": ";
//        _gate_list[i]->printGate();
//    }
//    cout << "Comments: " << endl;
//    cout << comments << endl;
}

void
CirMgr::printNetlist() const
{
    unsigned int print_line_no = 0;
    CirGate::reset_visited();
    cout << endl;
    for(auto e:_po_list)
    {
        _gate_list[e]->print_net_dfs(print_line_no);
    }
}

void
CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for(auto& e:_pi_list)
        cout << " " << e;
    cout << endl;
}

void
CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    for(auto& e:_po_list)
        cout << " " << e;
    cout << endl;
}

void
CirMgr::printFloatGates() const
{
    cout << "Gates with floating fanin(s):";
    for(auto& e:_float_fanin_list)
        cout << " " << e;
    cout << endl;

    cout << "Gates defined but not used  :";
    for(auto& e:_not_used_list)
        cout << " " << e;
    cout << endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
}

// Help function for read
bool CirMgr::confirm_circuit()
{
    for(unsigned int i = 0;i < _gate_list.size();++i)
    {
        CirGate::related_gate myself(&_gate_list[i]);
        _gate_list[i]->update_inputs_output_list(myself);
    }
//    for(unsigned int i = 0;i < _gate_list.size();++i)
//    {
//        if(_gate_list[i]->is_floating())
//            _gate_list[i]->set_floating();
//    }
    for(unsigned int i = 0;i < _gate_list.size();++i)
    {
        if(_gate_list[i]->have_floating_fanin())
            _float_fanin_list.push_back(i);
        if(_gate_list[i]->not_used())
            _not_used_list.push_back(i);
        if(_gate_list[i]->gate_type() == UNDEF_GATE)
            _gate_list[i] = new UNDEFGate(i);
    }
    return true;
}
bool CirMgr::read_symbol_parser(string input, CirGate*& target, string& symbolic_name) const
{
    stringstream ss(input);
    char ch;
    unsigned int position;
    ss.get(ch) >> position;

    if(ch == 'i')
    {
        target = _gate_list[_pi_list[position]];
    }
    else if(ch == 'o')
    {
        target = _gate_list[_po_list[position]];
    }

    ss.get(ch);
    getline(ss, symbolic_name);
    return true;
}
bool CirMgr::read_interger_parser(string input, vector<int>& tokens, unsigned int number_num) const
{
    stringstream ss;
    ss << " " << input;
    while(!ss.eof())
    {
        char ch;
        int token;

        ss.get(ch);
        if(ch != ' ') // TODO error
            return false;

        ss.get(ch);
        if(ss.eof()) // TODO error addtion space
            return false;
        if(!('0' <= ch && ch <= '9')) // TODO error
            return false;
        ss.putback(ch);

        ss >> token;
        //if(!ss.good() && !ss.eof()) // TODO error
        //    return false;
        tokens.push_back(token);
    }
    if(tokens.size() != number_num) // TODO error
        return false;
    return true;
}
bool CirMgr::read_header_parser(const string& input, vector<int>& tokens) const
{
    // TODO check aag
    if(!read_interger_parser(input.substr(4), tokens, 5)) // remove aag
        return false;
    // TODO check all tokens positive
    return true;
}
bool CirMgr::read_gate_parser(const string& input, vector<int>& tokens, GateType type) const
{
    switch(type)
    {
        case PI_GATE:
        case PO_GATE:
            return read_interger_parser(input, tokens, 1);
        case AIG_GATE:
            return read_interger_parser(input, tokens, 3);
        default:
            return false;
    }
}
unsigned int CirMgr::literal_to_variable(int literal_id, bool& inverted) const
{
    inverted = false;
    if(literal_id % 2)
    {
        inverted = true;
        --literal_id;
    }
    return (literal_id / 2);
}
unsigned int CirMgr::literal_to_variable(int literal_id) const
{
    return (literal_id / 2);
}
void CirMgr::set_header(const vector<int>& tokens)
{
    _header_M = tokens[0];
    _header_I = tokens[1];
    _header_L = tokens[2];
    _header_O = tokens[3];
    _header_A = tokens[4];
    _gate_list.resize(_header_M + _header_O + 1, _dummy_udf_gate);
    _gate_list[0] = new CONSTGate;
}
void CirMgr::set_gate(const vector<int>& tokens, GateType type, unsigned int lno)
{
    if(type == PI_GATE)
    {
        unsigned int the_gate_id = literal_to_variable(tokens[0]);
        if(_gate_list[the_gate_id]->gate_type() != UNDEF_GATE) // TODO error corruption
            return;

        _gate_list[the_gate_id] = new PIGate(the_gate_id, lno);
        _pi_list.push_back(the_gate_id);
    }
    else if(type == PO_GATE)
    {
        unsigned int the_gate_id = tokens[1];
        if(_gate_list[the_gate_id]->gate_type() != UNDEF_GATE) // TODO error corruption
            return;

        bool src_inverted;
        unsigned int src_gate_id = literal_to_variable(tokens[0], src_inverted);
        vector<CirGate::related_gate> src;
        src.push_back(CirGate::related_gate(&_gate_list[src_gate_id], src_inverted));

        _gate_list[the_gate_id] = new POGate(the_gate_id, lno, src);
        _po_list.push_back(the_gate_id);
    }
    else if(type == AIG_GATE)
    {
        unsigned int the_gate_id = literal_to_variable(tokens[0]);
        if(_gate_list[the_gate_id]->gate_type() != UNDEF_GATE) // TODO error corruption
            return;

        bool src_inverted[2];
        unsigned int src_gate_id[2];
        src_gate_id[0] = literal_to_variable(tokens[1], src_inverted[0]);
        src_gate_id[1] = literal_to_variable(tokens[2], src_inverted[1]);
        vector<CirGate::related_gate> src;
        src.push_back(CirGate::related_gate(&_gate_list[src_gate_id[0]], src_inverted[0]));
        src.push_back(CirGate::related_gate(&_gate_list[src_gate_id[1]], src_inverted[1]));

        _gate_list[the_gate_id] = new AIGGate(the_gate_id, lno, src);
    }
}
