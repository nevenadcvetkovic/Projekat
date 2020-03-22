
#ifndef TABLEOFSYMBOLS_H
#define TABLEOFSYMBOLS_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include "Symbol.h"

using namespace std;

class TableOfSymbols {
public:
    TableOfSymbols();
    TableOfSymbols(const TableOfSymbols& orig);
    virtual ~TableOfSymbols();
    bool addSymbol(Symbol*&); //true  if the operation was  successful
    bool symbolExists(string name); //returns  true  if symbol  name exists in the table
    void printTable();
    void printTable(ofstream& file);
    Symbol*& getSymbolByName(string name);
    
    bool  addSection(Symbol*&);
    bool  sectionExists(string name);
    Symbol*& getSectionByName(string  name);
    
    bool calculateEQUvalue(Symbol*& symbol);
    void calculateEQUsymbols();
    void incNumOfEQUnotDefined();
    
    void checkIfSymbDef();
private:
    vector<Symbol*>tableSymb;
    vector<Symbol*>tableSect;
    int numEQUnotDefined;

};

#endif /* TABLEOFSYMBOLS_H */

