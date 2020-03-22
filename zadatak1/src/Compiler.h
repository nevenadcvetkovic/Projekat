/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Compiler.h
 * Author: etf
 *
 * Created on June 4, 2019, 2:26 PM
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>
#include "TableOfSymbols.h"
#include "RealocationTable.h"
#include "RelRow.h"

using namespace std;

class Compiler {
public:
    Compiler();
    Compiler(const Compiler& orig);
    virtual ~Compiler();
    void FirstRun(ifstream& input);
    void SecondRun(ifstream& input);
    void parseLine(string& line, vector<string>& dst);
    void removeWhiteSpacesFromStart(string& line);
    void processDirective(string& directive, string& label, string& line);
    
    string  intToHex(int num);
    void createNewReal(Symbol*& symbol, int type);
    bool regIndirPom(string& line, int opCode,bool secondOp);
    bool regIndirDir(string& line, int opCode, bool  secondOp);
    bool pcRelAdr(string& line, int opCode, bool secondOp);
    bool immAdr(string& line,  int opCode, bool  secondOp);
    bool memAdr(string& line, int opCode, bool secondOp);
    
    void printToFile(ofstream& file);

private:
    int locationCounter;
    int currentSection;
    TableOfSymbols* myTable;
    string* sections;
    RealocationTable* realTable;
    string*types;
    vector<string>dataWrite;
    vector<string>textWrite;

};

#endif /* COMPILER_H */

