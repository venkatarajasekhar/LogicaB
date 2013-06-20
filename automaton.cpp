#include "automaton.h"

#include <iostream>
#include <string>
#include <list>


//private constructor
Automaton::Automaton() {

}

Automaton::Automaton(std::istream & readFile) {
	readStatesFromFile(readFile);
	readSymbolsFromFile(readFile);
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

void Automaton::readTransitionsFromFile(std::istream & readFile) {
	int k, transition[3];
	bool insideQuotes;
	std::string tmp;
	std::string line;
	


	while(readFile.good()) {
		readFile >> line;

		k = 0;
		insideQuotes = false;
		for(int i = 0; i < line.size(); i++) {
			if(line[i] == '\"') {
				if(insideQuotes) {
					if(k == 0)
						transition[k++] = findStateId(tmp);
					else if(k == 1)
						transition[k++] = findSymbolId(tmp);
					else {
						transition[k] = findStateId(tmp);
						mTransitions[transition[0]][transition[1]].push_back(transition[2]);
						k = 0;
					}
					
					tmp.clear();
				}	//closing quote

				insideQuotes = !insideQuotes;
			}

			else {
				if(insideQuotes) {
					tmp += line[i];
				}
			}

		}
	}
}

void Automaton::addState(std::string state, bool isFinal) {
	vStates.push_back(state);
	vFinal.push_back(isFinal);

	mTransitions.resize(vStates.size());
	mTransitions[vStates.size() - 1].resize(vSymbols.size());
}

void Automaton::addSymbol(std::string symbol) {
	vSymbols.push_back(symbol);
	
	for(int i = 0; i < mTransitions.size(); i++)
		mTransitions[i].resize(vSymbols.size());
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
	//sorts mTransitions and removes duplicates
	for(int i = 0; i < mTransitions.size(); i++)
		for(int j = 0; j < mTransitions[i].size(); j++) {
			mTransitions[i][j].sort();
			mTransitions[i][j].unique();
		}


	for(int j = 0; j < vSymbols.size(); j++) {
		output << "\t\"" << vSymbols[j] << "\"";
	}
	output << std::endl;

	for(int i = 0; i < vStates.size(); i++) {
		if(vFinal[i])
			output << "\"*" << vStates[i] << "\"";
		else
			output << "\"" << vStates[i] << "\"";

		for(int j = 0; j < vSymbols.size(); j++) {
			output << "\t";

			int k = 0;
			for(std::list<int>::iterator it = mTransitions[i][j].begin(); it != mTransitions[i][j].end(); it++) {
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
		for(std::list<int>::iterator it = mTransitions[stateIdFrom][i].begin(); it != mTransitions[stateIdFrom][i].end(); it++) {
			mTransitions[stateIdTo][i].push_back(*it);
		}

		mTransitions[stateIdTo][i].sort();
		mTransitions[stateIdTo][i].unique();
	}
}

void Automaton::removeIncomingEmptyTransitions(int stateId) {
	int j = findSymbolId("");
	if(j != -1) {
		for(int i  = 0; i < getNStates(); i++) {
			mTransitions[i][j].clear();
		}
	}
}

int Automaton::testString(const std::string & string) {
	return testString(string, 0, 0);
}

int Automaton::testString(const std::string & string, int strPos, int stateId) {
	int result = 0;
	int symbolId;
	int len;

	if((strPos == string.size()) && vFinal[stateId])
		result++;

	for(len = 0; len < (string.size() - strPos + 1); len++) {

		int symbolId = findSymbolId(string.substr(strPos, len));

		if(symbolId != -1) {
			for(std::list<int>::iterator it = mTransitions[stateId][symbolId].begin(); it != mTransitions[stateId][symbolId].end(); it++) {
				result += testString(string, strPos + len, *it);
			}
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

Automaton * Automaton::getDeterministic() {
	Automaton * b = new Automaton;
	std::vector< std::vector< int > > mEmptyTransitions;
	std::vector< int > vEmptyIn;
	std::vector< int > vEmptyOut;

	int i, j, removed;

	b->vStates = vStates;
	b->vFinal = vFinal;
	b->vSymbols = vSymbols;
	b->mTransitions = mTransitions;

	//creates the mEmptyTransitions table
	vEmptyIn.resize(getNStates());
	vEmptyOut.resize(getNStates());
	mEmptyTransitions.resize(getNStates());
	for(i = 0; i < getNStates(); i++)
		mEmptyTransitions[i].resize(getNStates());

	//initialize mEmptyTransitions
	for(i = 0; i < mEmptyTransitions.size(); i++){
		vEmptyIn[i] = 0;
		vEmptyOut[i] = 0;
		for(j = 0; j < mEmptyTransitions[i].size(); j++)
			mEmptyTransitions[i][j] = 0;
	}

	//fills mEmptyTransitons
	j = findSymbolId("");
	if(j != -1) {
		for(int i = 0; i < getNStates(); i++) {
			for(std::list<int>::iterator it = mTransitions[i][j].begin(); it != mTransitions[i][j].end(); it++) {
				vEmptyIn[*it]++;
				vEmptyOut[i]++;
				mEmptyTransitions[i][*it]++;
			}	
		}
	}

	//finds a state 'i' with incoming empty transitions, but no outgoing empty transitions,
	//copy its transtions to states 'j' able to reach 'i' and removes these empty transitions
	do {
		removed = 0;

		for(i = 0; i < getNStates(); i++) {
			if((vEmptyIn[i] > 0) && (vEmptyOut[i] == 0)) {
				for(j = 0; j < getNStates(); j++) {
					if(i == j)
						continue;

					if(mEmptyTransitions[j][i] > 0) {
						b->copyOutgoingTransitions(i, j);
						b->removeIncomingEmptyTransitions(i);
						if(b->vFinal[i] == true)
							b->vFinal[j] = true;

						mEmptyTransitions[j][i]--;
						vEmptyIn[i]--;
						vEmptyOut[j]--;
						removed++;
					}
					
				}
				
			}
		}

	} while(removed > 0);
	

	//finds a state 'i' that has a non-determinism (two or more outputs for the same symbol),
	//creates a state representing all those symbols and change the transition to this one
	do {
		removed = 0;

		for(i = 0; i < b->getNStates(); i++) {
			for(j = 0; j < b->getNSymbols(); j++) {
				if(b->mTransitions[i][j].size() > 1) {
					int newStateId;
					std::string newStateName;
					bool isFinal;

					//fills newStateName and isFinal
					newStateName = b->vStates[mTransitions[i][j].front()];
					std::list<int>::iterator it = b->mTransitions[i][j].begin();
					isFinal = vFinal[*it];
					++it;
					for(; it != b->mTransitions[i][j].end(); it++) {
						newStateName += "_" + b->vStates[*it];
						isFinal = isFinal || vFinal[*it];
					}

					//guarantees the newStateName is unique
					while(b->findStateId(newStateName) != -1)
						newStateName += '_';

					//adds newStateName to b
					b->addState(newStateName, isFinal);
					newStateId = b->findStateId(newStateName);

					//copy the old state's outgoing transtions to the new state
					for(std::list<int>::iterator it = b->mTransitions[i][j].begin(); it != b->mTransitions[i][j].end(); it++)
						b->copyOutgoingTransitions(*it, newStateId);

					//removes the transitions going to the old states and adds a transtion going to the new one 
					b->mTransitions[i][j].clear();
					b->mTransitions[i][j].push_back(newStateId);

					removed++;
				}
			}
		}
	} while(removed > 0);
		

	return b;
}
