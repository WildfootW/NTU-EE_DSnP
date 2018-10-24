/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
    if(_dofileStack.size() >= 512)
    {
        //cerr << "Error: open too many dofile!! skip...\n";
        return false;
    }

    ifstream * test_stream = new ifstream(dof.c_str());
    if(!test_stream->is_open())
        return false;

    _dofileStack.push(_dofile);
    _dofile = test_stream;
    return true;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
    // check if dofile is end. if it is not, return
    if(_dofile == 0 || !_dofile->eof())
        return;

    _dofile->close();
    delete _dofile;
    _dofile = _dofileStack.top();
    _dofileStack.pop();

    closeDofile();
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str)) return false;
      if (s == nCmp) break;
      str.resize(--s);
   }

   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);    // if cmd is HIStory than optCmd store "tory"

   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
    if(_dofile != 0)
        closeDofile();

   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);

   // execute the command
   if (newCmd) {
      string option;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
    for(auto it = _cmdMap.begin(); it != _cmdMap.end(); it++)
        it->second->help();
    cout << endl;
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
    assert(_tempCmdStored == false);
    assert(!_history.empty());

    string str = _history.back();   // str = "cmd opt opt opt"
    size_t found_space = str.find(" ");
    if(found_space != ::string::npos)
        option = str.substr(found_space+ 1, str.length());
    string cmdName = str.substr(0, found_space);

    CmdExec * e = getCmd(cmdName);
    if(e == 0)
        cerr << "Illegal command!! (" << cmdName << ")\n";
    else
        return e;

    assert(str[0] != 0 && str[0] != ' ');
    return NULL;
}

// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               // auto completed with a space inserted
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    // e.g. in hw3/ref
//    cmd> help mydb-$orld
//    ==> DO NOT print the matched files
//    ==> If "tab" is pressed again, see 6.2
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    // e.g. in hw3/bin
//    cmd> help mydb $
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    ==> If "tab" is pressed again, make a beep sound and DO NOT re-print 
//        the singly-matched file
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
void
CmdParser::listCmd(const string& str)
{
    cout << "\n";

    // case 1
    if(str.find_first_not_of(' ') == string::npos)
    {
        size_t count = 0;
        for(auto it = _cmdMap.begin(); it != _cmdMap.end(); it++)
        {
            if(count && !(count % 5))
                cout << "\n";
            count++;

            cout << setw(12) << left << (it->first + it->second->getOptCmd());
        }
        reprintCmd();
        return;
    }

    string lead_str = str, rest_str;
    // remove lead blank
    lead_str = lead_str.substr(lead_str.find_first_not_of(' '));

    // case 2, 3, 4
    if(lead_str.find(' ') == ::string::npos)
    {
        CmdMap match_cmd;
        for(auto it = _cmdMap.begin(); it != _cmdMap.end(); it++)
        {
            if(!myStrNCmp(it->first + it->second->getOptCmd(), lead_str, 1))
                match_cmd.insert(*it);
        }
        if(match_cmd.size() == 0) // case 4
        {
            mybeep();
            reprintCmd();
            return;
        }
        else if(match_cmd.size() == 1) // case 3
        {
            reprintCmd();
            string insert_str = match_cmd.begin()->first + match_cmd.begin()->second->getOptCmd();
            insert_str = insert_str.substr(lead_str.length());
            insert_str += " ";
            for(size_t i = 0;i < insert_str.length();i++)
                insertChar(insert_str[i]);
            return;
        }
        else // case 2
        {
            size_t count = 0;
            for(auto it = match_cmd.begin(); it != match_cmd.end(); it++)
            {
                if(count && !(count % 5))
                    cout << "\n";
                count++;
                cout << setw(12) << left << (it->first + it->second->getOptCmd());
            }
            reprintCmd();
            return;
        }
    }
    // case 5, 6, 7
    // split by ' '
    rest_str = lead_str.substr(lead_str.find(' '));
    lead_str = lead_str.substr(0, lead_str.find(' '));
    //clog << lead_str << "%\n";
    //clog << rest_str << "%\n";

    CmdExec * e;
    e = getCmd(lead_str);
    // case 5, 6
    if(e != 0)
    {
        static string last_str = "";
        static size_t last_tabcount;

        // case 6
        if(last_str == str && last_tabcount + 1 == _tabPressCount)
        {
            last_tabcount = _tabPressCount;
            rest_str = rest_str.substr(rest_str.find_last_of(' ') + 1);
            vector<string> files;
            if(listDir(files, rest_str, "."))
                cerr << "Error: listDir sth wrong!\n";

            if(files.size() == 0) // case 6.5
            {
                mybeep();
                reprintCmd();
                return;
            }
            else if(files.size() == 1) // case 6.1.3, 6.4
            {
                reprintCmd();
                string insert_str = files[0];
                insert_str = insert_str.substr(rest_str.length());
                insert_str += " ";
                for(size_t i = 0;i < insert_str.length();i++)
                    insertChar(insert_str[i]);
                return;
            }
            else // case 6.1.1, 6.1.2, 6.2, 6.3
            {
                // [TODO] need better algorithm
                string common_prefix = "";
                for(size_t i = 0;;i++)
                {
                    for(const string & file:files)
                        if(i == file.length())
                            break;

                    char test_char = files[i][0];
                    bool test_flag = true;
                    for(const string & file:files)
                    {
                        if(test_char != file[i])
                        {
                            test_flag = false;
                            break;
                        }
                    }
                    if(test_flag)
                        common_prefix += test_char;
                    else
                        break;
                }

                // [TODO]
                if(common_prefix.length() == 0) // case 6.1.1, 6.2
                {
                }
                else // case 6.1.2, 6.3
                {
                    reprintCmd();
                    string insert_str = common_prefix;
                    insert_str = insert_str.substr(rest_str.length());
                    insert_str += " ";
                    for(size_t i = 0;i < insert_str.length();i++)
                        insertChar(insert_str[i]);
                    return;
                }
            }
        }
        // case 5
        else
        {
            last_str = str;
            last_tabcount = _tabPressCount;
            e->usage(cout);
            reprintCmd();
            return;
        }
    }
    else // case 7
    {
        mybeep();
        reprintCmd();
        return;
    }

}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
    CmdExec* e = 0;

    for (size_t i = 0; i < cmd.length(); i++)
        cmd[i] = toupper(cmd[i]);

    for(size_t i = 1;i <= cmd.length();i++)
    {
        if(_cmdMap.find(cmd.substr(0, i)) != _cmdMap.end())
        {
            string mandCmd = cmd.substr(0, i);

            // check _optCmd
            //cout << mandCmd + _cmdMap[mandCmd]->getOptCmd() << " " << cmd << " " << mandCmd.length();
            if(myStrNCmp(mandCmd + _cmdMap[mandCmd]->getOptCmd(), cmd, mandCmd.length()) == 0)
            {
                e = _cmdMap[mandCmd];
            }
            break;
        }
    }

    return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool
CmdExec::lexNoOption(const string& option) const
{
   string err;
   myStrGetTok(option, err);
   if (err.size()) {
      errorOption(CMD_OPT_EXTRA, err);
      return false;
   }
   return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Error: Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Error: Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Error: Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

