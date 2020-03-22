


#include "Compiler.h"

Compiler::Compiler() {
    sections = new string[4];
    sections[0] = "UND";
    sections[1] = "TEXT";
    sections[2] = "DATA";
    sections[3] = "BSS";
    myTable = new TableOfSymbols();
    currentSection = 0;
    locationCounter = 0;
    types = new string[2];
    types[0] = "R_386_32";
    types[1] = "R_386_PC32";
    realTable = new RealocationTable();
}

Compiler::Compiler(const Compiler& orig) {
}

Compiler::~Compiler() {
    delete myTable;
    delete realTable;
    //delete sections;
    //delete types;
}

void Compiler::removeWhiteSpacesFromStart(string& line) {
    if (line.empty())
        return;
    if (line.at(0) == '\n')
        line.resize(0);
    if (line.at(0) == '\t' || line.at(0) == ' ') {
        int i = 0;
        while (i < line.length() && (line.at(i) == '\t' || line.at(i) == ' '))
            i++;
        line.erase(0, i);
    }
}

void Compiler::parseLine(string& line, vector<string>& dst) {
    regex regWhiteSpaces{"(([\\w\\d.&:]+)?)"};
    smatch match;

    string out = "";
    while (regex_search(line, match, regWhiteSpaces)) {
        cout << match.str(1) << endl;
        line = match.suffix().str();
    }
    cout << out;



    /*// This holds the first match
 std::sregex_iterator currentMatch(line.begin(),
         line.end(), regWhiteSpaces);
    
 // Used to determine if there are any more matches
 std::sregex_iterator lastMatch;
    
 // While the current match doesn't equal the last
 while(currentMatch != lastMatch){
     std::smatch match = *currentMatch;
     std::cout << match.str() << "\n";
     currentMatch++;
 }
 std::cout << std::endl;*/
}

void Compiler::FirstRun(ifstream& input) {
    locationCounter = 0;
    string line;
    string label;
    bool labelInANewLine = false;
    bool endExists=false;
    while (getline(input, line)) {
        removeWhiteSpacesFromStart(line);
        if (line.length() == 0 || line.at(0) == '#')
            continue;
        //labels
        regex regLab("([a-z]+:)");
        smatch matchLab;
        regex_search(line, matchLab, regLab);
        if (matchLab.size() > 0) {
            if (currentSection == 0) {//no section
                cout << "Error! Labels have to be defined inside of a section!" << endl;
                exit(-1);
            }

            //*if (labelInANewLine) {
            // cout << "Error! You can't define multiple labels for the same value!" << endl;
            //exit(-1);
            //}*/
            labelInANewLine = false;

            label = matchLab.str(0);
            label.erase(label.size() - 1); //delete :
            line = matchLab.suffix().str();

            Symbol* iter = myTable->getSymbolByName(label);
            if (iter->getName().compare("und") == 0) {
                Symbol* newSymbol = new Symbol(label, sections[currentSection], locationCounter, 1, 0);
                myTable->addSymbol(newSymbol);
                labelInANewLine = false;
            } else if (iter->getValue() == -1 && iter->getLocality() == 0) {//global undefined
                iter->setValue(locationCounter);
                iter->setSection(sections[currentSection]);
                labelInANewLine = false;
            } else if (iter->getValue() == -1 && iter->getLocality() == 2) {//extern defined
                cout << "Error! Extern symbol defined!" << endl;
                exit(-1);
            } else {
                //add if the symbol is  global; its definition should be  added
                cout << "Error! Multiple symbol definition!" << endl;
                exit(-1);
            }
            if (line.size() == 0) {
                labelInANewLine = true;
                continue;
            }


        }

        //directive
        regex regDir("(\\.[a-z]+)");
        smatch matchDir;
        regex_search(line, matchDir, regDir);

        if (matchDir.size() > 0) {
            string directive = matchDir.str(0);
            line = matchDir.suffix().str();
            removeWhiteSpacesFromStart(line);
            processDirective(directive, label, line);
            if (directive.compare("end") == 0){
                endExists=true;             
                break;
            }
            continue;

        }
        if (line.size() != 0 && currentSection != 1) {
            cout << "Error! Instructions can be written only in .text section!" << endl;
            exit(-1);
        }


        //instructions
        cout << line << endl;
        regex oneByteInstr("(halt|ret|iret)");
        smatch instrMach;
        regex_search(line, instrMach, oneByteInstr);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            locationCounter++;
            if (line.size() == 0)
                continue;
            else {
                cout << "Error! Invalid number of operands!" << endl;
                exit(-1);
            }
        }

        regex jmpInstr("(jmp|jeq|jne|jgt|call)");
        regex_search(line, instrMach, jmpInstr);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//skok  +   registarsko indirektno sa  pomerajem
                regex number("([0-9]+)");
                smatch numMatch;
                regex_search(line.substr(2), numMatch, number);
                if (numMatch.size() > 0) {
                    int value = stoi(numMatch.str(0));
                    if (value < 127 && value >= -127)
                        locationCounter += 3;
                    else
                        locationCounter += 4;
                } else {
                    locationCounter += 4;
                }

                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela1("(\\$?[a-z]+)");
                regex_search(line, instrMach, labela1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem("(\\*[0-9]+)");
                regex_search(line, instrMach, mem);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
            }
            cout << "Error! This type of operands are not suported";
            exit(-1);

        }

        regex instrPushInt("(push|int)");
        regex_search(line, instrMach, instrPushInt);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//push int  +  regindpom, &symbol, symbol, *val
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela1("(&?[a-z]+)");
                regex_search(line, instrMach, labela1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem("(\\*[0-9]+)");
                regex_search(line, instrMach, mem);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex number("(\\-?[0-9]+)");
                regex_search(line, instrMach, number);
                if (instrMach.size() > 0) {
                    int value = stoi(instrMach.str(0));
                    line = instrMach.suffix().str();
                    if (value < 127 && value >= -127)
                        locationCounter += 3;
                    else
                        locationCounter += 4;
                }
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }

            cout << "Error! This type of operands are not suported";
            exit(-1);
        }
        regex instrPopNot("(pop|not)");
        regex_search(line, instrMach, instrPopNot);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//pop not +  regindpom + memdir
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela1("([a-z]+)");
                regex_search(line, instrMach, labela1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem("(\\*[0-9]+)");
                regex_search(line, instrMach, mem);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
            }

            cout << "Error! This type of operands are not suported";
            exit(-1);
        }
        regex twoOperands("(mov|add|sub|mul|div|or|xor|and|slh|shr)");
        regex_search(line, instrMach, twoOperands);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//aritm +  regindpom + memdir
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }

                }
                line = instrMach.suffix().str();
                if (line.size() == 0) {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex registar1("(r[0-7]|\\[r[0-7]\\])");
                regex_search(line, instrMach, registar1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 2;
                    if (line.size() == 0) {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                } else {
                    regex labela1("([a-z]+)");
                    regex_search(line, instrMach, labela1);
                    if (instrMach.size() > 0) {
                        line = instrMach.suffix().str();
                        locationCounter += 4;
                        if (line.size() == 0) {
                            cout << "Error! Invalid number of operands!" << endl;
                            exit(-1);
                        }
                    } else {
                        regex mem("(\\*[0-9]+)");
                        regex_search(line, instrMach, mem);
                        if (instrMach.size() > 0) {
                            line = instrMach.suffix().str();
                            locationCounter += 4;
                            if (line.size() == 0) {
                                cout << "Error! Invalid number of operands!" << endl;
                                exit(-1);
                            }
                        } else {
                            cout << "Error! This type of operands are not suported for  the firs operand!";
                            exit(-1);
                        }
                    }
                }

            }

            line = line.substr(2); //remove the comma and the space  after  instruction
            regex registarINDpom1("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom1);
            if (instrMach.size() > 0) {//aritm  +  regindpom, &symbol, symbol, *val
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {
                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela("(&?[a-z]+)");
                regex_search(line, instrMach, labela);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem1("(\\*[0-9]+)");
                regex_search(line, instrMach, mem1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex number("(\\-?[0-9]+)");
                regex_search(line, instrMach, number);
                if (instrMach.size() > 0) {
                    int value = stoi(instrMach.str(0));
                    line = instrMach.suffix().str();
                    if (value < 127 && value >= -127)
                        locationCounter += 3;
                    else
                        locationCounter += 4;
                }
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }

            cout << "Error! This type of operands are not suported for  the second operand!" << endl;
            exit(-1);

        }
        regex instrTestCmp("(test|cmp)");
        regex_search(line, instrMach, instrTestCmp);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//push int  +  regindpom, &symbol, symbol, *val
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0) {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex registar("(r[0-7]|\\[r[0-7]\\])");
                regex_search(line, instrMach, registar);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 2;
                    if (line.size() == 0) {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                } else {
                    regex labela1("(&?[a-z]+)");
                    regex_search(line, instrMach, labela1);
                    if (instrMach.size() > 0) {
                        line = instrMach.suffix().str();
                        locationCounter += 4;
                        if (line.size() == 0) {
                            cout << "Error! Invalid number of operands!" << endl;
                            exit(-1);
                        }
                    } else {
                        regex mem("(\\*[0-9]+)");
                        regex_search(line, instrMach, mem);
                        if (instrMach.size() > 0) {
                            line = instrMach.suffix().str();
                            locationCounter += 4;
                            if (line.size() == 0) {
                                cout << "Error! Invalid number of operands!" << endl;
                                exit(-1);
                            }
                        } else {
                            regex number("(\\-?[0-9]+)");
                            regex_search(line, instrMach, number);
                            if (instrMach.size() > 0) {
                                int value = stoi(instrMach.str(0));
                                line = instrMach.suffix().str();
                                if (value < 127 && value >= -127)
                                    locationCounter += 3;
                                else
                                    locationCounter += 4;
                            } else {
                                cout << "Error! This type of operands are not suported for the first operand!" << endl;
                                exit(-1);
                            }
                            if (line.size() == 0) {
                                cout << "Error! Invalid number of operands!" << endl;
                                exit(-1);
                            }
                        }
                    }
                }

            }
            line = line.substr(2); //remove the space  and coma after  instruction
            regex registarINDpom1("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom1);
            if (instrMach.size() > 0) {//push int  +  regindpom, &symbol, symbol, *val
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar1("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar1);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela1("(&?[a-z]+)");
                regex_search(line, instrMach, labela1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem1("(\\*[0-9])+");
                regex_search(line, instrMach, mem1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex number("(\\-?[0-9]+)");
                regex_search(line, instrMach, number);
                if (instrMach.size() > 0) {
                    int value = stoi(instrMach.str(0));
                    line = instrMach.suffix().str();
                    if (value < 127 && value >= -127)
                        locationCounter += 3;
                    else
                        locationCounter += 4;
                }
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }

            cout << "Error! This type of operands are not suported for  the second operand!!" << endl;
            exit(-1);
        }
        regex instrXchg("(xchg)");
        regex_search(line, instrMach, instrXchg);
        if (instrMach.size() > 0) {
            line = instrMach.suffix().str();
            line = line.substr(1); //remove the space  after  instruction
            regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom);
            if (instrMach.size() > 0) {//pop not +  regindpom + memdir
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0) {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex registar("(r[0-7]|\\[r[0-7]\\])");
                regex_search(line, instrMach, registar);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 2;
                    if (line.size() == 0) {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                } else {
                    regex labela1("([a-z]+)");
                    regex_search(line, instrMach, labela1);
                    if (instrMach.size() > 0) {
                        line = instrMach.suffix().str();
                        locationCounter += 4;
                        if (line.size() == 0) {
                            cout << "Error! Invalid number of operands!" << endl;
                            exit(-1);
                        }
                    } else {
                        regex mem("(\\*[0-9]+)");
                        regex_search(line, instrMach, mem);
                        if (instrMach.size() > 0) {
                            line = instrMach.suffix().str();
                            locationCounter += 4;
                            if (line.size() == 0) {
                                cout << "Error! Invalid number of operands!" << endl;
                                exit(-1);
                            }
                        } else {
                            cout << "Error! This type of operands are not suported for the frst operand!" << endl;
                            exit(-1);
                        }
                    }
                }


            }

            line = line.substr(2); //remove the space  after  instruction
            regex registarINDpom1("(r[0-7]\\[[a-z0-9]+\\])");
            regex_search(line, instrMach, registarINDpom1);
            if (instrMach.size() > 0) {//pop not +  regindpom + memdir
                string oper = instrMach.str(0);
                if (oper.at(0) == '*') {
                    locationCounter += 4;
                } else {
                    regex number("([0-9]+)");
                    smatch numMatch;
                    regex_search(oper.substr(2), numMatch, number);
                    if (numMatch.size() > 0) {
                        int value = stoi(numMatch.str(0));
                        if (value < 127 && value >= -127)
                            locationCounter += 3;
                        else
                            locationCounter += 4;
                    } else {

                        locationCounter += 4;
                    }
                }
                line = instrMach.suffix().str();
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            }
            regex registar1("(r[0-7]|\\[r[0-7]\\])");
            regex_search(line, instrMach, registar1);
            if (instrMach.size() > 0) {
                line = instrMach.suffix().str();
                locationCounter += 2;
                if (line.size() == 0)
                    continue;
                else {
                    cout << "Error! Invalid number of operands!" << endl;
                    exit(-1);
                }
            } else {
                regex labela1("([a-z]+)");
                regex_search(line, instrMach, labela1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem("(\\*[0-9]+)");
                regex_search(line, instrMach, mem);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0) {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
                regex mem1("(\\*[0-9]+)");
                regex_search(line, instrMach, mem1);
                if (instrMach.size() > 0) {
                    line = instrMach.suffix().str();
                    locationCounter += 4;
                    if (line.size() == 0)
                        continue;
                    else {
                        cout << "Error! Invalid number of operands!" << endl;
                        exit(-1);
                    }
                }
            }

            cout << "Error! This type of operands are not suported for  the second operand!" << endl;
            exit(-1);
        }
    }
    
    if(!endExists){
        cout<<"Error! .end  doesn't  exist!"<<endl;
        exit(-1);
    }

    myTable->calculateEQUsymbols();
    myTable->checkIfSymbDef();
    myTable->printTable();
}

void Compiler::processDirective(string& directive, string& label, string & line) {
    directive = directive.substr(1);
    if (directive.compare("align") == 0) {
        if (currentSection == 0 || currentSection == 1) {//no section ||text section
            cout << "Error! Directives can't  be defined inside this section!" << endl;
            exit(-1);
        }
        regex number("([0-9]+)");
        smatch matchNumber;
        regex_search(line, matchNumber, number);
        if (matchNumber.size() > 0) {
            string alignNum = matchNumber.str(0);
            if (line.find_first_of(alignNum) != 0 || (line.size() > alignNum.size() && line.at(alignNum.size()) != '#')) {
                cout << "Error! Invalid arguments for  .align directive!" << endl;
                exit(-1);
            }
            int value = stoi(alignNum);
            locationCounter = locationCounter / value * value + value;
        } else {
            cout << "Error! Invalid arguments for  .align directive!" << endl;
            exit(-1);
        }
        return;
    }
    if (directive.compare("skip") == 0) {
        if (currentSection == 0 || currentSection == 1) {//no section ||text section
            cout << "Error! Directives can't  be defined inside this section!" << endl;
            exit(-1);
        }

        regex number("([0-9]+)");
        smatch matchNumber;
        regex_search(line, matchNumber, number);
        if (matchNumber.size() > 0) {
            string skipNum = matchNumber.str(0);
            if (line.find_first_of(skipNum) != 0 || (line.size() > skipNum.size() && line.at(skipNum.size()) != '#')) {
                cout << "Error! Invalid arguments for .skip directive!" << endl;
                exit(-1);
            }
            int value = stoi(skipNum);
            locationCounter += value;
        } else {
            cout << "Error! Invalid arguments for .skip directive!" << endl;
            exit(-1);
        }
        return;
    }
    if (directive.compare("word") == 0) {
        if (currentSection == 0) {//no section ||text section
            cout << "Error! Directives can't  be defined inside this section!" << endl;
            exit(-1);
        }
        regex number("([0-9]+)");
        regex variable("([a-z]+)");
        smatch matchNumber;
        smatch matchVariable;
        regex_search(line, matchNumber, number);
        regex_search(line, matchVariable, variable);
        if (matchNumber.size() > 0 || matchVariable.size() > 0) {
            if (matchNumber.size() > 0) {
                string charNum = matchNumber.str(0);
                int value = stoi(charNum);
                if (value >= 32767 || value<-32767) {
                    cout << "Error! Invalid arguments for .word directive!" << endl;
                    exit(-1);
                }
            }
            locationCounter += 2;
        } else {
            cout << "Error! Invalid arguments for .word directive!" << endl;
            exit(-1);
        }
    }
    if (directive.compare("byte") == 0) {
        if (currentSection == 0) {//no section ||text section
            cout << "Error! Directives can't  be defined inside this section!" << endl;
            exit(-1);
        }
        regex number("([0-9]+)");
        regex variable("([a-z]+)");
        smatch matchNumber;
        smatch matchVariable;
        regex_search(line, matchNumber, number);
        regex_search(line, matchVariable, variable);
        if (matchNumber.size() > 0 || matchVariable.size() > 0) {
            if (matchNumber.size() > 0) {
                string charNum = matchNumber.str(0);
                int value = stoi(charNum);
                if (value >= 127 || value<-127) {
                    cout << "Error! Invalid arguments for .byte directive!" << endl;
                    exit(-1);
                }
            }
            locationCounter += 1;
        } else {
            cout << "Error! Invalid arguments for .byte directive!" << endl;
            exit(-1);
        }

        return;
    }

    if (directive.compare("text") == 0) {
        Symbol* iter = myTable->getSectionByName(directive);
        if (iter->getName().compare("text") == 0) {
            cout << "Error! Multiple text sections!" << endl;
            exit(-1);
        }
        Symbol*s = new Symbol(directive, "TEXT", -1, 1, 0);
        myTable->addSection(s);
        if (currentSection != 0) {
            iter = myTable->getSectionByName(sections[currentSection]);
            iter->setSize(locationCounter);
        }
        currentSection = 1;
        locationCounter = 0;
        return;
    }
    if (directive.compare("data") == 0) {
        Symbol* iter = myTable->getSectionByName(directive);
        if (iter->getName().compare("data") == 0) {
            cout << "Error! Multiple data sections!" << endl;
            exit(-1);
        }
        Symbol*s = new Symbol(directive, "DATA", -1, 1, 0);
        myTable->addSection(s);
        if (currentSection != 0) {
            iter = myTable->getSectionByName(sections[currentSection]);
            iter->setSize(locationCounter);

        }
        currentSection = 2;
        locationCounter = 0;
        return;
    }
    if (directive.compare("bss") == 0) {
        Symbol* iter = myTable->getSectionByName(directive);

        if (iter->getName().compare("bss") == 0) {
            cout << "Error! Multiple bss sections!" << endl;
            exit(-1);
        }
        Symbol*s = new Symbol(directive, "BSS", -1, 1, 0);
        myTable->addSection(s);
        if (currentSection != 0) {
            iter = myTable->getSectionByName(sections[currentSection]);
            iter->setSize(locationCounter);
        }
        currentSection = 3;
        locationCounter = 0;
        return;
    }
    if (directive.compare("extern") == 0) {
        regex regProm("([a-z]+,? ?)");
        smatch promMatch;
        while (regex_search(line, promMatch, regProm)) {
            string variable = promMatch.str(0);
            regex replace("(,? ?)");
            variable = regex_replace(variable, replace, "");
            Symbol* symbol = myTable->getSymbolByName(variable);
            if (symbol->getName().compare("und") != 0) {
                cout << "Error! Exern symbol definiton found!" << endl;
                exit(-1);
            }
            Symbol*newSymbol = new Symbol(variable, "UND", -1, 2, 0);
            myTable->addSymbol(newSymbol);
            line = promMatch.suffix().str();
        }
        return;
    }

    if (directive.compare("global") == 0) {
        regex regProm("([a-z]+,? ?)");
        smatch promMatch;
        while (regex_search(line, promMatch, regProm)) {
            string variable = promMatch.str(0);
            regex replace("(,? ?)");
            variable = regex_replace(variable, replace, "");
            Symbol* symbol = myTable->getSymbolByName(variable);
            if (symbol->getName().compare(variable) == 0) {
                symbol->setLocality(0);
            } else {
                Symbol*newSymbol = new Symbol(variable, sections[currentSection], -1, 0, 0);
                myTable->addSymbol(newSymbol);
            }
            line = promMatch.suffix().str();
        }

        return;
    }

    if (directive.compare("equ") == 0) {
        regex equRegex("([a-z]+,)");
        smatch equMatch;
        regex_search(line, equMatch, equRegex);
        if (equMatch.size() > 0) {
            string variable = equMatch.str(0); //.erase(0, equMatch.str(0).size() - 1); //MOZDA   -2
            variable.erase(variable.size() - 1);
            Symbol*symb = myTable->getSymbolByName(variable);
            bool isGlobal = false;
            if (symb->getName().compare(variable) == 0 && symb->getLocality() != 0) {
                cout << "Error! Symbol is already defined!" << endl;
                exit(-1);
            } else if (symb->getLocality() == 0) {
                isGlobal = true;
            }
            line = equMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            regex numOpr("([0-9]+)");
            smatch numMatch;
            regex_search(line, numMatch, numOpr);
            if (numMatch.size() > 0) {
                string number = numMatch.str(0);
                if (line.compare(number) == 0) {
                    int value = stoi(number);
                    if (isGlobal) {
                        symb->setValue(value);
                    } else {
                        Symbol*newSymbol = new Symbol(variable, sections[currentSection], value, 1, 0);
                        myTable->addSymbol(newSymbol);
                    }
                    return;
                }
            }
            if (line.length() == 0) {
                cout << "Error! Directive .equ doesn't have a  valid  argument!" << endl;
                exit(-1);
            }

            if (isGlobal) {
                symb->setExpression(line);
                bool done = myTable->calculateEQUvalue(symb);
                if (!done)
                    myTable->incNumOfEQUnotDefined();
            } else {
                Symbol*newSymbol = new Symbol(variable, sections[currentSection], -1, 1, 0);
                myTable->addSymbol(newSymbol);
                newSymbol->setExpression(line);
                bool done = myTable->calculateEQUvalue(newSymbol);
                if (!done)
                    myTable->incNumOfEQUnotDefined();
            }
            return;

        } else {
            cout << "Error! Directive .equ doesn't have a  valid  argument!" << endl;
            exit(-1);
        }
        return;
    }

    if (directive.compare("end") == 0) {
        if (currentSection == 0) {
            cout << "Error! Sections don't exist before  the .end  directive!" << endl;
            exit(-1);
        }
        Symbol*iter = myTable->getSectionByName(sections[currentSection]);
        iter->setSize(locationCounter);
        locationCounter = 0;
        currentSection = 0;
    }



}

void Compiler::SecondRun(ifstream& input) {
    string line;
    currentSection = 0;
    locationCounter = 0;
    bool bssSection = false;
    while (getline(input, line)) {
        removeWhiteSpacesFromStart(line);
        if (line.size() == 0 || line.at(0) == '#')
            continue;

        regex skipSecondPass("(.global|.extern|.equ)");
        smatch skipMatch;
        regex_search(line, skipMatch, skipSecondPass);
        if (skipMatch.size() > 0)
            continue;
        regex bss(".bss");
        regex_search(line, skipMatch, bss);
        if (skipMatch.size() > 0) {
            bssSection = true;
            continue;
        }
        cout << line << endl;


        regex text("(.text)");
        smatch directives;
        regex_search(line, directives, text);
        if (directives.size() > 0) {
            currentSection = 1;
            locationCounter = 0;
            bssSection = false;
            continue;
        }

        regex data("(.data)");
        regex_search(line, directives, data);
        if (directives.size() > 0) {
            currentSection = 2;
            locationCounter = 0;
            bssSection = false;
            continue;
        }

        regex end(".end");
        regex_search(line, directives, end);
        if (directives.size() > 0) {
            locationCounter = 0;
            bssSection = false;
            break;
        }

        if (bssSection)
            continue;

        regex skip("(.skip)");
        regex_search(line, directives, skip);
        if (directives.size() > 0) {
            line = directives.suffix().str();
            regex number("([0-9]+)");
            smatch matchNumber;
            regex_search(line, matchNumber, number);
            int value = stoi(matchNumber.str(0));
            for (int i = 0; i < value; i++) {
                if (currentSection == 1)
                    textWrite.push_back("00");
                else
                    dataWrite.push_back("00");
            }
            locationCounter += value;
            continue;
        }

        regex align("(.align)");
        regex_search(line, directives, align);
        if (directives.size() > 0) {
            line = directives.suffix().str();
            regex number("([0-9]+)");
            smatch matchNumber;
            regex_search(line, matchNumber, number);
            int value = stoi(matchNumber.str(0));
            locationCounter = locationCounter / value * value + value;
            int value1 = locationCounter / value * value + value - locationCounter;
            for (int i = 0; i < value1; i++) {
                if (currentSection == 1)
                    textWrite.push_back("00");
                else
                    dataWrite.push_back("00");
            }
            continue;
        }
        //U KOJOJ SEKCIJI MOGU DA BUDU
        regex byte("(.byte)");
        regex_search(line, directives, byte);
        if (directives.size() > 0) {
            line = directives.suffix().str();
            regex number("([0-9]+)");
            smatch match;
            regex_search(line, match, number);
            if (match.size() > 0) {
                int value = stoi(match.str(0));
                string str = intToHex(value);
                if (currentSection == 1)
                    textWrite.push_back(str);
                else
                    dataWrite.push_back(str);
                continue;
            }
            regex label("([a-z]+)");
            regex_search(line, match, label);
            if (match.size() > 0) {
                string name = match.str(0);
                Symbol*symb = myTable->getSymbolByName(name);
                if (symb->getName().compare(name) == 0) {
                    int value = symb->getValue();
                    if (symb->getLocality() == 2)
                        value = 0;
                    if (value >= -127 && value < 127) {
                        string str = intToHex(value);
                        if (currentSection == 1)
                            textWrite.push_back(str);
                        else
                            dataWrite.push_back(str);
                        createNewReal(symb, 0);
                        continue;
                    } else {
                        cout << "Error! Directive .byte has  an   invalid argument!" << endl;
                        exit(-1);
                    }
                } else {
                    cout << "Error! Directive .byte has  an   invalid argument!" << endl;
                    exit(-1);
                }
            }
        }

        regex word("(.word)");
        regex_search(line, directives, word);
        if (directives.size() > 0) {
            line = directives.suffix().str();
            regex number("([0-9]+)");
            smatch match;
            regex_search(line, match, number);
            if (match.size() > 0) {
                int value = stoi(match.str(0));
                int lower = value & 0xFF;
                int higher = (value >> 8)&0xFF;
                string str1 = intToHex(lower);
                string str2 = intToHex(higher);
                if (currentSection == 1) {
                    textWrite.push_back(str1);
                    textWrite.push_back(str2);
                } else {
                    dataWrite.push_back(str1);
                    dataWrite.push_back(str2);
                }
                continue;
            }
            regex label("([a-z]+)");
            regex_search(line, match, label);
            if (match.size() > 0) {
                string name = match.str(0);
                Symbol*symb = myTable->getSymbolByName(name);
                if (symb->getName().compare(name) == 0) {
                    int value = symb->getValue();
                    if (symb->getLocality() == 2)
                        value = 0;
                    if (value >= -32767 && value < 32767) {
                        int lower = value & 0xFF;
                        int higher = (value >> 8)&0xFF;
                        string str1 = intToHex(lower);
                        string str2 = intToHex(higher);
                        if (currentSection == 1) {
                            textWrite.push_back(str1);
                            textWrite.push_back(str2);
                        } else {
                            dataWrite.push_back(str1);
                            dataWrite.push_back(str2);
                        }
                        createNewReal(symb, 0);
                        continue;
                    } else {
                        cout << "Error! Directive .word has  an   invalid argument!" << endl;
                        exit(-1);
                    }
                } else {
                    cout << "Error! Directive .word has  an   invalid argument!" << endl;
                    exit(-1);
                }
            }

        }

        regex halt("(halt)");
        smatch instrMatch;
        regex_search(line, instrMatch, halt);
        if (instrMatch.size() > 0) {
            int firstByte = 1 << 3;
            string str = intToHex(firstByte);
            textWrite.push_back(str);
            locationCounter++;
            continue;
        }

        regex ret("(ret)");
        regex_search(line, instrMatch, ret);
        if (instrMatch.size() > 0) {
            int firstByte = 24 << 3;
            string str = intToHex(firstByte);
            textWrite.push_back(str);
            locationCounter++;
            continue;
        }

        regex iret("iret");
        regex_search(line, instrMatch, iret);
        if (instrMatch.size() > 0) {
            int firstByte = 25 << 3;
            string str = intToHex(firstByte);
            textWrite.push_back(str);
            locationCounter++;
            continue;
        }

        regex notReg("(not)");
        regex_search(line, instrMatch, notReg);
        if (instrMatch.size() > 0) {
            int opCode = 10 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (memAdr(line, opCode, false))
                continue;

        }
        regex pop("(pop)");
        regex_search(line, instrMatch, pop);
        if (instrMatch.size() > 0) {
            int opCode = 18 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (memAdr(line, opCode, false))
                continue;

        }

        regex push("(push)");
        regex_search(line, instrMatch, push);
        if (instrMatch.size() > 0) {
            int opCode = 17 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (memAdr(line, opCode, false))
                continue;
            if (immAdr(line, opCode, false))
                continue;

        }
        regex intReg("(int)");
        regex_search(line, instrMatch, intReg);
        if (instrMatch.size() > 0) {
            int opCode = 3 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (memAdr(line, opCode, false))
                continue;
            if (immAdr(line, opCode, false))
                continue;

        }

        regex jmp("(jmp)");
        regex_search(line, instrMatch, jmp);
        if (instrMatch.size() > 0) {
            int opCode = 19 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (pcRelAdr(line, opCode, false))
                continue;
            if (memAdr(line, opCode, false))
                continue;


        }
        regex jeq("(jeq)");
        regex_search(line, instrMatch, jeq);
        if (instrMatch.size() > 0) {
            int opCode = 20 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (pcRelAdr(line, opCode, false))
                continue;
            if (memAdr(line, opCode, false))
                continue;


        }
        regex jne("(jne)");
        regex_search(line, instrMatch, jne);
        if (instrMatch.size() > 0) {
            int opCode = 21 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (pcRelAdr(line, opCode, false))
                continue;
            if (memAdr(line, opCode, false))
                continue;


        }
        regex jgt("(jgt)");
        regex_search(line, instrMatch, jgt);
        if (instrMatch.size() > 0) {
            int opCode = 22 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (pcRelAdr(line, opCode, false))
                continue;
            if (memAdr(line, opCode, false))
                continue;


        }
        regex call("(call)");
        regex_search(line, instrMatch, call);
        if (instrMatch.size() > 0) {
            int opCode = 23 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            bool isRegIndir = regIndirPom(line, opCode, false);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, false) == 1)
                continue;
            if (pcRelAdr(line, opCode, false))
                continue;
            if (memAdr(line, opCode, false))
                continue;


        }

        regex mov("(mov)");
        regex_search(line, instrMatch, mov);
        if (instrMatch.size() > 0) {
            int opCode = 4 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex add("(add)");
        regex_search(line, instrMatch, add);
        if (instrMatch.size() > 0) {
            int opCode = 5 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            bool isRegIndir = regIndirPom(line, opCode, true);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex sub("(sub)");
        regex_search(line, instrMatch, sub);
        if (instrMatch.size() > 0) {
            int opCode = 6 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            bool isRegIndir = regIndirPom(line, opCode, true);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex mul("(mul)");
        regex_search(line, instrMatch, mul);
        if (instrMatch.size() > 0) {
            int opCode = 7 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);
            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            bool isRegIndir = regIndirPom(line, opCode, true);
            if (isRegIndir == 1)
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex div("(div)");
        regex_search(line, instrMatch, div);
        if (instrMatch.size() > 0) {
            int opCode = 8 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex andReg("(and)");
        regex_search(line, instrMatch, andReg);
        if (instrMatch.size() > 0) {
            int opCode = 11 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex xorReg("(xor)");
        regex_search(line, instrMatch, xorReg);
        if (instrMatch.size() > 0) {
            int opCode = 13 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex orReg("(or)");
        regex_search(line, instrMatch, orReg);
        if (instrMatch.size() > 0) {
            int opCode = 12 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex shl("(shl)");
        regex_search(line, instrMatch, shl);
        if (instrMatch.size() > 0) {
            int opCode = 15 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex shr("(shr)");
        regex_search(line, instrMatch, shr);
        if (instrMatch.size() > 0) {
            int opCode = 16 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }

        regex xchg("(xchg)");
        regex_search(line, instrMatch, xchg);
        if (instrMatch.size() > 0) {
            int opCode = 2 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    memAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;

        }

        regex cmp("(cmp)");
        regex_search(line, instrMatch, cmp);
        if (instrMatch.size() > 0) {
            int opCode = 9 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    if (!memAdr(line, opCode, false))
                        immAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }
        regex test("(test)");
        regex_search(line, instrMatch, test);
        if (instrMatch.size() > 0) {
            int opCode = 14 << 3;
            line = instrMatch.suffix().str();
            removeWhiteSpacesFromStart(line);

            if (!regIndirPom(line, opCode, false)) {
                if (!regIndirDir(line, opCode, false))
                    if (!memAdr(line, opCode, false))
                        immAdr(line, opCode, false);
            }

            line = line.substr(2); //uklonimo , i razmak 

            if (regIndirPom(line, opCode, true))
                continue;
            if (regIndirDir(line, opCode, true) == 1)
                continue;
            if (memAdr(line, opCode, true))
                continue;
            if (immAdr(line, opCode, true))
                continue;
        }


    }
    realTable->printRelTable();
}

bool Compiler::regIndirPom(string& line, int opCode, bool secondOp) {
    regex registarINDpom("(r[0-7]\\[[a-z0-9]+\\])");
    smatch adressType;
    regex_search(line, adressType, registarINDpom);
    if (adressType.size() > 0) {
        int regNum = (line.at(1) - '0') << 1;
        //line = line.substr(2); //skip rx
        regex number("([0-9]+)");
        smatch adressType1;
        regex_search(line.substr(2), adressType1, number);
        if (adressType1.size() > 0) {
            int pomeraj = stoi(adressType1.str(0));
            if (pomeraj >= -127 && pomeraj < 127) {
                locationCounter += 3;
                opCode &= 0xF8; //op size 1B
                int oprDescr = 3 << 4;
                oprDescr |= regNum;
                string op1 = intToHex(opCode);
                string op2 = intToHex(oprDescr);
                string op3 = intToHex(pomeraj);
                if (!secondOp)//if it's  the second op don't write op code
                    textWrite.push_back(op1);
                textWrite.push_back(op2);
                textWrite.push_back(op3);
                line = adressType.suffix().str();
                return true;
            } else if (pomeraj >= -32767 && pomeraj < 32767) {
                locationCounter += 4;
                opCode &= 0xFC; //op size 2B
                int oprDescr = 4 << 4;
                oprDescr |= regNum;
                int lower = pomeraj & 0xFF;
                int higher = (pomeraj >> 8)&0xFF;
                string str1 = intToHex(lower);
                string str2 = intToHex(higher);
                string op1 = intToHex(opCode);
                string op2 = intToHex(oprDescr);
                string op3 = intToHex(lower);
                string op4 = intToHex(higher);
                if (!secondOp)
                    textWrite.push_back(op1);
                textWrite.push_back(op2);
                textWrite.push_back(op3);
                textWrite.push_back(op4);
                line = adressType.suffix().str();
                return true;

            } else {
                cout << "Error! Displacement  is too  big!" << endl;
                exit(-1);
            }
        }
        regex label("([a-z]+)");
        regex_search(line.substr(2), adressType1, label);
        if (adressType1.size() > 0) {
            string name = adressType1.str(0);
            Symbol*symb = myTable->getSymbolByName(name);
            if (symb->getName().compare(name) == 0) {
                int pomeraj = symb->getValue();
                if (symb->getLocality() == 2)
                    pomeraj = 0;
                if (pomeraj >= -32767 && pomeraj < 32767) {
                    locationCounter += 4;
                    opCode &= 0xFC; //op size 2B
                    int oprDescr = 4 << 4;
                    oprDescr |= regNum;
                    int lower = pomeraj & 0xFF;
                    int higher = pomeraj >> 8;
                    string str1 = intToHex(lower);
                    string str2 = intToHex(higher);
                    string op1 = intToHex(opCode);
                    string op2 = intToHex(oprDescr);
                    string op3 = intToHex(lower);
                    string op4 = intToHex(higher);
                    if (!secondOp)
                        textWrite.push_back(op1);
                    textWrite.push_back(op2);
                    textWrite.push_back(op3);
                    textWrite.push_back(op4);
                    createNewReal(symb, 0);
                    line = adressType.suffix().str();
                    return true;
                } else {
                    cout << "Error! Displacement  is too  big!" << endl;
                    exit(-1);
                }

            } else {
                cout << "Error! Symbol name  doesn't exist!" << endl;
                exit(-1);
            }
        }
    }
    return false;
}

bool Compiler::regIndirDir(string& line, int opCode, bool secondOp) {
    regex registar("(r[0-7]|\\[r[0-7]\\])");
    smatch instrMatch;
    regex_search(line, instrMatch, registar);
    if (instrMatch.size() > 0) {
        int regNum;
        int oprDesc;
        opCode &= 0xF8; //1B
        if (line.at(1) == '[') {
            regNum = (line.at(2) - '0') << 1;
            oprDesc = 2 << 4;
            oprDesc |= regNum;
        } else {
            regNum = (line.at(1) - '0') << 1;
            oprDesc = 1 << 4;
            oprDesc |= regNum;
        }
        string opr1 = intToHex(opCode);
        string opr2 = intToHex(oprDesc);
        if (!secondOp)
            textWrite.push_back(opr1);
        textWrite.push_back(opr2);
        line = instrMatch.suffix().str();
        return true;
    }
    return false;
}

bool Compiler::pcRelAdr(string& line, int opCode, bool secondOp) {
    regex adress("(\\$[a-z]+)");
    smatch match;
    regex_search(line, match, adress);
    if (match.size()) {
        string name = match.str(0).substr(1);
        Symbol*symb = myTable->getSymbolByName(name);
        if (symb->getName().compare(name) == 0) {
            locationCounter += 4;
            int value;
            if (symb->getLocality() == 1 && symb->getSection().compare("TEXT") == 0) {
                value = -locationCounter + symb->getValue();

            }
            if (symb->getLocality() == 1 && symb->getSection().compare("TEXT") != 0) {
                value = symb->getValue() - 2;
                createNewReal(symb, 1);
            }
            if (symb->getLocality() == 0) {
                value = -2;
                createNewReal(symb, 1);
            }
            opCode &= 0xFC; //op size 2B
            int oprDescr = 4 << 4;
            int lower = value & 0xFF;
            int higher = (value >> 8)&0xFF;
            string str1 = intToHex(lower);
            string str2 = intToHex(higher);
            string op1 = intToHex(opCode);
            string op2 = intToHex(oprDescr);
            string op3 = intToHex(lower);
            string op4 = intToHex(higher);
            if (!secondOp)
                textWrite.push_back(op1);
            textWrite.push_back(op2);
            textWrite.push_back(op3);
            textWrite.push_back(op4);

            line = match.suffix().str();
            return true;


        } else {
            cout << "Error! Symbol is not defined!" << endl;
            exit(-1);
        }

        line = match.suffix().str();
        return true;
    }
    return false;
}

bool Compiler::memAdr(string& line, int opCode, bool secondOp) {
    regex label("([a-z]+)");
    smatch match;
    regex_search(line, match, label);
    if (match.size() > 0) {
        string name = match.str();
        Symbol*symb = myTable->getSymbolByName(name);
        if (symb->getName().compare(name) == 0) {
            locationCounter += 4;
            int value = symb->getValue();
            if (symb->getLocality() == 2)
                value = 0;
            opCode &= 0xFC; //op size 2B
            int oprDescr = 5 << 4;
            int lower = value & 0xFF;
            int higher = (value >> 8)&0xFF;
            string str1 = intToHex(lower);
            string str2 = intToHex(higher);
            string op1 = intToHex(opCode);
            string op2 = intToHex(oprDescr);
            string op3 = intToHex(lower);
            string op4 = intToHex(higher);
            if (!secondOp)
                textWrite.push_back(op1);
            textWrite.push_back(op2);
            textWrite.push_back(op3);
            textWrite.push_back(op4);
            line = match.suffix().str();
            createNewReal(symb, 0);
            return true;
        } else {
            cout << "Error! Symbol is not defined!" << endl;
            exit(-1);
        }
    }
    regex number("(\\*[0-9]+)");
    regex_search(line, match, number);
    if (match.size() > 0) {
        int value = stoi(match.str(0).substr(1));

        locationCounter += 4;
        opCode &= 0xFC; //op size 2B
        int oprDescr = 5 << 4;
        int lower = value & 0xFF;
        int higher = (value >> 8)&0xFF;
        string str1 = intToHex(lower);
        string str2 = intToHex(higher);
        string op1 = intToHex(opCode);
        string op2 = intToHex(oprDescr);
        string op3 = intToHex(lower);
        string op4 = intToHex(higher);
        if (!secondOp)
            textWrite.push_back(op1);
        textWrite.push_back(op2);
        textWrite.push_back(op3);
        textWrite.push_back(op4);
        line = match.suffix().str();
        return true;

    }
    return false;

}

bool Compiler::immAdr(string& line, int opCode, bool secondOp) {
    regex label("(&[a-z]+)");
    smatch match;
    regex_search(line, match, label);
    if (match.size() > 0) {
        string name = match.str().substr(1);
        Symbol*symb = myTable->getSymbolByName(name);
        if (symb->getName().compare(name) == 0) {
            int value = symb->getValue();
            if (symb->getLocality() == 2)//externi
                value = 0;
            if (value >= -32767 && value < 32767) {
                locationCounter += 4;
                opCode &= 0xFC; //op size 2B
                int oprDescr = 0 << 4;
                int lower = value & 0xFF;
                int higher = (value >> 8)&0xFF;
                string str1 = intToHex(lower);
                string str2 = intToHex(higher);
                string op1 = intToHex(opCode);
                string op2 = intToHex(oprDescr);
                string op3 = intToHex(lower);
                string op4 = intToHex(higher);
                if (!secondOp)
                    textWrite.push_back(op1);
                textWrite.push_back(op2);
                textWrite.push_back(op3);
                textWrite.push_back(op4);
                line = match.suffix().str();
                createNewReal(symb, 0);
                return true;
            } else {
                cout << "Error! Value is too big!" << endl;
                exit(-1);
            }
        } else {
            cout << "Error! Symbol is not defined!" << endl;
            exit(-1);
        }
    }
    regex number("(\\-?[0-9]+)");
    regex_search(line, match, number);
    if (match.size() > 0) {
        int value = stoi(match.str(0));
        if (value >= -32767 && value < 32767) {
            locationCounter += 4;
            opCode &= 0xFC; //op size 2B
            int oprDescr = 0 << 4;
            int lower = value & 0xFF;
            int higher = (value >> 8)&0xFF;
            string str1 = intToHex(lower);
            string str2 = intToHex(higher);
            string op1 = intToHex(opCode);
            string op2 = intToHex(oprDescr);
            string op3 = intToHex(lower);
            string op4 = intToHex(higher);
            if (!secondOp)
                textWrite.push_back(op1);
            textWrite.push_back(op2);
            textWrite.push_back(op3);
            textWrite.push_back(op4);
            line = match.suffix().str();
            return true;
        } else {
            cout << "Error! Value is too big!" << endl;
            exit(-1);
        }
    }
    return false;
}

string Compiler::intToHex(int num) {
    stringstream stream;
    stream << "0x" << setfill('0') << setw(2) << hex << num << " ";
    return stream.str().substr(2);

}

void Compiler::createNewReal(Symbol*& symbol, int type) {
    int id;
    RelRow*row;
    if (symbol->getLocality() == 1) {//lokalan
        id = myTable->getSectionByName(symbol->getSection())->getId();
        row = new RelRow(id, types[type], locationCounter);
    } else {
        id = symbol->getId();
        row = new RelRow(id, types[type], 0);
    }

    if (currentSection == 1)
        realTable->addRelText(row);
    else
        realTable->addRelData(row);

}

void Compiler::printToFile(ofstream& file) {
    myTable->printTable(file);
    realTable->printRelTable(file);
    file << "#.text" << endl;
    for (int i = 0; i < textWrite.size(); i++) {
        file << textWrite.at(i) << " ";
    }
    file << endl;
    file << "#.data" << endl;
    for (int i = 0; i < dataWrite.size(); i++) {
        file << dataWrite.at(i) << " ";
    }
    file << endl;
}