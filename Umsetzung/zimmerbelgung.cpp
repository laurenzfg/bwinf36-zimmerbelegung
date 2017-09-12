#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "person.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::vector;

vector<Person> parseWishes(std::ifstream& inputFile);

int main(int argc, char* argv[]) {
	if (argc == 2) {
		// Schritt 1: Parsen der Zimmerwünsche
		std::ifstream inputFile (argv[1]); // Eingabedatei 1. Parameter
		auto personen = parseWishes(inputFile);

		// TODO Debugoutput entfernen
		for (auto person : personen)
			person.debug_out(cout);
	} else {
		cerr << "Das Programm muss mit dem Dateinamen der Belegungsüwnsche als einzigen Parameter aufgerufen werden.\n";
	}
}

// Hilfsunktion zur Extraktion der einzelnen Strings in der like/dislikeliste
vector<string> extractNames (string input) {
	vector<string> output;
	
	input = input.substr(2); // Prefixentfernung
	if (!input.empty()) { // WENN Namen hinterlegt sind
		// Pushe sie einzeln mit " " als delimiter
		int last = 0;
		for (unsigned i = 0; i < input.size(); ++i) {
			if (input[i] == ' ') {
				output.push_back(input.substr(last, i-last));
				last = i + 1;
			}
		}
		// Letzer Name ist nicht delimited --> Einfach rein
		output.push_back(input.substr(last));
	}

	return output;
}

// Liest aus dem BwInf-Eingabeformat einen Vektor von Personen
vector<Person> parseWishes(std::ifstream& inputFile) {
	// Anlegen des Vektors für die Personen
	std::vector<Person> personen;
	
	if (inputFile.is_open()) {
		// Parsen der Personen bis zum Dateiende
		string name, likes, dislikes;
		while (getline(inputFile, name)) {
			// Einlesen der Likes / Dislikes
			getline(inputFile, likes);
			getline(inputFile, dislikes);

			// Überspringen der Leerzeile
			string empty;
			getline(inputFile, empty);

			// Anlegen des Person-Objektes
			Person p {name, 
				extractNames(likes), extractNames(dislikes)};

			// Speichern des Person-Objektes
			personen.push_back(p);
		}
	}

	return personen;
}
