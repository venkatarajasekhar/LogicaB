#ifndef __AUTOMATON_H__
#define __AUTOMATON_H__

#include <vector>
#include <string>
#include <fstream>
#include <list>

class Automaton {
private:
	Automaton(std::string name);

	std::string name;
	std::vector<std::string> vStates;
	std::vector<bool> vFinal;
	std::vector<std::string> vSymbols;
	std::vector< std::vector< std::list < int > > > mSymbolTransitions;
	std::vector< std::vector< std::list < int > > > mSubmachineTransitions;

	void addState(std::string state, bool isFinal);
	void addSymbol(std::string symbol); 
	void addSymbolTransition(std::string stateFrom, std::string symbol, std::string stateTo);
	void addSubmachineTransition(std::string stateFrom, std::string machine, std::string stateTo);

	int findStateId(std::string state);
	int findSymbolId(std::string symbol);

	void readStatesFromFile(std::istream & readFile);
	void readSymbolsFromFile(std::istream & readFile);
	int readTransitionsFromFile(std::istream & readFile);

	void copyOutgoingTransitions(int stateIdFrom, int stateIdTo);
	void removeIncomingEmptyTransitions(int stateId);

	int testString(const std::string & string, int strPos, int stateId);

	int getNStates();
	int getNSymbols();

public:
	Automaton(std::string name, std::istream & readFile);
	void printTransitionsTable(std::ostream & output);

	int testString(const std::string & string);

};

#endif //__AUTOMATON_H__
