/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
    resetBufAndPrintPrompt();

    while (1) {
        ParseChar pch = getChar(istr);
        if (pch == INPUT_END_KEY) break;
        switch (pch) {
            case LINE_BEGIN_KEY :
            case HOME_KEY       : moveBufPtr(_readBuf); break;
            case LINE_END_KEY   :
            case END_KEY        : moveBufPtr(_readBufEnd); break;
            case BACK_SPACE_KEY :
                if(moveBufPtr(_readBufPtr - 1))
                    deleteChar();
                break;
            case DELETE_KEY     : deleteChar(); break;
            case NEWLINE_KEY    : addHistory();
                                  cout << char(NEWLINE_KEY);
                                  resetBufAndPrintPrompt(); break;
            case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
            case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
            case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr + 1); break;
            case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr - 1); break;
            case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
            case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
            case TAB_KEY        : insertChar(' ', TAB_POSITION - ((_readBufPtr - _readBuf) % TAB_POSITION)); break;
            case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
            case UNDEFINED_KEY:   mybeep(); break;
            default:  // printable character
                insertChar(char(pch)); break;
        }
        #ifdef TA_KB_SETTING
        taTestOnly();
        #endif
    }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
    // check if ptr is in the range
    if(!(_readBuf <= ptr && ptr <= _readBufEnd))
    {
        mybeep();
        return false;
    }
    // move to right or left
    while(ptr < _readBufPtr)
    {
        cout << '\b';
        _readBufPtr--;
    }
    while(ptr > _readBufPtr)
    {
        cout << *_readBufPtr;
        _readBufPtr++;
    }
    return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
    char * old_readBufPtr = _readBufPtr;
    char * old_readBufEnd = _readBufEnd;

    // check if cursor is on the end of line
    if(_readBufPtr == old_readBufEnd)
    {
        mybeep();
        return false;
    }

    // modify data members
    for(char * copy_ptr = old_readBufPtr;copy_ptr < old_readBufEnd - 1;copy_ptr++)
    {
        *copy_ptr = *(copy_ptr + 1);
    }
    *(old_readBufEnd - 1) = '\0';

    // print
    for(;_readBufPtr < old_readBufEnd;_readBufPtr++)
        cout << *_readBufPtr;
    cout << ' ';
    moveBufPtr(old_readBufPtr);
    _readBufEnd--;
    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
    assert(repeat >= 1);
    char * old_readBufEnd = _readBufEnd;
    char * old_readBufPtr = _readBufPtr;

    // class data
    for(char * copy_ptr = (old_readBufEnd - 1);copy_ptr >= old_readBufPtr;copy_ptr--)
        *(copy_ptr + repeat) = *(copy_ptr);
    for(int i = 0;i < repeat;i++)
        *(old_readBufPtr + i) = ch;

    // print
    for(;_readBufPtr < old_readBufEnd + repeat;_readBufPtr++)
        cout << *_readBufPtr;
    moveBufPtr(old_readBufPtr + repeat);    // second, use moveBufPtr to modify the cursor and _readBufPtr
    _readBufEnd += repeat;      // modify the position of pointers

}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
    int _historyEnd = _history.size();
    if(index < _historyIdx)
    {
        if(_historyIdx == 0)
        {
            mybeep();
            return;
        }
        if(index < 0)
            index = 0;
        //save current line to history vector
        //if(!_tempCmdStored)
        //{
        //    addHistory();
        //    _tempCmdStored = true;
        //}
        //


    }
    else if(index > _historyIdx)
    {
        if(_historyIdx == _historyEnd - 1)
        {
            mybeep();
            return;
        }
        if(index >= _historyEnd)
            index = _historyEnd - 1;
    }
    // print
    moveBufPtr(_readBuf);
    long int StrLength = _readBufEnd - _readBuf;
    if(StrLength < 1)
        StrLength = 1;

    insertChar(' ', StrLength);
    moveBufPtr(_readBuf);
    for(unsigned int i = 0;i < _history[index].length();i++)
        insertChar(_history[index][i]);

    //cout << "index = " << index << endl;
    //cout << "_historyIdx = " << _historyIdx << endl;

}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
    *_readBufEnd = '\0';
    string str(_readBuf);

    _historyIdx = _history.size();
    if(_tempCmdStored)
    {
        _history.pop_back();    // pop temp saved string
        _tempCmdStored = false;
    }

    // remove ' ' at the beginning and end of str
    unsigned int NewStrBegin = str.find_first_not_of(' ');
    unsigned int NewStrEnd = str.find_last_not_of(' ');
    unsigned int NewStrRange = NewStrEnd - NewStrBegin + 1;

    if(NewStrBegin > str.size()) // null string
        return;

    else
    {
        str = str.substr(NewStrBegin, NewStrRange);
        _history.push_back(str);
        _historyIdx++;
    }
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}

// lazy debug
// printf("\n");
// printf("_readBuf: %p\n", _readBuf);
// printf("_readBufEnd: %p\n", _readBufEnd);
// printf("length: %ld\n", _readBufEnd - _readBuf);
// printf("_readBufPtr: %p\n", _readBufPtr);
// printf("length: %ld\n", _readBufPtr - _readBuf);

