#include <iostream>
#include <fstream>
#include <string>

#include "automaton.h"


int main() {
	std::string automatonFilename;
	std::ifstream automatonFile;

	std::cout << "Digite o nome do arquivo que contem o automato." << std::endl << ">> ";
	std::cin >> automatonFilename;
	std::cout << std::endl;
	std::cout << std::endl;

	automatonFile.open(automatonFilename.c_str(), std::ifstream::in);

	Automaton * automaton = new Automaton(automatonFile);
	Automaton * automaton2 = automaton->getDeterministic();

	std::cout << "====== Problema Inicial ======" << std::endl;
	automaton->printTransitionsTable(std::cout);
	std::cout << std::endl;
	std::cout << "A cadeia foi aceita de " << automaton->testString("acd") << " formas diferentes" << std::endl;
	std::cout << "==============================" << std::endl << std::endl << std::endl;

	std::cout << "====== Problema Alterado ======" << std::endl;
	automaton2->printTransitionsTable(std::cout);
	std::cout << std::endl;
	std::cout << "A cadeia foi aceita de " << automaton2->testString("acd") << " formas diferentes" << std::endl;
	std::cout << "===============================" << std::endl << std::endl;

	return 0;
}
