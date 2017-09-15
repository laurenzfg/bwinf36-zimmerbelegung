#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "person.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::map;

map<string, Person> parseWishes(std::ifstream& inputFile);
void markRooms(map<string, Person>& personen);
bool checkValidity (map<string, Person>& personen);

int main(int argc, char* argv[]) {
	if (argc >= 2) {
		// Schritt 1: Parsen der Zimmerwünsche
		std::ifstream inputFile (argv[1]); // Eingabedatei 1. Parameter
		// Der zurückgegebene Vektor asoziiert 
		// jedem Namen auch eine id von 0 bis n-1
		auto personen = parseWishes(inputFile);

		// Schritt 2: Markieren der Zimmer
		markRooms(personen);
		
		// Schritt 3: Überprüfen der Validität
		if(checkValidity(personen)) {
			// Schritt 4: Ausgabe der Zimmerbelegung
			for (int i = 0; i < personen.size(); ++i) {
				bool anyOutput = false;
				for (auto &person : personen) {
					if (person.second.room_ == i) {
						cout << person.first << " ";
						anyOutput = true;
					}
				}
				if (anyOutput)
					cout << "\n";
			}
		} else {
			cout << "IMPOSSIBLE\n";
		}
		
		if (argc == 3) {
			for (auto person : personen) {
				person.second.debug_out(cerr);
			}
		}
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
map<string, Person> parseWishes(std::ifstream& inputFile) {
	// Anlegen der Map für die Personen
	map<string, Person> personen;
	
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
			personen.insert(std::make_pair(name, p));
		}
	}

	return personen;
}

void flipAllGirls(map<string, Person>& personen, int from, int to) {
	for (auto &girl : personen) {
		if (girl.second.room_ == from) {
			girl.second.room_ = to;
		}
	}
}

// Berechnet mit Quick-Find alle Zimmer
void markRooms(map<string, Person>& personen) {
	// Legt jedes Mädchen in ein 1-Personen-Zimmer
	int i = 0;
	for (auto &person : personen) {
		person.second.room_ = i++;
	}

	// Erfüllen der Like-Wünsche jedes Mädchens
	for (auto &person : personen) {
		int curRoom = person.second.room_;
		auto likes = person.second.likes_;
		for (string like : likes) {
			int otherRoom = personen[like].room_;
			if (curRoom != otherRoom) {
				// --> Alle Mädchen im anderen Raum müssen wechseln
				flipAllGirls(personen, otherRoom, curRoom);
			}
		}
	}
}

bool checkValidity (map<string, Person>& personen) {
	for (auto &person : personen) {
		int curRoom = person.second.room_;
		auto dislikes = person.second.dislikes_;
		for (string dislike : dislikes) {
			int otherRoom = personen[dislike].room_;
			if (curRoom == otherRoom) {
				return false;
			}
		}
	}
	return true;
}
