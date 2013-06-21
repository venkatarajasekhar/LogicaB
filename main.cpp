#include <iostream>
#include <fstream>
#include <string>

#include "automaton.h"

int nAutomata;
std::string *automataName;
Automaton **automata;

int getNMachines() {
	return nAutomata;
}

int findMachineId(std::string machineName) {
	for(int i = 0; i < nAutomata; i++)
		if(automataName[i] == machineName)
			return i;

	std::cerr << "getMachineId looking for a non-existent machine." << std::endl;
	return -1;
}

void readAutoamata(std::istream & readFile) {
	int cont;
	bool insideQuotes;
	std::string line;


	readFile >> nAutomata;
	readFile >> line;

	automata = new Automaton*[nAutomata];
	automataName = new std::string[nAutomata];


	/* read the automaton names */
	cont = 0;
	insideQuotes = false;
	for(int i = 0; i < line.size(); i++) {
		if(line[i] == '\"') {
			if(insideQuotes) {
				cont++;
			}	//closing quote

			insideQuotes = !insideQuotes;
		}

		else {
			if(insideQuotes) {
				automataName[cont] += line[i];
			}
		}

	}

	for(int i = 0; i < nAutomata; i++) {
		automata[i] = new Automaton(automataName[i], readFile);
	}
}

int main() {
	std::string automatonFilename;
	std::ifstream automatonFile;
	std::string string = "a";

	std::cout << "Digite o nome do arquivo que contem o automato." << std::endl << ">> ";
	std::cin >> automatonFilename;
	std::cout << std::endl;
	std::cout << std::endl;

	automatonFile.open(automatonFilename.c_str(), std::ifstream::in);

	readAutoamata(automatonFile);

	std::cout << "====== " << automataName[0] << " ======" << std::endl;
	automata[0]->printTransitionsTable(std::cout);
	std::cout << std::endl;

	if(automata[0]->testString(string) == string.length())
		std::cout << "A cadeia foi aceita." << std::endl;
	else
		std::cout << "A cadeia nao foi aceita." << std::endl;
	std::cout << "====== " << automataName[0] << " ======" << std::endl << std::endl << std::endl;

	return 0;
}
