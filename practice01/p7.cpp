/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/Wildfoot
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */


#include <iostream>

using namespace std;

class C {
public:
    void printAddr() const {
        cout << &_a << " " << &_b << endl;
    }
private:
    int _a;
    int _b;
};

int main()
{
    C c1, c2;
    c1.printAddr();
    c2.printAddr();

    return 0;
}

