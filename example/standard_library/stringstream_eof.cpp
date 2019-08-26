#include <cassert>

#include <sstream>
using namespace std;

int main() {
    {
        stringstream ss("123");
        int n = ss.str().size();
        assert(n == 3);

        int i;
        ss >> i;
        assert(ss.eof() == true);
    }

    {
        stringstream ss("123");
        int n = ss.str().size();
        assert(n == 3);

        for(int i = 0; i < n; i++)
            ss.get();
        assert(ss.eof() == false);
        assert(ss.peek() == char_traits<char>::eof());

        char c = ss.get();
        assert(c == char_traits<char>::eof());
        assert(ss.eof() == true);
    }

    return 0;
}
