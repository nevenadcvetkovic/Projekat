/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RelRow.cpp
 * Author: etf
 * 
 * Created on June 13, 2019, 5:06 PM
 */

#include "RelRow.h"
#include "RealocationTable.h"

RelRow::RelRow() {
    id = offset = 0;
    type = "";
}

RelRow::RelRow(int id, string type, int offset) {
    this->id = id;
    this->type = type;
    this->offset = offset;
}

RelRow::RelRow(const RelRow& orig) {
}

RelRow::~RelRow() {
}

void RelRow::setId(int i) {
    id = i;
}

int RelRow::getId() {
    return id;
}

void RelRow::setOffset(int o) {
    offset = o;
}

int RelRow::getOffset() {
    return offset;
}

void RelRow::setType(string t) {
    type = t;
}

string RelRow::getType() {
    return type;
}

void RelRow::printRow(){
    int lower = offset& 0xFF;
    int higher = (offset >> 8)&0xFF;
    string str1 = intToHex(lower);
    string str2 = intToHex(higher);
    cout<<str2<<str1<<'\t'<<type<<'\t'<<id<<endl;
}
void RelRow::printRow(ostream& file){
    int lower = offset& 0xFF;
    int higher = (offset >> 8)&0xFF;
    string str1 = intToHex(lower);
    string str2 = intToHex(higher);
    file<<str2<<str1<<'\t'<<type<<'\t'<<id<<endl;
}

string RelRow::intToHex(int num) {
    stringstream stream;
    stream << "0x" << setfill('0') << setw(2) << hex << num;
    return stream.str().substr(2);

}