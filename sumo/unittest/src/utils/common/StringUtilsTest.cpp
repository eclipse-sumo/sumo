#include <gtest/gtest.h>
#include <utils/common/StringUtils.h>

using namespace std;

/*
Testet die Klasse StringUtils
*/

/* Testet die Methode prune. Schneidet Leerzeichen am Anfang und am Ende eines Strings ab.*/
TEST(StringUtils, test_method_prune) {
	EXPECT_EQ("Ergebniss", StringUtils::prune("  Ergebniss "))<< "Leerzeichen am Anfang und am Ende des Strings sollten entfernt werden.";
}

/* Testet die Methode to_lower_case. Wandelt den String in kleine Buchstaben um.*/
TEST(StringUtils, test_method_to_lower_case) {
	EXPECT_EQ("hallo", StringUtils::to_lower_case("HALLO"))<< "String sollte in Kleinbuchstaben umgewandelt sein.";
}

/* Testet die Methode version1. Erweitert den übergebenen String um '_x', wobei x die naechste Versionsnummer ist.*/
TEST(StringUtils, test_method_version1) {
	EXPECT_EQ("hallo_0", StringUtils::version1("hallo"))<< "Anhaengen von _0.";
	EXPECT_EQ("hallo_4", StringUtils::version1("hallo_3"))<< "_3 auf _4 ändern.";
	EXPECT_EQ("hal_lo_0", StringUtils::version1("hal_lo"))<< "Anhaengen von _0.";
}

/* Testet die Methode convertUmlaute.*/
TEST(StringUtils, test_method_convertUmlaute) {
	EXPECT_EQ("ae", StringUtils::convertUmlaute("ä"));
	EXPECT_EQ("Ae", StringUtils::convertUmlaute("Ä"));
	EXPECT_EQ("oe", StringUtils::convertUmlaute("ö"));
	EXPECT_EQ("Oe", StringUtils::convertUmlaute("Ö"));
	EXPECT_EQ("ue", StringUtils::convertUmlaute("ü"));
	EXPECT_EQ("Ue", StringUtils::convertUmlaute("Ü"));
	EXPECT_EQ("ss", StringUtils::convertUmlaute("ß"));
	EXPECT_EQ("E", StringUtils::convertUmlaute("É"));
	EXPECT_EQ("e", StringUtils::convertUmlaute("é"));
	EXPECT_EQ("E", StringUtils::convertUmlaute("È"));
	EXPECT_EQ("e", StringUtils::convertUmlaute("è"));
}

/* Testet die Methode replace. Ersetzt alle vorkommen des 2. Strings durch den 3. String im 1. String*/
TEST(StringUtils, test_method_replace) {
	EXPECT_EQ("halt", StringUtils::replace("hallo","lo","t"))<< "Einmalige Ersetzung.";
	EXPECT_EQ("haststo", StringUtils::replace("hallo","l","st"))<< "Mehrere Ersetzungen";
}

/* Testet die Methode upper. Der übergebene String von in Großbuchstaben umgewandelt.*/
TEST(StringUtils, test_method_upper) {
	string toUpper = "halloWelt123-?";
	StringUtils::upper(toUpper);
	EXPECT_EQ("HALLOWELT123-?", toUpper)<< "Es sollten nur Großbuchstaben vorkommen.";
}

/* Testet die Methode toTimeString. Wandelt ein int als Sekunden in (hh:mm:ss) um.*/
TEST(StringUtils, test_method_toTimeString) {	
	EXPECT_EQ("00:00:00", StringUtils::toTimeString(0))<< "Es sollte 0 Sek angezeigt werden.";
	EXPECT_EQ("01:00:00", StringUtils::toTimeString(3600))<< "Es sollte 1 h angezeigt werden.";
	EXPECT_EQ("00:00:01", StringUtils::toTimeString(1))<< "Es sollte 1 Sek angezeigt werden.";
	EXPECT_EQ("49:40:00", StringUtils::toTimeString(178800))<< "Es sollte 49 h und 40 min angezeigt werden.";
}