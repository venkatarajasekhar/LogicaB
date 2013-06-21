#include "automaton.h"
#include "main.h"

#include <iostream>
#include <string>
#include <list>

extern std::string *automataName;
extern Automaton **automata;

enum TransitionType {
	SYMBOL,
	SUBMACHINE,
	TOKEN
};

//private constructor
Automaton::Automaton(std::string name) {
	this->name = name;
}

Automaton::Automaton(std::string name, std::istream & readFile) {
	this->name = name;
	readStatesFromFile(readFile);
	readSymbolsFromFile(readFile);
	readFile.get();
	readTransitionsFromFile(readFile);
}

void Automaton::readStatesFromFile(std::istream & readFile) {
	int n;
	bool insideQuotes, isFinal;
	std::string line;
	std::string stateName;


	readFile >> n;
	readFile >> line;

	insideQuotes = false;
	isFinal = false;
	for(int i = 0; i < line.size(); i++) {
		if(line[i] == '\"') {

			if(insideQuotes) {
				addState(stateName, isFinal);
				stateName.clear();
			}	//closing quote

			else {
				if(line[i + 1] == '*'){
					i++;
					isFinal = true;
				}
				else
					isFinal = false;
			}	//opening quote

			insideQuotes = !insideQuotes;
		}

		else {

			if(insideQuotes) {
				stateName += line[i];
			}
		}

	}
}

void Automaton::readSymbolsFromFile(std::istream & readFile) {
	int n;
	bool insideQuotes;
	std::string line;
	std::string symbolName;


	readFile >> n;
	readFile >> line;

	insideQuotes = false;
	for(int i = 0; i < line.size(); i++) {
		if(line[i] == '\"') {
			if(insideQuotes) {
				addSymbol(symbolName);
				symbolName.clear();
			}	//closing quote

			insideQuotes = !insideQuotes;
		}

		else {
			if(insideQuotes) {
				symbolName += line[i];
			}
		}

	}
}

int Automaton::readTransitionsFromFile(std::istream & readFile) {
	int i;
	TransitionType transitionType;
	std::string line;

	std::string stateFrom, transition, stateTo;

	getline(readFile, line);
	while(line != "") {
		stateFrom.clear();
		transition.clear();
		stateTo.clear();

		//begin
		i = 0;

		//state names must start with a "
		if(line[i] != '\"')
			return -1;
		i++;

		//read the state name
		while(line[i] != '\"') {
			stateFrom += line[i];
			i++;
		}

		//read the "
		i++;

		//read the comma
		if(line[i] != ',')
			return -1;
		i++;

		//select transition type
		if(line[i] == '\"') {
			transitionType = SYMBOL;
			i++;
		}
		else if(line[i] == '<') {
			transitionType = SUBMACHINE;
			i++;
		}
		else
			transitionType = TOKEN;
		

		//get the transition
		if(transitionType == SYMBOL){
			while(line[i] != '\"') {
				transition += line[i];
				i++;
			}
			i++;
		}
		else if(transitionType == SUBMACHINE) {
			while(line[i] != '>') {
				transition += line[i];
				i++;
			}
			i++;
		}
		else if(transitionType == TOKEN) {
			while(line[i] != ',') {
				transition += line[i];
				i++;
			}
		}


		//read the comma
		if(line[i] != ',')
			return -1;
		i++;

		//state names must start with a "
		if(line[i] != '\"')
			return -1;
		i++;

		//read the state name
		while(line[i] != '\"') {
			stateTo += line[i];
			i++;
		}

		//read the "
		i++;

		
		if(transitionType == SYMBOL)
			addSymbolTransition(stateFrom, transition, stateTo);
		else if(transitionType == SUBMACHINE)
			addSubmachineTransition(stateFrom, transition, stateTo);
		//else if(transitionType == TOKEN)
			//std::cout << "TOKEN "  << '\"' << stateFrom << "\"," << transition << ",\"" << stateTo << '\"' << std::endl;
		

		//gets the next line
		getline(readFile, line);
	}

	return 0;
}

void Automaton::addState(std::string state, bool isFinal) {
	vStates.push_back(state);
	vFinal.push_back(isFinal);

	mSymbolTransitions.resize(getNStates());
	mSymbolTransitions[getNStates() - 1].resize(getNSymbols());

	mSubmachineTransitions.resize(getNStates());
	mSubmachineTransitions[getNStates() - 1].resize(getNMachines());
}

void Automaton::addSymbol(std::string symbol) {
	vSymbols.push_back(symbol);
	
	for(int i = 0; i < mSymbolTransitions.size(); i++)
		mSymbolTransitions[i].resize(vSymbols.size());
}

void Automaton::addSymbolTransition(std::string stateFrom, std::string symbol, std::string stateTo){
	mSymbolTransitions[findStateId(stateFrom)][findSymbolId(symbol)].push_back(findStateId(stateTo));
}

void Automaton::addSubmachineTransition(std::string stateFrom, std::string machine, std::string stateTo){
	mSubmachineTransitions[findStateId(stateFrom)][findMachineId(machine)].push_back(findStateId(stateTo));
}

int Automaton::findStateId(std::string state) {
	for(int i = 0; i < vStates.size(); i++) {
		if(vStates[i] == state)
			return i;
	}
	return -1;
}

int Automaton::findSymbolId(std::string symbol) {
	for(int i = 0; i < vSymbols.size(); i++) {
		if(vSymbols[i] == symbol)
			return i;
	}
	return -1;
}

void Automaton::printTransitionsTable(std::ostream & output) {
	//sorts mSymbolTransitions and removes duplicates
	for(int i = 0; i < mSymbolTransitions.size(); i++)
		for(int j = 0; j < mSymbolTransitions[i].size(); j++) {
			mSymbolTransitions[i][j].sort();
			mSymbolTransitions[i][j].unique();
		}

	//sorts mSubmachineTransitions and removes duplicates
	for(int i = 0; i < mSubmachineTransitions.size(); i++)
		for(int j = 0; j < mSubmachineTransitions[i].size(); j++) {
			mSubmachineTransitions[i][j].sort();
			mSubmachineTransitions[i][j].unique();
		}


	for(int j = 0; j < getNSymbols(); j++) {
		output << "\t\"" << vSymbols[j] << "\"";
	}
	for(int j = 0; j < getNMachines(); j++) {
		output << "\t<" << automataName[0] << ">";
	}
	output << std::endl;

	for(int i = 0; i < vStates.size(); i++) {
		if(vFinal[i])
			output << "\"*" << vStates[i] << "\"";
		else
			output << "\"" << vStates[i] << "\"";

		for(int j = 0; j < getNSymbols(); j++) {
			output << "\t";

			int k = 0;
			for(std::list<int>::iterator it = mSymbolTransitions[i][j].begin(); it != mSymbolTransitions[i][j].end(); it++) {
				if(k != 0)
					output << ",";

				output << "\"" << vStates[*it] << "\"";
				k++;
			}
		}

		for(int j = 0; j < getNMachines(); j++) {
			output << "\t";

			int k = 0;
			for(std::list<int>::iterator it = mSubmachineTransitions[i][j].begin(); it != mSubmachineTransitions[i][j].end(); it++) {
				if(k != 0)
					output << ",";

				output << "\"" << vStates[*it] << "\"";
				k++;
			}
		}
		output << std::endl;
	}
}

void Automaton::copyOutgoingTransitions(int stateIdFrom, int stateIdTo) {
	for(int i  = 0; i < getNSymbols(); i++) {
		for(std::list<int>::iterator it = mSymbolTransitions[stateIdFrom][i].begin(); it != mSymbolTransitions[stateIdFrom][i].end(); it++) {
			mSymbolTransitions[stateIdTo][i].push_back(*it);
		}

		mSymbolTransitions[stateIdTo][i].sort();
		mSymbolTransitions[stateIdTo][i].unique();
	}

	for(int i  = 0; i < getNMachines(); i++) {
		for(std::list<int>::iterator it = mSubmachineTransitions[stateIdFrom][i].begin(); it != mSubmachineTransitions[stateIdFrom][i].end(); it++) {
			mSubmachineTransitions[stateIdTo][i].push_back(*it);
		}

		mSubmachineTransitions[stateIdTo][i].sort();
		mSubmachineTransitions[stateIdTo][i].unique();
	}
}


void Automaton::removeIncomingEmptyTransitions(int stateId) {
	int j = findSymbolId("");
	if(j != -1) {
		for(int i  = 0; i < getNStates(); i++) {
			mSymbolTransitions[i][j].clear();
		}
	}
}

//retorna a maior posição da string onde foi encontrado um estado de aceitação
int Automaton::testString(const std::string & string) {
	return testString(string, 0, 0);
}

//retorna a maior posição da string onde foi encontrado um estado de aceitação
int Automaton::testString(const std::string & string, int strPos, int stateId) {
	int result = 0;
	int tmp;
	int symbolId;
	int len;


	if(vFinal[stateId])
		result = strPos;

	if(result == string.size())
		return result;

	//symbol transitions
	for(len = 0; len < (string.size() - strPos + 1); len++) {

		int symbolId = findSymbolId(string.substr(strPos, len));

		if(symbolId != -1) {
			for(std::list<int>::iterator it = mSymbolTransitions[stateId][symbolId].begin(); it != mSymbolTransitions[stateId][symbolId].end(); it++) {
				result = std::max(result, testString(string, strPos + len, *it));
			}
		}
	}

	//submachine transitions
	for(int submachineId = 0; submachineId < getNMachines(); submachineId++) {
		for(std::list<int>::iterator it = mSubmachineTransitions[stateId][submachineId].begin(); it != mSubmachineTransitions[stateId][submachineId].end(); it++) {
			//calls the submachine
			tmp = automata[submachineId]->testString(string, strPos, 0);

			//continues solving the automaton from where the submachine has stoped
			result = std::max(result, testString(string, tmp, *it));
		}
	}


	return result;
}

int Automaton::getNStates() {
	return vStates.size();
}

int Automaton::getNSymbols() {
	return vSymbols.size();
}
