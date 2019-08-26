#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

int main() {
    // locale loc("zh_CN.UTF-8");
    locale loc("");
    locale::global(loc);

    wcout << loc.name().c_str() << endl; // zh_CN.UTF-8

    wcout << endl;

    wstring s = L"中国";
    wcout << s << endl; // 中国
    wcout << s.size() << endl; // 2

    wcout << endl;

    reverse(s.begin(), s.end());
    wcout << s << endl; // 国中

    wcout << endl;

    wstringstream wss(L"中国 你好 汉字 ");
    wcout << wss.str().size() << endl; // 9
    while(wss >> s)
        wcout << s << endl;

    wcout << endl;

    wcout << 1000.01 << endl; // 1000.01
    std::wcout.imbue(loc);
    wcout << 1000.01 << endl; // 1,000.01

    return 0;
}
