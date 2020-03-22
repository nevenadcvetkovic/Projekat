
/* 
 * File:   Symbol.cpp
 * Author: etf
 * 
 * Created on June 5, 2019, 4:22 AM
 */

#include "Symbol.h"

int Symbol::posId = 0;

Symbol::Symbol() {
    id = posId++;
    value = size = -1;
    myExpression = "";

}

Symbol::Symbol(string name, string section, int value, int local, int rwx) {
    this->id = posId++;
    this->isLocal = local;
    this->name = name;
    this->rwx = rwx;
    this->section = section;
    this->value = value;
    this->size = -1;
    this->myExpression = "";

}

Symbol::Symbol(const Symbol& orig) {
}

Symbol::~Symbol() {
}

void Symbol::setId(int i) {
    id = i;
}

int Symbol::getId() {
    return id;
}

void Symbol::setName(string n) {
    name = n;
}

string Symbol::getName() {
    return name;
}

int Symbol::getLocality() {
    return isLocal;
}

void Symbol::setLocality(int l) {
    isLocal = l;
}

string Symbol::getSection() {
    return section;
}

void Symbol::setSection(string s) {
    section = s;
}

int Symbol::getValue() {
    return value;
}

void Symbol::setValue(int v) {
    value = v;
}

int Symbol::getRWX() {
    return rwx;
}

void Symbol::setRWX(int flags) {
    rwx = flags;
}

void Symbol::setSize(int s) {
    size = s;
}

int Symbol::getSize() {
    return size;
}

void Symbol::setExpression(string s) {
    myExpression = s;
}

string Symbol::getExpressionl() {
    return myExpression;
}

void Symbol::printSymbol() {
    cout << id << '\t' << name << '\t' << section << '\t';
    if (value != -1)
        cout << value << '\t';
    else
        cout << "UND" << '\t';
    if (isLocal == 1)
        cout << "local" << '\t';
    else
        cout << "global" << '\t';
    if (size != -1)
        cout << " " << size << '\t';
    else
        cout << " " << size << '\t';
    if (rwx == 0)
        cout << rwx << endl;
    else if (rwx == 1)
        cout << "RE" << endl;
    else
        cout << "WR" << endl;
}

void Symbol::printSymbol(ofstream& file) {
    file << id << '\t' << name << '\t' << section << '\t';
    if (value != -1)
        file << value << '\t';
    else
        file << "UND" << '\t';
    if (isLocal == 1)
        file << "local" << '\t';
    else
        file << "global" << '\t';
    if (size != -1)
        file << " " << size << '\t';
    else
        file << " " << size << '\t';
    if (rwx == 0)
        file << rwx << endl;
    else if (rwx == 1)
        file << "RE" << endl;
    else
        file << "WR" << endl;

}