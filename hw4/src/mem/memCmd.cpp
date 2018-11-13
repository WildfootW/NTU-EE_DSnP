/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
    vector<string> tokens;
    if(!CmdExec::lexOptions(option, tokens, 0))
        return CMD_EXEC_ERROR;

    int obj_num = -1, arr_size = -1, _tmp_int;

    for(size_t i = 0;i < tokens.size();++i)
    {
        if(myStrNCmp("-Array", tokens[i], 2) == 0)
        {
            if(arr_size != -1)  // duplicate
                return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[i]);
            if(i == tokens.size() - 1)
                return CmdExec::errorOption(CMD_OPT_MISSING, tokens[i]);
            if(!myStr2Int(tokens[i + 1], _tmp_int) || _tmp_int <= 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[i + 1]);
            arr_size = _tmp_int;
            ++i;
            continue;
        }
        else if(myStr2Int(tokens[i], _tmp_int) && _tmp_int > 0)
        {
            if(obj_num != -1)
                return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[i]);
            obj_num = _tmp_int;
            continue;
        }
        else
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[i]);
        }
    }
    if(obj_num == -1)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    // Use try-catch to catch the bad_alloc exception
    try
    {
        if(arr_size != -1)
            mtest.newArrs(obj_num, arr_size);
        else
            mtest.newObjs(obj_num);
    }
    catch(bad_alloc)
    {
        return CMD_EXEC_ERROR;
    }
    return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
    vector<string> tokens;
    if(!CmdExec::lexOptions(option, tokens, 0))
        return CMD_EXEC_ERROR;

    bool _random_flag = false, _index_flag = false, _array_flag = false;
    int obj_id, randid_num;
    size_t _random_option_idx, _obj_id_idx; // just for error message

    for(size_t i = 0;i < tokens.size();++i)
    {
        if(myStrNCmp("-Index", tokens[i], 2) == 0)
        {
            if(_random_flag || _index_flag)  // duplicate
                return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[i]);
            if(i == tokens.size() - 1)
                return CmdExec::errorOption(CMD_OPT_MISSING, tokens[i]);
            if(!myStr2Int(tokens[i + 1], obj_id) || obj_id < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[i + 1]);
            _index_flag = true;
            _obj_id_idx = i + 1;
            ++i;
            continue;
        }
        else if(myStrNCmp("-Random", tokens[i], 2) == 0)
        {
            if(_random_flag || _index_flag)  // duplicate
                return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[i]);
            if(i == tokens.size() - 1)
                return CmdExec::errorOption(CMD_OPT_MISSING, tokens[i]);
            if(!myStr2Int(tokens[i + 1], randid_num) || randid_num <= 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[i + 1]);
            _random_flag = true;
            _random_option_idx = i;
            ++i;
            continue;
        }
        else if(myStrNCmp("-Array", tokens[i], 2) == 0)
        {
            if(_array_flag)  // duplicate
                return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[i]);
            _array_flag = true;
            continue;
        }
        else
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[i]);
        }
    }
    if(!(_random_flag || _index_flag))
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    if(_index_flag) // check if obj_id is out of range
    {
        if(_array_flag && mtest.getArrListSize() <= (size_t)obj_id)
        {
            cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << obj_id << "!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[_obj_id_idx]);
        }
        if(!_array_flag && mtest.getObjListSize() <= (size_t)obj_id)
        {
            cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << obj_id << "!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[_obj_id_idx]);
        }
    }
    if(_random_flag) // check if ObjListSize or ArrListSize is 0
    {
        if(_array_flag && mtest.getArrListSize() == 0)
        {
            cerr << "Size of array list is 0!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[_random_option_idx]);
        }
        if(!_array_flag && mtest.getObjListSize() == 0)
        {
            cerr << "Size of object list is 0!!\n";
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[_random_option_idx]);
        }
    }
    // run
    if(_index_flag)
    {
        if(_array_flag)
            mtest.deleteArr(obj_id);
        else
            mtest.deleteObj(obj_id);
    }
    else if(_random_flag)
    {
        if(_array_flag)
            for(int i = 0;i < randid_num;++i)
                mtest.deleteArr((size_t)rnGen(mtest.getArrListSize()));
        else
            for(int i = 0;i < randid_num;++i)
                mtest.deleteObj((size_t)rnGen(mtest.getObjListSize()));
    }

    return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}


