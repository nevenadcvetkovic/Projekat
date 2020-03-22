/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RelRow.h
 * Author: etf
 *
 * Created on June 13, 2019, 5:06 PM
 */

#ifndef RELROW_H
#define RELROW_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

using  namespace std;

class RelRow {
public:
    RelRow();
    RelRow(int id, string type, int offset);
    RelRow(const RelRow& orig);
    virtual ~RelRow();
    
    int getId();
    void setId(int i);
    string getType();
    void setType(string t);
    int getOffset();
    void  setOffset(int  o);
    
    void printRow(ostream& file);
    void printRow();
    string intToHex(int  num);
    
private:
    int id;
    string  type;
    int  offset;

};

#endif /* RELROW_H */

