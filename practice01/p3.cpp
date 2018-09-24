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
    int a;
    int * p;
    p = &a;
    print_anv(a, "a");
    print_anv(p, "p");

    return 0;
}

