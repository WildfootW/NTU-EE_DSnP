/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <iomanip>
#include "p2Json.h"

using namespace std;

bool
JsonElem::operator< (const JsonElem & param)
{
    return _value < param._value;
}
bool
JsonElem::operator> (const JsonElem & param)
{
    return _value > param._value;
}

// Implement member functions of class Row and Table here
bool
Json::read(const string& json_Filename)
{
    fstream json_File;
    json_File.open(json_Filename, ios::in);
    if(!json_File.is_open())
        return false;

    json_File.ignore(numeric_limits<streamsize>::max(), '{');
    while(true)
    {
        char next_char;
        while(true)
        {
            next_char = json_File.get();
            if(next_char == '"' || next_char == '}')
                break;
        }
        if(next_char == '}')
            break;
        json_File.unget();  // put '"' back

        string new_key;
        int new_value;
        json_File.ignore(numeric_limits<streamsize>::max(), '"');
        getline(json_File, new_key, '"');
        json_File.ignore(numeric_limits<streamsize>::max(), ':');
        while(true)
        {
            next_char = json_File.get();
            if(('0' <= next_char && next_char <= '9') || next_char == '-')
            {
                json_File.unget();
                break;
            }
        }
        json_File >> new_value;

        add_elem(new_key, new_value);

        //cout << new_key << " " << new_value << endl;
    }

    json_File.close();
    return true;
}

void
Json::print_all()
{
    cout << "{\n";
    for(vector<JsonElem>::iterator it = _obj.begin(); it != _obj.end();)
    {
        cout << "  " << *it;
        if(++it != _obj.end())
            cout << ",";
        cout << "\n";
    }
    cout << "}\n";
}

void
Json::add_elem(string new_key, int new_value)
{
    JsonElem new_JsonElem(new_key, new_value);
    sum_value += new_value;
    if(Max_Elem < new_JsonElem)
        Max_Elem = new_JsonElem;
    if(Min_Elem > new_JsonElem)
        Min_Elem = new_JsonElem;
    _obj.push_back(new_JsonElem);
}

void
Json::print_sum_value()
{
    if(empty())
        return;
    cout << "The summation of the values is: " << sum_value << ".\n";
}
void
Json::print_ave_value()
{
    if(empty())
        return;
    cout << "The average of the values is: " << fixed << setprecision(1) << sum_value / (double)_obj.size() << ".\n";
}
void
Json::print_max_elem()
{
    if(empty())
        return;
    cout << "The maximum element is: { " << Max_Elem << " }.\n";
}
void
Json::print_min_elem()
{
    if(empty())
        return;
    cout << "The minimum element is: { " << Min_Elem << " }.\n";
}

bool
Json::empty()
{
    if(_obj.empty())
    {
        cout << "Error: No element found!!\n";
        return true;
    }
    else
        return false;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

