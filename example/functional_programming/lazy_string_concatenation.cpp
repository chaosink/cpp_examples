#include <string>
#include <tuple>
#include <cassert>

// Using expression templates to concatenate strings

// 使用const引用可减少拷贝的开销，但是要保证函数的purity。
// 否则在构造表达式模板的实例（用auto构造实例，而不是直接求值）之后，如果修改了字符串的值，则最终的拼接结果也会发生变化。
// #define USE_REF
#ifdef USE_REF
#define STR_REF const &
#else
#define STR_REF
#endif

template<typename... Strings>
class lazy_string_concat_helper;

template<typename LastString, typename... Strings>
class lazy_string_concat_helper<LastString, Strings...> {
private:
    // Stores the copy of the original string
    LastString STR_REF data;
    // Stores the structure that contains other strings
    lazy_string_concat_helper<Strings...> tail;

public:
    lazy_string_concat_helper(LastString STR_REF data, lazy_string_concat_helper<Strings...> tail)
        : data(data), tail(tail) {}

    // Calculates the size of all strings combined
    int size() const {
        return data.size() + tail.size();
    }

    // The structure stores strings in reverse order: the data
    // member variable contains the string that comes last,
    // so it needs to go to the end of the buffer.
    template<typename It>
    void save(It end) const {
        const auto begin = end - data.size();
        std::copy(data.cbegin(), data.cend(), begin);
        tail.save(begin);
    }

    // When you want to convert the expression definition into a real string,
    // allocate enough memory and start copying the strings into it.
    operator std::string() const {
        std::string result(size(), '\0');
        save(result.end());
        return result;
    }

    // Creates a new instance of the structure with one string added to it
    lazy_string_concat_helper<std::string, LastString, Strings...> operator+(
        const std::string &other) const {
        return lazy_string_concat_helper<std::string, LastString, Strings...>(other, *this);
    }
};

template<>
class lazy_string_concat_helper<> {
public:
    lazy_string_concat_helper() {}

    int size() const {
        return 0;
    }

    template<typename It>
    void save(It) const {}

    lazy_string_concat_helper<std::string> operator+(const std::string &other) const {
        return lazy_string_concat_helper<std::string>(other, *this);
    }
};

lazy_string_concat_helper<> lazy_concat;

int main() {
    std::string name = "Jane";
    std::string surname = "Smith";

    // You can’t overload the operator+ on std::string, so use a small
    // trick to force the use of the concatenation structure by appending
    // to an instance of it.
    auto fullname_concat = lazy_concat + surname + ", " + name;

    name = "Gates";
    surname = "Bill";

#ifdef USE_REF
    const std::string fullname = fullname_concat;
    // 如果使用const引用，则在构造表达式模板的实例（用auto构造实例，而不是直接求值）之后，
    // 修改字符串的值会导致最终的拼接结果发生变化。
    assert(fullname == "Bill, Gates");
#else
    const std::string fullname = fullname_concat;
    assert(fullname == "Smith, Jane");
#endif

    return 0;
}
