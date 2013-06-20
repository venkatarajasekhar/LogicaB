#ifndef __AUTOMATON_H__
#define __AUTOMATON_H__

#include <vector>
#include <string>
#include <fstream>
#include <list>

class Automaton {
private:
	Automaton();


	std::vector<std::string> vStates;
	std::vector<bool> vFinal;
	std::vector<std::string> vSymbols;
	std::vector< std::vector< std::list < int > > > mTransitions;

	void addState(std::string state, bool isFinal);
	void addSymbol(std::string symbol); 

	int findStateId(std::string state);
	int findSymbolId(std::string symbol);

	void readStatesFromFile(std::istream & readFile);
	void readSymbolsFromFile(std::istream & readFile);
	void readTransitionsFromFile(std::istream & readFile);

	void copyOutgoingTransitions(int stateIdFrom, int stateIdTo);
	void removeIncomingEmptyTransitions(int stateId);

	int testString(const std::string & string, int strPos, int stateId);

	int getNStates();
	int getNSymbols();

public:
	Automaton(std::istream & readFile);
	void printTransitionsTable(std::ostream & output);

	int testString(const std::string & string);

	Automaton * getDeterministic();
};

#endif //__AUTOMATON_H__
