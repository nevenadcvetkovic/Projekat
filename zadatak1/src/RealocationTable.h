/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RealocationTable.h
 * Author: etf
 *
 * Created on June 13, 2019, 4:56 PM
 */

#ifndef REALOCATIONTABLE_H
#define REALOCATIONTABLE_H

#include <vector>
#include "RelRow.h"

using  namespace std;

class RealocationTable {
public:
    RealocationTable();
    RealocationTable(const RealocationTable& orig);
    virtual ~RealocationTable();
    
    void addRelText(RelRow*& row);
    void addRelData(RelRow*& row);
    void  printRelTable();
    void  printRelTable(ofstream& file);
private:
    vector<RelRow*>relText;
    vector<RelRow*>relData;
};

#endif /* REALOCATIONTABLE_H */

