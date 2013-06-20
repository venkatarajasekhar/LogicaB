all:
	g++ -c main.cpp
	g++ -c automaton.cpp
	g++ main.o automaton.o -o logica.exe
	