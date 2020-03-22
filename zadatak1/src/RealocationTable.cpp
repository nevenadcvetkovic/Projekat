/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RealocationTable.cpp
 * Author: etf
 * 
 * Created on June 13, 2019, 4:56 PM
 */

#include "RealocationTable.h"

RealocationTable::RealocationTable() {
}

RealocationTable::RealocationTable(const RealocationTable& orig) {
}

RealocationTable::~RealocationTable() {
}

void RealocationTable::addRelData(RelRow*& row) {
    relData.push_back(row);
}

void RealocationTable::addRelText(RelRow*& row) {
    relText.push_back(row);
}

void RealocationTable::printRelTable() {
    if (relText.size() > 0) {
        cout << "#text" << endl;
        cout << "#offset   " << "#type    " << "vr[.text]"<<endl;
        for(int i=0; i<relText.size();i++){
            relText.at(i)->printRow();                  
        }

    }
        if (relData.size() > 0) {
        cout << "#data" << endl;
        cout << "#offset   " << "#type    " << "vr[.data]"<<endl;
        for(int i=0; i<relData.size();i++){
            relData.at(i)->printRow();                  
        }

    }
}
void RealocationTable::printRelTable(ofstream& file){
    if (relText.size() > 0) {
        file << "#text" << endl;
        file << "#offset   " << "#type    " << "vr[.text]"<<endl;
        for(int i=0; i<relText.size();i++){
            relText.at(i)->printRow(file);                  
        }

    }
        if (relData.size() > 0) {
        file << "#data" << endl;
        file << "#offset   " << "#type    " << "vr[.data]"<<endl;
        for(int i=0; i<relData.size();i++){
            relData.at(i)->printRow(file);                  
        }

    }
}