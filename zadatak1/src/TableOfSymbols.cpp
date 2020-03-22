/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TableOfSymbols.cpp
 * Author: etf
 * 
 * Created on June 5, 2019, 11:42 PM
 */


#include "TableOfSymbols.h"

TableOfSymbols::TableOfSymbols() {
    Symbol*s = new Symbol();
    s->setId(0);
    s->setLocality(1);
    s->setName("und");
    s->setRWX(0);
    s->setSection("UND");
    s->setValue(0);
    s->setSize(0);
    tableSect.push_back(s);
    numEQUnotDefined = 0;
}

TableOfSymbols::TableOfSymbols(const TableOfSymbols& orig) {
}

TableOfSymbols::~TableOfSymbols() {
    while (tableSymb.size() != 0) {
        Symbol*s = tableSymb.back();
        tableSymb.pop_back();
        delete s;
    }
    while (tableSect.size() != 0) {
        Symbol*s = tableSect.back();
        tableSect.pop_back();
        delete s;
    }
}

void TableOfSymbols::incNumOfEQUnotDefined() {
    numEQUnotDefined++;
}

bool TableOfSymbols::addSymbol(Symbol*& s) {
    if (symbolExists(s->getName()))
        return false;
    tableSymb.push_back(s);
    return true;
}

bool TableOfSymbols::addSection(Symbol*& s) {
    if (sectionExists(s->getName()))
        return false;
    tableSect.push_back(s);
    if(s->getName().compare("text")==0)
        s->setRWX(1);//read+execute
    else
        s->setRWX(2);//read+write
    return true;
}

bool TableOfSymbols::symbolExists(string name) {
    for (int i = 0; i < tableSymb.size(); i++) {
        if (tableSymb.at(i)->getName().compare(name) == 0)
            return true;
    }
    return false;
}

bool TableOfSymbols::sectionExists(string name) {
    for (int i = 0; i < tableSect.size(); i++) {
        if (tableSect.at(i)->getName().compare(name) == 0)
            return true;
    }
    return false;
}

Symbol*& TableOfSymbols::getSymbolByName(string name) {
    for (int i = 0; i < tableSymb.size(); i++) {
        if (tableSymb.at(i)->getName().compare(name) == 0)
            return tableSymb.at(i);
    }

    return tableSect.at(0);
}

Symbol*& TableOfSymbols::getSectionByName(string name) {
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    for (int i = 0; i < tableSect.size(); i++) {
        if (tableSect.at(i)->getName().compare(name) == 0)
            return tableSect.at(i);
    }

    return tableSect.at(0);
}

bool TableOfSymbols::calculateEQUvalue(Symbol*& symbol) {
    if (symbol->getValue() != -1)
        return false;
    string expression = symbol->getExpressionl();
    int result = 0;
    //copy(expression, symbol->getExpressionl().size(), 0);
    regex operands("([a-z0-9]+)");
    smatch operMatch;
    regex_search(expression, operMatch, operands);
    if (operMatch.size() > 0) {
        string first = operMatch.str(0);
        expression = operMatch.suffix().str();
        regex number("([0-9]+)");
        smatch numMatch;
        regex_search(first, numMatch, number);
        if (numMatch.size() > 0) {//it's a number
            int value = stoi(numMatch.str(0));
            result += value;
        } else {//it's a variable
            Symbol*symb = getSymbolByName(first);
            if (symb->getName().compare("und") == 0 || symb->getValue() == -1)//symbol isn't defined  yet
                return false;
            result += symb->getValue();
        }
        if (expression.length() == 0) {
            symbol->setValue(result);
            return true;
        }
        char operation = expression.at(0);
        expression = expression.substr(1);

        int secondOprValue;
        regex_search(expression, operMatch, operands);
        if (operMatch.size() > 0) {
            string second = operMatch.str(0);
            expression = operMatch.suffix().str();
            regex_search(second, numMatch, number);
            if (numMatch.size() > 0) {//it's a number
                int value = stoi(numMatch.str(0));
                secondOprValue = value;
            } else {//it's a variable
                Symbol*symb = getSymbolByName(second);
                if (symb->getName().compare("und") == 0 || symb->getValue() == -1)//symbol isn't defined  yet
                    return false;
                secondOprValue = symb->getValue();
            }
            if (operation == '+')
                result += secondOprValue;
            else if (operation == '-')
                result -= secondOprValue;
            else {
                cout << "Error! Directive .equ invalid argument!0" << endl;
                exit(-1);
            }
            if (expression.length() == 0) {
                symbol->setValue(result);
                return true;
            }
            operation = expression.at(0);
            expression = expression.substr(1);

            int thirdOprValue;
            regex_search(expression, operMatch, operands);
            if (operMatch.size() > 0) {
                string third = operMatch.str(0);
                expression = operMatch.suffix().str();
                regex_search(third, numMatch, number);
                if (numMatch.size() > 0) {//it's a number
                    int value = stoi(numMatch.str(0));
                    thirdOprValue = value;
                } else {//it's a variable
                    Symbol*symb = getSymbolByName(second);
                    if (symb->getName().compare("und") == 0 || symb->getValue() == -1)//symbol isn't defined  yet
                        return false;
                    thirdOprValue = symb->getValue();
                }
                if (operation == '+')
                    result += thirdOprValue;
                else if (operation == '-')
                    result -= thirdOprValue;
                else {
                    cout << "Error! Directive .equ invalid argument" << endl;
                    exit(-1);
                }
                if (expression.length() == 0) {
                    symbol->setValue(result);
                    return true;
                } else {
                    cout << "Error! Directive .equ can't have  more than 3 arguments!1" << endl;
                    exit(-1);
                }

            }
        } else {
            cout << "Error! Directive .equ invalid argument!1" << endl;
            exit(-1);
        }
    } else {
        cout << "Error! Directive .equ invalid argument!2" << endl;
        exit(-1);
    }

    return false;
}

void TableOfSymbols::calculateEQUsymbols() {
    if (numEQUnotDefined == 0)
        return;
    int numOfSymbols = numEQUnotDefined;
    for (int i = 0; i < tableSymb.size(); i++) {
        if (tableSymb.at(i)->getExpressionl().size() > 0 && tableSymb.at(i)->getValue() == -1) {
            bool calculated = calculateEQUvalue(tableSymb.at(i));
            if (calculated) {
                numOfSymbols--;
            }
        }
    }
    if (numOfSymbols == numEQUnotDefined) {
        cout << "Error! Symbol couldn't be calculated";
        exit(-1);
    } else {
        numEQUnotDefined = numOfSymbols;
        calculateEQUsymbols();

    }
}

void TableOfSymbols::checkIfSymbDef(){
    for (int  i=0; i<tableSymb.size();i++){
        if(tableSymb.at(i)->getLocality()!=2 &&  tableSymb.at(i)->getValue()==-1){
            cout<<"Error! Symbol isn't defined!"<<endl;
            exit(-1);
        }
            
    }
    
}

void TableOfSymbols::printTable() {
    cout << "Id" << '\t' << "Name" << '\t' << "Section" << '\t' << "Value" << '\t' << "Is local " << "Size" << '\t' << "RWXP" << endl;
    for (int i = 0; i < tableSect.size(); i++) {
        tableSect.at(i)->printSymbol();

    }
    for (int i = 0; i < tableSymb.size(); i++) {
        tableSymb.at(i)->printSymbol();

    }
}

void TableOfSymbols::printTable(ofstream& file){
    file << "Id" << '\t' << "Name" << '\t' << "Section" << '\t' << "Value" << '\t' << "Is local " << "Size" << '\t' << "RWXP" << endl;
    for (int i = 0; i < tableSect.size(); i++) {
        tableSect.at(i)->printSymbol(file);

    }
    for (int i = 0; i < tableSymb.size(); i++) {
        tableSymb.at(i)->printSymbol(file);

    }
    
}