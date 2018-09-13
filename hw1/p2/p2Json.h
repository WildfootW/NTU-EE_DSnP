/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:
    JsonElem() {}
    JsonElem(const string& k, int v): _key(k), _value(v) {}

    bool operator< (const JsonElem &);
    bool operator> (const JsonElem &);

    friend ostream& operator << (ostream&, const JsonElem&);

private:
    string  _key;   // DO NOT change this definition. Use it to store key.
    int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:
    Json(): sum_value(0), Max_Elem("Default_MAX", -2147483648), Min_Elem("Default_MIN", 2147483647) {}
    bool read(const string&);
    void print_all(void);
    void add_elem(string new_key, int new_value);
    void print_sum_value(void);
    void print_ave_value(void);
    void print_max_elem(void);
    void print_min_elem(void);
    bool empty(void);

private:
    vector<JsonElem>       _obj;  // DO NOT change this definition.
                                  // Use it to store JSON elements.
    long long int sum_value;
    JsonElem Max_Elem, Min_Elem;
};
#endif // P2_TABLE_H
