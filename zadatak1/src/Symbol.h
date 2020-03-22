/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Symbol.h
 * Author: etf
 *
 * Created on June 5, 2019, 4:22 AM
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Symbol {
public:

    Symbol();
    Symbol(string name, string section, int  value, int local, int  rwx);
    Symbol(const Symbol& orig);
    virtual ~Symbol();

    void setId(int i);
    int getId();
    void setName(string n);
    string getName();
    void setSection(string s);
    string getSection();
    void setValue(int v);
    int getValue();
    void setRWX(int flags);
    int getRWX();
    void setLocality(int l);
    int getLocality();
    void setSize(int s);
    int getSize();
    string getExpressionl();
    void setExpression(string s);
    void printSymbol();
    void printSymbol(ofstream&);

private:
    static int posId;
    int size;
    int id;
    string name;
    string section;
    int value;
    int isLocal; //local=1 global=0 extern=2
    int rwx;
    string myExpression;

};

#endif /* SYMBOL_H */

