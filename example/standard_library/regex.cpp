#include <cassert>

#include <iostream>
#include <regex>
using namespace std;

int main() {
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

    return 0;
}
