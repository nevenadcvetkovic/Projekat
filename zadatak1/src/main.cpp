/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: etf
 *
 * Created on June 4, 2019, 2:12 PM
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "Compiler.h"
#include "Symbol.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if(argc!=3){
        cout<<"Error!  Invalid number of operands!"<<endl;
        exit(-1);
    }
    string inputName=argv[1];
    string outputName=argv[2];

    
    string  str="\n";
    str.resize(0);
    cout<<str;
    cout<<"cao"<<endl;
    
    ifstream input;
    input.open(inputName);
    Compiler*compiler=new Compiler();
    compiler->FirstRun(input);
    input.clear();
    input.seekg(0, ios::beg);
    compiler->SecondRun(input);
    input.close();
    
    ofstream output;
    output.open(outputName);
    compiler->printToFile(output);
    output.close();
    delete compiler;        
        
    return 0;
}

