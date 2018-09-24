/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/Wildfoot
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */


#include <iostream>

using namespace std;

template <class T>
void print_anv(const T & p, const string & str)
{
    cout << str << "'s value : ";
    cout << p << "\t";
    cout << " address : ";
    cout << &p << endl;
}

int main()
{
    int a = 10;
    int &r = a;
    print_anv(a, "a");
    print_anv(r, "r");

    cout << "int b = 20; r = b;" << endl;
    int b = 20;
    r = b;
    print_anv(a, "a");
    print_anv(r, "r");
    print_anv(b, "b");

    return 0;
}

