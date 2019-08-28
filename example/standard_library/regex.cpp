#include <cassert>

#include <iostream>
#include <regex>
using namespace std;

int main() {
    // regex_match()
    {
        // https://www.regular-expressions.info/floatingpoint.html
        regex re("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"); // floating-point number
        // Modified: "^[-+]?[0-9]*\\.?([0-9]+([eE][-+]?[0-9]+)?)?$"
        smatch match;

        {
            string s = "12";
            assert(regex_match(s, match, re) == true);
            assert(match.size() == 2);
            assert(match[0] == "12"); // The whole string.
            assert(match[1] == ""); // The first parenthesized expression.
        }
        {
            string s = " 12";
            assert(regex_match(s, match, re) == false);
            assert(match.size() == 0);
        }
        {
            string s = "12 ";
            assert(regex_match(s, match, re) == false);
            assert(match.size() == 0);
        }

        {
            string s = "1.2E34";
            assert(regex_match(s, match, re) == true);
            assert(match.size() == 2);
            assert(match[0] == "1.2E34"); // The whole string.
            assert(match[1] == "E34"); // The first parenthesized expression.
        }
        {
            string s = "12E3.4";
            assert(regex_match(s, match, re) == false);
            assert(match.size() == 0);
        }
        {
            string s = "12.";
            assert(regex_match(s, match, re) == false); // True for the modified.
            assert(match.size() == 0);
        }
    }

    {
        wregex re(L"C\\+\\+是(世界|地球|宇宙)上最[好棒帅酷美]的语言！");
        // No escape for back slashes in raw string literal.
        wregex re_raw(LR"(C\+\+是(世界|地球|宇宙)上最[好棒帅酷美]的语言！)");
        wsmatch match;

        {
            wstring s = L"C++是世界上最好的语言！";
            assert(regex_match(s, match, re) == true);
        }

        {
            wstring s = L"C++是地球上最棒的语言！";
            assert(regex_match(s, match, re) == true);
        }

        {
            wstring s = L"PHP是世界上最好的语言！";
            assert(regex_match(s, match, re) == false);
        }
    }



    // regex_search()
    {
        string s = "Roses are #ff3300.";
        regex color_re(
            "#([a-f0-9]{2})"
            "([a-f0-9]{2})"
            "([a-f0-9]{2})");
        assert(regex_search(s, color_re));

        smatch color_match;
        assert(regex_search(s, color_match, color_re));
        assert(color_match.prefix() == "Roses are ");
        assert(color_match.suffix() == ".");
        assert(color_match.size() == 4);
        assert(color_match[0] == "#ff3300");
        assert(color_match[1] == "ff");
        assert(color_match[2] == "33");
        assert(color_match[3] == "00");
    }



    // regex_replace()
    {
        string s = "Quick brown fox.";
        regex vowel_re("a|e|i|o|u");

        assert(regex_replace(s, vowel_re, "*") == "Q**ck br*wn f*x.");
        assert(regex_replace(s, vowel_re, "[$&]") == "Q[u][i]ck br[o]wn f[o]x.");
    }

    return 0;
}
