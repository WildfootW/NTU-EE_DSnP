/*
 * Version 
 * Author: WildfootW
 * GitHub: github.com/WildfootW
 * Copyright (C) 2018 WildfootW All rights reserved.
 *
 */


#include <iostream>
#include <fstream>
#include <vector>

#define MAX 2147483647
using namespace std;

static const string path = "./tests_aag.fraig/";

void generate_cirgate_test(const string filename, ostream& os)
{
    os << "CIRRead -R " << filename << endl;
    ifstream fs(filename);
    while(fs)
    {
        string str;
        char ch;
        fs.get(ch);
        if(ch == 'c')
            break;
        if(!('0' <= ch && ch <= '9'))
            continue;
        fs.putback(ch);
        int num;
        fs >> num;
        os << "CIRGate " << (int)(num / 2) << endl;
        os << "CIRGate " << (int)(num / 2) << " -FANIn " << MAX << endl;
        os << "CIRGate " << (int)(num / 2) << " -FANOut " << MAX << endl;
        getline(fs, str);
    }
    fs.close();
}

int main()
{
    const vector<string> filename = {
        "cirp-n.aag",
        "my_test1.aag",
        "opt01.aag",
        "opt02.aag",
        "opt03.aag",
        "opt04.aag",
        "opt05.aag",
        "opt06.aag",
//        "sim01.aag",
//        "sim02.aag",
//        "sim03.aag",
//        "sim04.aag",
//        "sim05.aag",
//        "sim06.aag",
//        "sim07.aag",
//        "sim08.aag",
//        "sim09.aag",
//        "sim10.aag",
//        "sim11.aag",
//        "sim12.aag",
//        "sim13.aag",
//        "sim14.aag",
//        "sim15.aag",
//        "strash01.aag",
//        "strash02.aag",
//        "strash03.aag",
//        "strash04.aag",
//        "strash05.aag",
//        "strash06.aag",
//        "strash07.aag",
//        "strash08.aag",
//        "strash09.aag",
//        "strash10.aag",
//        "test1.aag",
//        "test2.aag"
    };

    ofstream mydo("mydo3");
    for(const string& e:filename)
    {
        //generate_cirgate_test(path + e, cout);
        generate_cirgate_test(path + e, mydo);
    }
    mydo << "q -f" << endl;
    mydo.close();
    return 0;
}

