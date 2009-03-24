#include <gtest/gtest.h>
#include <utils/common/StringTokenizer.cpp>

using namespace std;

/*
Testet die Klasse StringTokenizer
*/

/* Testet die Funktionweise bei einem WHITECHAR als Trenner.*/
TEST(StringTokenizer, test_trenner_whitechar) {
	StringTokenizer *strTok = new StringTokenizer("Hallo  Welt", StringTokenizer.WHITECHARS);
	EXPECT_TRUE(strTok->hasNext()) << "Es sollten noch weitere Tokens vorhanden sein.";
	EXPECT_EQ("Hallo",strTok->next());
	EXPECT_EQ("Welt",strTok->next());
	EXPECT_FALSE(strTok->hasNext()) << "Es sollten keine weitere Tokens vorhanden sein.";
}

/* Testet die Funktionweise bei einem NEWLINE als Trenner.*/
TEST(StringTokenizer, test_trenner_newline) {
	StringTokenizer *strTok = new StringTokenizer("Hallo\nWelt", StringTokenizer.NEWLINE);
	EXPECT_TRUE(strTok->hasNext()) << "Es sollten noch weitere Tokens vorhanden sein.";
	EXPECT_EQ("Hallo",strTok->next());
	EXPECT_EQ("Welt",strTok->next());
	EXPECT_FALSE(strTok->hasNext()) << "Es sollten keine weitere Tokens vorhanden sein.";
}

/* Testet die Funktionweise bei einem beliebigen Trenner.*/
TEST(StringTokenizer, test_trenner_x) {
	StringTokenizer *strTok = new StringTokenizer("HalloxxWelt", "x");
	EXPECT_TRUE(strTok->hasNext()) << "Es sollten noch weitere Tokens vorhanden sein.";
	EXPECT_EQ("Hallo",strTok->next());
	EXPECT_EQ("",strTok->next());
	EXPECT_EQ("Welt",strTok->next());
	EXPECT_FALSE(strTok->hasNext()) << "Es sollten keine weitere Tokens vorhanden sein.";
}

/* Testet die Funktionweise bei einem beliebigen Trenner, mit der Option splitAtAllChars=true*/
TEST(StringTokenizer, test_trenner_xy_immerspliten) {
	StringTokenizer *strTok = new StringTokenizer("HalloxWyelt", "xy",true);
	EXPECT_TRUE(strTok->hasNext()) << "Es sollten noch weitere Tokens vorhanden sein.";
	EXPECT_EQ("Hallo",strTok->next());
	EXPECT_EQ("W",strTok->next());
	EXPECT_EQ("elt",strTok->next());
	EXPECT_FALSE(strTok->hasNext()) << "Es sollten keine weitere Tokens vorhanden sein.";
}

/* Testet die Funktionweise der Funktion reinit*/
TEST(StringTokenizer, test_method_reinit) {
	StringTokenizer *strTok = new StringTokenizer("Hallo");
	strTok->next();
	EXPECT_FALSE(strTok->hasNext()) << "Es sollten keine weitere Tokens vorhanden sein.";
	strTok->reinit();
	EXPECT_TRUE(strTok->hasNext()) << "Es sollten noch weitere Tokens vorhanden sein.";	
}

/* Testet die Funktionweise der Funktion size*/
TEST(StringTokenizer, test_method_size) {
	StringTokenizer *strTok = new StringTokenizer("Hallo kleine Welt");
	EXPECT_EQ(3,strTok->size()) << "Die Anzahl der Token stimmt nicht.";
	StringTokenizer *strTok2 = new StringTokenizer("");
	EXPECT_EQ(0,strTok2->size()) << "Die Anzahl der Token stimmt nicht.";
}

/* Testet die Funktionweise der Funktion front*/
TEST(StringTokenizer, test_method_front) {
	StringTokenizer *strTok = new StringTokenizer("Hallo Welt");
	EXPECT_EQ("Hallo",strTok->front()) << "Das vorderste Token stimmt nicht.";
	strTok->next();
	EXPECT_EQ("Hallo",strTok->front()) << "Das vorderste Token stimmt nicht.";
}

/* Testet die Funktionweise der Funktion get*/
TEST(StringTokenizer, test_method_get) {
	StringTokenizer *strTok = new StringTokenizer("Hallo Welt");
	EXPECT_EQ("Hallo",strTok->get(0)) << "Das erste Token stimmt nicht.";
	EXPECT_EQ("Welt",strTok->get(1)) << "Das zweite Token stimmt nicht.";
}

/* Testet die Funktionweise der Funktion getVector*/
TEST(StringTokenizer, test_method_getVector) {
	StringTokenizer *strTok = new StringTokenizer("Hallo Welt");
	vector<string> strVek = strTok->getVector();
	EXPECT_EQ("Welt",strVek.back());
	EXPECT_EQ("Hallo",strVek.front());
}





