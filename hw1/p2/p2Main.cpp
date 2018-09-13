/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstring>
#include "p2Json.h"

using namespace std;

int main()
{
    Json json;

    // Read in the csv file. Do NOT change this part of code.
    string jsonFile;
    cout << "Please enter the file name: ";
    cin >> jsonFile;
    if (json.read(jsonFile))
        cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
    else {
        cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
        exit(-1); // jsonFile does not exist.
    }

    while (true)
    {
        string input_command;
        cout << "Enter command: ";
        cin >> input_command;

        if(!input_command.compare("PRINT"))
            json.print_all();
        else if(!input_command.compare("ADD"))
        {
            string new_key;
            int new_value;
            cin >> new_key >> new_value;
            json.add_elem(new_key, new_value);
        }
        else if(!input_command.compare("SUM"))
            json.print_sum_value();
        else if(!input_command.compare("AVE"))
            json.print_ave_value();
        else if(!input_command.compare("MAX"))
            json.print_max_elem();
        else if(!input_command.compare("MIN"))
            json.print_min_elem();
        else if(!input_command.compare("EXIT"))
            break;
        else
            cout << "Error: unknown command: \"" << input_command << "\"\n";

    }
    return 0;
}
