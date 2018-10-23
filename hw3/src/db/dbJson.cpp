/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
    assert(j._obj.empty());

    is.ignore(numeric_limits<streamsize>::max(), '{');
    while(true)
    {
        string new_key;
        int new_value;

        char next_char;
        while(true)
        {
            next_char = is.get();
            if(next_char == '"' || next_char == '}')
                break;
        }
        if(next_char == '}')
            break;
        is.unget();  // put '"' back

        is.ignore(numeric_limits<streamsize>::max(), '"');
        getline(is, new_key, '"');
        is.ignore(numeric_limits<streamsize>::max(), ':');
        while(true)
        {
            next_char = is.get();
            if(('0' <= next_char && next_char <= '9') || next_char == '-')
            {
                is.unget();
                break;
            }
        }
        is >> new_value;

        //clog << new_key << " " << new_value << endl;
        assert(isValidVarName(new_key));

        DBJsonElem json_elem(new_key, new_value);
        //assert(j.add(json_elem));
        j.add(json_elem);
    }
    return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
    os << "{\n";
    for(auto it = j._obj.begin(); it != j._obj.end() - 1; it++)
    {
        os << "    " << *it << ",\n";
    }
    os << "    " << j._obj.back() << "\n";
    os << "}";
    return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
    _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
    // check if repeat
    for(DBJsonElem e:_obj)
    {
        if(e.key() == elm.key())
        {
            cerr << elm << " is duplicate with " << e << ". skip...\n";
            return false;
        }
    }

    _obj.push_back(elm);
    return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
    if(empty())
        return NAN;
    return sum() / (double)size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
    int maxN = INT_MIN;
    if(empty())
        idx = size();
    else
        for(size_t i = 0;i < size();i++)
            if(_obj[i].value() > maxN)
            {
                maxN = _obj[i].value();
                idx = i;
            }

    return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
    int minN = INT_MAX;
    if(empty())
        idx = size();
    else
        for(size_t i = 0;i < size();i++)
            if(_obj[i].value() < minN)
            {
                minN = _obj[i].value();
                idx = i;
            }

    return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   int s = 0;
   for(DBJsonElem e:_obj)
       s += e.value();
   return s;
}
