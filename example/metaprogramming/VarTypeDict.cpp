#include <cassert>
#include <ctime>

#include <memory>
using namespace std;

struct NullParameter {}; // 占位类型，之后再替换为`Set()`的参数类型

template <typename T>
struct Identity_ {
    using type = T;
};

namespace NSMultiTypeDict {
/// ====================== Create ===================================
template <size_t N, template<typename...> class TCont, typename...T> // 第二个参数是一个模板类，实参为模板类Values，而不是一个类型Values<...>
struct Create_ {
    using type = typename Create_<N-1, TCont, NullParameter, T...>::type;
};

template <template<typename...> class TCont, typename...T>
struct Create_<0, TCont, T...> {
    using type = TCont<T...>; // TCont一直没动，是一个模板类，最后用来构造一个具体的返回类
};

/// ====================== FindTagPos ===================================
template <typename TFindTag, size_t N, typename TCurTag, typename...TTags>
struct Tag2ID_ {
    constexpr static size_t value = Tag2ID_<TFindTag, N + 1, TTags...>::value;
};

template <typename TFindTag, size_t N, typename...TTags>
struct Tag2ID_<TFindTag, N, TFindTag, TTags...> {
    constexpr static size_t value = N;
};

template <typename TFindTag, typename...TTags>
constexpr size_t Tag2ID = Tag2ID_<TFindTag, 0, TTags...>::value;

/// ====================== NewTupleType ===================================
template <typename TVal, size_t N, size_t M, typename TProcessedTypes, typename... TRemainTypes>
struct NewTupleType_;

template <typename TVal, size_t N, size_t M, template <typename...> class TCont, // TCont是一个模板类，将实参Values<>，一个具体的类，解析为模板类Values。
          typename...TModifiedTypes, typename TCurType, typename... TRemainTypes>
struct NewTupleType_<TVal, N, M, TCont<TModifiedTypes...>,
                     TCurType, TRemainTypes...> {
    using type = typename NewTupleType_<TVal, N, M + 1,
                                        TCont<TModifiedTypes..., TCurType>,
                                        TRemainTypes...>::type;
};

template <typename TVal, size_t N, template <typename...> class TCont,
          typename...TModifiedTypes, typename TCurType, typename... TRemainTypes>
struct NewTupleType_<TVal, N, N, TCont<TModifiedTypes...>, TCurType, TRemainTypes...> {
    using type = TCont<TModifiedTypes..., TVal, TRemainTypes...>;
};

template <typename TVal, size_t TagPos, typename TCont, typename... TRemainTypes>
using NewTupleType = typename NewTupleType_<TVal, TagPos, 0, TCont, TRemainTypes...>::type;

/// ====================== Pos2Type ===================================
template <size_t Pos, typename...TTags>
struct Pos2Type_ {
    static_assert((Pos != 0), "Invalid position.");
};

template <size_t Pos, typename TCur, typename...TTags>
struct Pos2Type_<Pos, TCur, TTags...> {
    using type = typename std::conditional_t<(Pos == 0),
                                             Identity_<TCur>,
                                             Pos2Type_<Pos-1, TTags...>>::type;
};

template <size_t Pos, typename...TTags>
using Pos2Type = typename Pos2Type_<Pos, TTags...>::type;

} //end namespace NSMultiTypeDict

template <typename...TParameters> // TParameters保存键
struct VarTypeDict {
    template <typename...TTypes> // TTypes保存数值类型
    struct Values {
        Values() = default;

        Values(std::shared_ptr<void> (&&input)[sizeof...(TTypes)]) {
            for (size_t i = 0; i < sizeof...(TTypes); ++i)
                m_tuple[i] = std::move(input[i]);
        }

    public:
        template <typename TTag, typename TVal>
        auto Set(TVal&& val) && { // Set()最后构造new_type时会用std::move移动m_tuple的内容从而使其未定义，所以当前的Values也变为未定义，不能再用，所以Set()只能被右值Values调用（即括号后的&&）。
            using namespace NSMultiTypeDict;
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;

            using rawVal = std::decay_t<TVal>;
            rawVal* tmp = new rawVal(std::forward<TVal>(val)); // perfect forwarding
            m_tuple[TagPos] = std::shared_ptr<void>(tmp,
                                    [](void* ptr) {
                                        rawVal* nptr = static_cast<rawVal*>(ptr);
                                        delete nptr;
                                    });

            using new_type = NewTupleType<rawVal, TagPos, Values<>, TTypes...>; // 这里实参为Values<>，一个具体的类，而不是模板类Values；之后NewTupleType会从Values<>解析出模板类Values。
            return new_type(std::move(m_tuple));
        }

        template <typename TTag>
        auto& Get() const {
            using namespace NSMultiTypeDict;
            constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
            using AimType = Pos2Type<TagPos, TTypes...>;

            void* tmp = m_tuple[TagPos].get();
            AimType* res = static_cast<AimType*>(tmp);
            return *res;
        }

        template <typename TTag>
        using ValueType = NSMultiTypeDict::Pos2Type<NSMultiTypeDict::Tag2ID<TTag, TParameters...>, TTypes...>;

    private:
        std::shared_ptr<void> m_tuple[sizeof...(TTypes)];
    };

public:
    static auto Create() {
        using type = typename NSMultiTypeDict::Create_<sizeof...(TParameters), Values>::type; // sizeof...是C++11的关键字，可得到变长参数的个数
        return type{}; // 现定义类型，并构造其对象
    }
};

/*--------------------------------------------------*/

struct A;
struct B;

template<typename TIn>
auto Fun(const TIn& in) {
    return in.template Get<B>() + in.template Get<A>();
}

int main() {
    // Set paramters **dynamically** using std::rand().
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int i = std::rand();
    float f = static_cast<float>(std::rand());



    auto p_Ai_Bf_0 = VarTypeDict<A, B>::Create() // p_Ai_Bf_0 类型为 VarTypeDict<A, B>::Values<int, float>
        .Set<A>(i)
        .Set<B>(f);
    assert(Fun(p_Ai_Bf_0) == i + f);
    static_assert(is_same<
            decltype(p_Ai_Bf_0),
            VarTypeDict<A, B>::Values<int, float>
        >::value == true);

    auto p_Ai_Bf_1 = VarTypeDict<A, B>::Create() // p_Ai_Bf_1 与 p_Ai_Bf_0 类型相同
        .Set<B>(f)
        .Set<A>(i); // 可以任意顺序设定参数
    assert(Fun(p_Ai_Bf_1) == i + f);
    static_assert(is_same<
            decltype(p_Ai_Bf_1),
            VarTypeDict<A, B>::Values<int, float>
        >::value == true);



    auto p_Af_Bi = VarTypeDict<A, B>::Create() // p_Af_Bi 类型为 VarTypeDict<A, B>::Values<float, int>
        .Set<A>(f)
        .Set<B>(i);
    assert(Fun(p_Af_Bi) == i + f);
    static_assert(is_same<
            decltype(p_Af_Bi),
            VarTypeDict<A, B>::Values<float, int>
        >::value == true);

    auto p_Bi_Af = VarTypeDict<B, A>::Create() // p_Bi_Af 类型为 VarTypeDict<B, A>::Values<int, float>
        .Set<B>(i)
        .Set<A>(f);
    assert(Fun(p_Bi_Af) == i + f);
    static_assert(is_same<
            decltype(p_Bi_Af),
            VarTypeDict<B, A>::Values<int, float>
        >::value == true);

    auto p_Bf_Ai = VarTypeDict<B, A>::Create() // p_Bf_Ai 类型为 VarTypeDict<B, A>::Values<float, int>
        .Set<B>(f)
        .Set<A>(i);
    assert(Fun(p_Bf_Ai) == i + f);
    static_assert(is_same<
            decltype(p_Bf_Ai),
            VarTypeDict<B, A>::Values<float, int>
        >::value == true);
}
