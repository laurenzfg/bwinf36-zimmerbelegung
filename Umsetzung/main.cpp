#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "girl.hpp" // Girl-Struct

using std::cin;
using std::cout;
using std::cerr;
using std::string;
using std::vector;
using std::map;

// Vorwärtsdeklarationenen der implementierten Funktionen
// Einlesen des Graphens in Adjazenzlisten je Mädchen
map<string, Girl> parseWishes (std::ifstream& inputFile);
// Bestimmung der Zimmer unter Berücksichtigung aller Wünsche
void markRooms (map<string, Girl>& girls);
// Überprüfung der Gültigkeit der Zimmerbelegung
bool checkValidity (map<string, Girl>& girls);

int main(int argc, char* argv[]) {
	// Wurde ein Dateiname übergeben?
	if (argc >= 2) {
		// Schritt 1: Parsen der Zimmerwünsche
		std::ifstream inputFile (argv[1]); // Eingabedatei 1. Parameter
		
		// Map von Mädchennamen auf die jeweiligen Wünsche + Konflikte
		auto girls = parseWishes(inputFile);

		// Schritt 2: Berechnen der Zimmerbelegung
		markRooms(girls);
		
		// Schritt 3: Überprüfen der Validität
		if(checkValidity(girls)) {
			// Schritt 4: Ausgabe der Zimmerbelegung
			// Da am Anfang jedes Mädchen ein Einzelzimmer hatte,
			// gibt es Zimmernummern von 0 .. n-1
			// Im Strukt jedes Mädchens ist das aktuelle Zimmer notiert.
			for (int i = 0; i < girls.size(); ++i) {
				// Wenn das Zimmer i verwaist ist, wird keine
				// Ausgabe getätigt --> kein Zeileneumbruch.
				bool anyOutput = false;
				for (auto &girl : girls) {
					// Gebe alle Mädchen in Zimmer i aus
					if (girl.second.room_ == i) {
						cout << girl.first << " ";
						// Mädchen im Zimmer --> Zeilenumbruch nötig
						anyOutput = true;
					}
				}
				if (anyOutput) // ggfs. Zeilenumbruch
					cout << "\n"; 
			}
		} else {
			// Keine befriediegende Zimmerbelegung möglich
			cout << "Impossible\n";
		}
	
		// Falls ein beliebiges 3. Argument übergeben wurde,
		// Verbose-Output auf cerr
		// #EliteDebugging
		if (argc == 3) {
			for (auto girl : girls) {
				girl.second.debug_out(cerr);
			}
		}
	} else {
		cerr << "Das Programm muss mit dem Dateinamen der Belegungswünsche \
                         als einzigen Parameter aufgerufen werden.\n";
	}
}

// Hilfsunktion zu parseWishes() zur Extraktion der 
// einzelnen Strings in der Like-/Dislikeliste
vector<string> extractNames (string input) {
	vector<string> output; // Vektor über die Namen in der Zeichenkette
	
	input = input.substr(2); // Prefixentfernung (+ oder -)
        // WENN die Zeichenkette nicht leer ist, pushe Namen einzeln
	if (!input.empty()) { 
		int last = 0; // Aktuelle Position im String
                // Für jeden Buchstaben
		for (unsigned i = 0; i < input.size(); ++i) {
			if (input[i] == ' ') { // WENN Anfang eines neuen Wortes
                                // Dann endet hier das vorherige Wort
                                // --> von last bis zum vorherigen Character steht ein Name
				output.push_back(input.substr(last, i-last));
				last = i + 1; // nächstes Zeichen ist dann nächster Wortanfang
			}
		}
		// Letzer Name ist nicht mit ' ' delimited --> Einfach rein
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
			// Einlesen der Wünsche und Konflikte aus der Datei
			getline(inputFile, likes);
			getline(inputFile, dislikes);

			// Anlegen des Girl-Objektes
			Girl p {name, 
				extractNames(likes), extractNames(dislikes)};

			// Speichern des Girl-Objektes gemappt auf den Namen
			girls.insert(std::make_pair(name, p));
			
                        // Überspringen der Leerzeile
			string empty;
			getline(inputFile, empty);
                }
	}
	return girls;
}

// Hilfsfunktion zu markRooms() zum Verschieben aller
// Mädchen aus Zimmer from in Zimmer to
void flipAllGirls(map<string, Girl>& girls, int from, int to) {
	for (auto &girl : girls) {
		// WENN Mädchen in Zimmer from
		if (girl.second.room_ == from) {
			girl.second.room_ = to; // NACH Zimmer to
		}
	}
}

// Berechnet mit "Quick-Find" die Zimmerbelegung unter Berücksichtigung aller Wünsche
void markRooms(map<string, Girl>& girls) {
	// Zunächst wird jedem Mädchen ein Einzelzimmer zugeordnet
	int i = 0;
	for (auto &girl : girls) {
		girl.second.room_ = i++;
	}

	// Erfüllen der Wünsche eines jeden Mädchens
	for (auto &girl : girls) {
		int curRoom = girl.second.room_; // akt. Zimmer von girl
		// Alle Mädchen, die sich das girl wünscht, werden in ihr Zimmer übertragen
		// Alle Mädchen, die mit gewünschten Mädchen gemeinsam wohnen,
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

// Checkt ob zwei Mädchen, die sich nicht mögen (Konflikt), in einem Zimmer gelandet sind
bool checkValidity (map<string, Girl>& girls) {
	// Für JEDES GIRL
	for (auto &girl : girls) {
		int curRoom = girl.second.room_;
		auto dislikes = girl.second.dislikes_;
		// Wenn ein Konflikt in einem Zimmer besteht, ist keine Zimmerbelegung möglich
		for (string dislike : dislikes) {
			int otherRoom = girls[dislike].room_;
			if (curRoom == otherRoom) {
				return false; // Dann fail fast
			}
		}
	}

	// Kein Konflikt, die Zimmerbelegung ist gültig!
	return true;
}
