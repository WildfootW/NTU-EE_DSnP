/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/Wildfoot
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */

#include <iostream>
#include <bitset>

using namespace std;

//template <class T>
//void print_memory(T & origin)
//{
//    union u
//    {
//        T object;
//        unsigned char byte_arr[sizeof(T)];
//    };
//    u temp;
//    temp.object = origin;
//    for(size_t i = 0;i < sizeof(T); i++)
//        printf("%02x ", temp.byte_arr[i]);
//    printf("\n");
//
//    bitset<sizeof(T) * 8> bin_temp(origin);
//    cout << bin_temp << endl;
//    return;
//}

template <class T>
void print_anc(const T & p, const string & str)
{
    cout << str << "'s address : ";
    cout << &p << endl;
}

class C
{
    char byte[3];
};

int main()
{
    int *a1 = new int;
    int *a2 = new int;
    C *c1 = new C;
    C *c2 = new C;
    C *c3 = new C;
    print_anc(a1, "a1");
    print_anc(a2, "a2");
    print_anc(c1, "c1");
    print_anc(c2, "c2");
    print_anc(c3, "c3");
    return 0;
}
