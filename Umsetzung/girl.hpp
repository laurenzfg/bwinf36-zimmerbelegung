#ifndef GIRL_HPP
#define GIRL_HPP

#include <string>
#include <vector>
#include <iostream>

struct Girl {
	std::string name_;
	std::vector<std::string> likes_; // gewünschte Mädchen
	std::vector<std::string> dislikes_; // mögliche Konflikte
	int room_ = -1; // Zimmer des Mädchens

	// Ausgabe der Personendaten zu Debugzwecken
	std::ostream& debug_out(std::ostream& os);
};
#endif //GIRL_HPP
