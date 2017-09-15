#include "person.hpp"

using std::ostream;
using std::string;

// Ausgabe der Personendaten zu Debugzwecken
ostream& Person::debug_out(ostream& os) {
	os << name_ << " --> " << room_ << "\n"; // Name

	// Likes
	os << "Likes: ";
	if (likes_.size() == 0) {
		os << "-";
	} else {
		for (string s : likes_)
			os << s << " ";
	}
	os << "\n";

	// Dislikes
	os << "Dislikes: ";
	if (dislikes_.size() == 0) {
		os << "-";
	} else {
		for (string s : dislikes_)
			os << s << " ";
	}
	os << "\n \n";

	return os;
}
