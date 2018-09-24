/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/Wildfoot
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */


#include <iostream>

using namespace std;

void f()
{
    int a, b, c;
    int *p = new int;
    int *q = new int;
    cout << &a << " " << &b << " " << &c << " " << p << " " << q << endl;
}

int main()
{
    f();
    f();
    f();
    return 0;
}

