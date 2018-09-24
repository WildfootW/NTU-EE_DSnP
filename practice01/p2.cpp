/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/Wildfoot
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */


#include <iostream>

using namespace std;

void print_anv(const int & p, const string & str)
{
    cout << str << " : ";
    cout << p << "\t";
    cout << &p << endl;
}

int main()
{
    int a, b, c;
    print_anv(a, "a");
    print_anv(b, "b");
    print_anv(c, "c");
    while (cin >> a >> b) {
        c = a + b;
        print_anv(a, "a");
        print_anv(b, "b");
        print_anv(c, "c");
    }

    return 0;
}

