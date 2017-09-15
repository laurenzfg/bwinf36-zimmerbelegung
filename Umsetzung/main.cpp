#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "girl.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::map;

map<string, Girl> parseWishes (std::ifstream& inputFile);
void markRooms (map<string, Girl>& girls);
bool checkValidity (map<string, Girl>& girls);

int main(int argc, char* argv[]) {
	// Wurde ein Dateiname übergeben
	if (argc >= 2) {
		// Schritt 1: Parsen der Zimmerwünsche
		std::ifstream inputFile (argv[1]); // Eingabedatei 1. Parameter
		
		// Map von Name auf Eigenschaften eines Mädchens
		auto girls = parseWishes(inputFile);

		// Schritt 2: Berechnen der Zimmer
		markRooms(girls);
		
		// Schritt 3: Überprüfen der Validität
		if(checkValidity(girls)) {
			// Schritt 4: Ausgabe der Zimmerbelegung
			for (int i = 0; i < girls.size(); ++i) {
				// Wenn das Zimmer verwaist ist, muss kein
				// Zeilenumbruch gemacht werden
				bool anyOutput = false;
				for (auto &girl : girls) {
					// Gebe alle Mädchen in Zimmer i aus
					if (girl.second.room_ == i) {
						cout << girl.first << " ";
						// Mädchen im Zimmer --> Zeilenumbruch
						anyOutput = true;
					}
				}
				if (anyOutput)
					cout << "\n";
			}
		} else {
			// Keine befriedriegende Zimmerbelegung möglich
			cout << "Impossible\n";
		}
	
		// Falls ein beliebiges 3. Argument übergeben wurde,
		// Verbose-Modus auf cerr	
		if (argc == 3) {
			for (auto girl : girls) {
				girl.second.debug_out(cerr);
			}
		}
	} else {
		cerr << "Das Programm muss mit dem Dateinamen der Belegungsüwnsche als einzigen Parameter aufgerufen werden.\n";
	}
}

// Hilfsunktion zur Extraktion der einzelnen Strings in der Like-/Dislikeliste
vector<string> extractNames (string input) {
	vector<string> output;
	
	input = input.substr(2); // Prefixentfernung (+ oder -)
	if (!input.empty()) { // WENN Namen in der Liste sind
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

// Liest aus dem BwInf-Eingabeformat eine Map von Namen auf Like/Dislikeliste
map<string, Girl> parseWishes(std::ifstream& inputFile) {
	// Anlegen der Map für die Mädchen
	map<string, Girl> girls;
	
	if (inputFile.is_open()) {
		// Parsen der Entrys bis zum Dateiende
		string name, likes, dislikes;
		while (getline(inputFile, name)) {
			// Einlesen der Likes / Dislikes als Zeile
			getline(inputFile, likes);
			getline(inputFile, dislikes);

			// Überspringen der Leerzeile
			string empty;
			getline(inputFile, empty);

			// Anlegen des Girl-Objektes
			Girl p {name, 
				extractNames(likes), extractNames(dislikes)};

			// Speichern des Girl-Objektes gemappt auf den Namen
			girls.insert(std::make_pair(name, p));
		}
	}

	return girls;
}

// Sortiert alle Mädchen aus Zimmer from in Zimmer to ein
void flipAllGirls(map<string, Girl>& girls, int from, int to) {
	for (auto &girl : girls) {
		// WENN Mädchen in Zimmer from
		if (girl.second.room_ == from) {
			girl.second.room_ = to; // NACH Zimmer to
		}
	}
}

// Berechnet mit "Quick-Find" die Zimmerbelegung unter Berücksichtigung aller Likes
void markRooms(map<string, Girl>& girls) {
	// Zunächst wird jedem Mädchen ein Einzelzimmer zugeordnet
	int i = 0;
	for (auto &girl : girls) {
		girl.second.room_ = i++;
	}

	// Erfüllen der Like-Wünsche jedes Mädchens
	for (auto &girl : girls) {
		int curRoom = girl.second.room_;
		// Alle Mädchen, die das girl mag, werden in ihr Zimmer übertragen
		// Alle Mädchen, die mit geliketen Mädchen gemeinsam wohnen,
		// müssen mit übertragen werden (siehe Doku)
		auto likes = girl.second.likes_;
		for (string like : likes) {
			int otherRoom = girls[like].room_;
			if (curRoom != otherRoom) {
				// --> Alle Mädchen im anderen Raum müssen wechseln
				flipAllGirls(girls, otherRoom, curRoom);
			}
		}
	}
}

// Checkt ob zwei Mädchen, die sich nicht mögen, in einem Zimmer gelandet sind
bool checkValidity (map<string, Girl>& girls) {
	// Für JEDES GIRL
	for (auto &girl : girls) {
		int curRoom = girl.second.room_;
		auto dislikes = girl.second.dislikes_;
		// Wenn ein gedisliktet girl im gleichen Zimmer liegt, ist keine
		// Zimmerbelegung möglich
		for (string dislike : dislikes) {
			int otherRoom = girls[dislike].room_;
			if (curRoom == otherRoom) {
				return false;
			}
		}
	}

	// Kein Konflikt, die Zimmerbelegung ist gültig!
	return true;
}
