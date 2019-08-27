#include <iostream>
#include <type_traits>
#include <typeinfo>
using namespace std;

using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

int main() {
    // clang-format off
    static_assert(sizeof( char) == 1);
    static_assert(sizeof(short) == 2);
    static_assert(sizeof(  int) == 4);
    static_assert(sizeof( long) == 8);



    // 4294967294 == 0xFFFFFFFE, int(0xFFFFFFFE) == -2
    static_assert(is_same_v<decltype(        4294967294), long>);
    static_assert(is_same_v<decltype(                -2), int>);
    static_assert(is_same_v<decltype(         0xFFFFFFE), int>);
    static_assert(is_same_v<decltype(        0xFFFFFFFE), uint>); // 4294967294 == 0xFFFFFFFE
    static_assert(is_same_v<decltype(       0xFFFFFFFFE), long>);
    static_assert(is_same_v<decltype( 0xFFFFFFFFFFFFFFE), long>);
    static_assert(is_same_v<decltype(0xFFFFFFFFFFFFFFFE), ulong>);
    // Error: Integer literal is too large to be represented in any integer type.
    // auto what_is_this =          0xFFFFFFFFFFFFFFFFE;



    static_assert(      0xFFFFFFFE  == 4294967294);
    static_assert(  int(0xFFFFFFFE) != 4294967294);
    static_assert( uint(0xFFFFFFFE) == 4294967294);
    static_assert( long(0xFFFFFFFE) == 4294967294);
    static_assert(ulong(0xFFFFFFFE) == 4294967294);

    static_assert(      0xFFFFFFFE  == -2);
    static_assert(  int(0xFFFFFFFE) == -2);
    static_assert( uint(0xFFFFFFFE) == -2);
    static_assert( long(0xFFFFFFFE) != -2);
    static_assert(ulong(0xFFFFFFFE) != -2);



    { // ulong <- long - ulong
        constexpr  long ll0 = 1;
        constexpr ulong ul0 = 3;

        constexpr  auto ul1 = ll0 - ul0; // ulong <- long - ulong
        static_assert(ul1 == -2);
        // static_assert(ul1 < 0); // error
        static_assert(ul1 != 4294967294);
        static_assert(ul1 != 0xFFFFFFFE);
        static_assert(ul1 == 0xFFFFFFFFFFFFFFFE);
        static_assert(ul1 > 0);
        static_assert(is_same_v<decltype(ul1), const ulong>);
    }

    { // uint <- int - uint
        constexpr  int ii0 = 1;
        constexpr uint ui0 = 3;

        constexpr  int ii1 = ii0 - ui0;
        static_assert(ii1 == -2);

        constexpr auto ui1 = ii0 - ui0; // uint <- int - uint
        static_assert(ui1 == -2);
        // static_assert(ui1 < 0); // error
        static_assert(ui1 == 4294967294); // 4294967294 == 0xFFFFFFFE, int(0xFFFFFFFE) == -2
        static_assert(ui1 == 0xFFFFFFFE);
        static_assert(ui1 != 0xFFFFFFFFFFFFFFFE);
        static_assert(ui1 > 0);
        static_assert(is_same_v<decltype(ui1), const uint>);
    }

    { // int <- short - ushort
        constexpr  short ss0 = 1;
        constexpr ushort us0 = 3;

        constexpr short ss1 = ss0 - us0;
        static_assert(ss1 == -2);

        constexpr  auto ii1 = ss0 - us0; // int <- short - ushort
        static_assert(ii1 == -2);
        static_assert(ii1 < 0);
        static_assert(ii1 != 4294967294);
        static_assert(ii1 == 0xFFFFFFFE);
        static_assert(ii1 == 0xFFFFFFFFFFFFFFFE);
        // static_assert(ii1 > 0); // error
        static_assert(is_same_v<decltype(ii1), const int>);
    }



    { // ulong <- int - ulong
        constexpr   int ii0 = 1;
        constexpr ulong ul0 = 3;

        constexpr   auto ul1 = ii0 - ul0; // ulong <- int - ulong
        static_assert(ul1 == -2);
        // static_assert(ul1 < 0); // error
        static_assert(ul1 != 4294967294);
        static_assert(ul1 != 0xFFFFFFFE);
        static_assert(ul1 == 0xFFFFFFFFFFFFFFFE);
        static_assert(ul1 > 0);
        static_assert(is_same_v<decltype(ul1), const ulong>);
    }

    { // ulong <- short - ulong
        constexpr short ss0 = 1;
        constexpr ulong ul0 = 3;

        constexpr  auto ul1 = ss0 - ul0; // long <- short - ulong
        static_assert(ul1 == -2);
        // static_assert(ul1 < 0); // error
        static_assert(ul1 != 4294967294);
        static_assert(ul1 != 0xFFFFFFFE);
        static_assert(ul1 == 0xFFFFFFFFFFFFFFFE);
        static_assert(ul1 > 0);
        static_assert(is_same_v<decltype(ul1), const ulong>);
    }



    { // long <- long - uint
        constexpr long ll0 = 1;
        constexpr uint ui0 = 3;

        constexpr  auto ll1 = ll0 - ui0; // long <- long - uint
        static_assert(ll1 == -2);
        static_assert(ll1 < 0);
        static_assert(ll1 != 4294967294);
        static_assert(ll1 != 0xFFFFFFFE);
        static_assert(ll1 == 0xFFFFFFFFFFFFFFFE);
        // static_assert(ll1 > 0); // error
        static_assert(is_same_v<decltype(ll1), const long>);
    }

    { // uint <- short - uint
        constexpr short ss0 = 1;
        constexpr  uint ui0 = 3;

        constexpr  auto ui1 = ss0 - ui0; // uint <- short - uint
        static_assert(ui1 == -2);
        // static_assert(ui1 < 0); // error
        static_assert(ui1 == 4294967294);
        static_assert(ui1 == 0xFFFFFFFE);
        static_assert(ui1 != 0xFFFFFFFFFFFFFFFE);
        static_assert(ui1 > 0);
        static_assert(is_same_v<decltype(ui1), const uint>);
    }



    { // long <- long - ushort
        constexpr   long ll0 = 1;
        constexpr ushort us0 = 3;

        constexpr   auto ll1 = ll0 - us0; // long <- long - ushort
        static_assert(ll1 == -2);
        static_assert(ll1 < 0);
        static_assert(ll1 != 4294967294);
        static_assert(ll1 != 0xFFFFFFFE);
        static_assert(ll1 == 0xFFFFFFFFFFFFFFFE);
        // static_assert(ll1 > 0); // error
        static_assert(is_same_v<decltype(ll1), const long>);
    }

    { // int <- int - ushort
        constexpr    int ii0 = 1;
        constexpr ushort us0 = 3;

        constexpr   auto ii1 = ii0 - us0; // int <- int - ushort
        static_assert(ii1 == -2);
        static_assert(ii1 < 0);
        static_assert(ii1 != 4294967294);
        static_assert(ii1 == 0xFFFFFFFE);
        static_assert(ii1 == 0xFFFFFFFFFFFFFFFE);
        // static_assert(ii1 > 0); // error
        static_assert(is_same_v<decltype(ii1), const int>);
    }
    // clang-format on
}
