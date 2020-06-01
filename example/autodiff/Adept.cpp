// An implementation of Adept.
// 2014 - TOMS - Fast Reverse-Mode Automatic Differentiation using Expression Templates in C++

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <cmath>
#include <valarray>

namespace adept {

/*------------------------------------------------------------------------------------------------*/
// Adept
// Record intermediate results.
// Calculate gradients in forward or reverse mode.

template<typename T>
class Variable;

template<typename T>
class Adept {
    size_t n_gradients_ = 0;
    std::vector<T> gradients_;

    struct Operand {
        T multiplier;
        size_t i_gradient;
    };
    std::vector<Operand> operands_;

    struct Statement {
        size_t i_gradient;
        size_t i_first_operand;
    };
    std::vector<Statement> statements_;

    Adept() {}

    static void PushStatement(size_t i_gradient) {
        Adept<T> &adept = Get();
        adept.statements_.push_back({i_gradient, adept.operands_.size()});
    }

    static void PushOperand(const T &multiplier, size_t i_gradient) {
        Adept<T> &adept = Get();
        adept.operands_.push_back({multiplier, i_gradient});
    }

    friend class Variable<T>;

#ifndef NDEBUG
public:
#endif
    static Adept<T> &Get() {
        static Adept<T> adept;
        return adept;
    }

public:
    // Clear all statements and operands, while preserve all gradients, to reuse variables and
    // run new statements.
    static void ClearStatementsAndOperands() {
        Adept<T> &adept = Get();
        adept.operands_.clear();
        adept.statements_.clear();
    }

    // Clear all statements, operands and gradients.
    static void Clear() {
        ClearStatementsAndOperands();

        Adept<T> &adept = Get();
        adept.n_gradients_ = 0;
        adept.statements_.clear();
    }

    static size_t RegisterGradient() {
        Adept<T> &adept = Get();
        return adept.n_gradients_++;
    }

    // Reset all gradients to zero, while keep all statements and operands, to recalculate gradients.
    static void ResetGradients() {
        Adept<T> &adept = Get();
        adept.gradients_.resize(adept.n_gradients_);
        std::fill(adept.gradients_.begin(), adept.gradients_.end(), 0.f);
    }

    static void SetGradient(const T &gradient, size_t i_gradient) {
        Adept<T> &adept = Get();
        adept.gradients_[i_gradient] = gradient;
    }

    static const T &GetGradient(size_t i_gradient) {
        Adept<T> &adept = Get();
        return adept.gradients_[i_gradient];
    }

    static void Forward() {
        Adept<T> &adept = Get();
        auto &statements = adept.statements_;
        auto &operands = adept.operands_;
        auto &gradients = adept.gradients_;

        size_t begin = statements.front().i_first_operand;
        for(auto it = statements.cbegin(); it != statements.cend(); ++it) {
            auto it_next = it + 1;
            size_t end = it_next == statements.cend() ? operands.size() : it_next->i_first_operand;
            T g = 0.f;
            for(size_t i = begin; i < end; ++i)
                g += operands[i].multiplier * gradients[operands[i].i_gradient];
            gradients[it->i_gradient] = g;
            begin = end;
        }
    }

    static void Reverse() {
        Adept<T> &adept = Get();
        auto &statements = adept.statements_;
        auto &operands = adept.operands_;
        auto &gradients = adept.gradients_;

        size_t end = operands.size();
        for(auto it = statements.crbegin(); it != statements.crend(); ++it) {
            size_t begin = it->i_first_operand;
            if(gradients[it->i_gradient]) {
                T g = gradients[it->i_gradient];
                gradients[it->i_gradient] = 0.f;
                for(size_t i = begin; i < end; ++i)
                    gradients[operands[i].i_gradient] += operands[i].multiplier * g;
            }
            end = begin;
        }
    }
};

/*------------------------------------------------------------------------------------------------*/
// Expression, Variable, Constant

template<typename T, typename A>
class Expression {
    const A &Cast() const {
        return static_cast<const A &>(*this);
    }

protected:
    T value_ = 0.f;

public:
    const T &GetValue() const {
        return value_;
    }

    void CalcGradient(const T &multiplier) const {
        Cast().CalcGradient(multiplier);
    }

    operator const A &() const {
        return Cast();
    }
};

template<typename T>
class Variable: public Expression<T, Variable<T>> {
    const size_t i_gradient_;

public:
    Variable(const T &value = 0.f): i_gradient_(Adept<T>::RegisterGradient()) {
        this->value_ = value;
    }

    Variable(const Variable &rhs): i_gradient_(Adept<T>::RegisterGradient()) {
        *this = rhs;
    }

    template<typename A>
    Variable(const Expression<T, A> &rhs): i_gradient_(Adept<T>::RegisterGradient()) {
        *this = rhs;
    }

    Variable &operator=(const T &value) {
        this->value_ = value;
        return *this;
    }

    Variable &operator=(const Variable &rhs) {
        Adept<T>::PushStatement(i_gradient_);
        rhs.CalcGradient(1.f);
        this->value_ = rhs.GetValue();
        return *this;
    }

    template<typename A>
    Variable &operator=(const Expression<T, A> &rhs) {
        Adept<T>::PushStatement(i_gradient_);
        rhs.CalcGradient(1.f);
        this->value_ = rhs.GetValue();
        return *this;
    }

    void CalcGradient(const T &multiplier) const {
        Adept<T>::PushOperand(multiplier, i_gradient_);
    }

    void SetGradient(const T &gradient) {
        Adept<T>::SetGradient(gradient, i_gradient_);
    }

    const T &GetGradient() {
        return Adept<T>::GetGradient(i_gradient_);
    }
};

template<typename T>
class Constant: public Expression<T, Constant<T>> {
public:
    Constant(const T &value) {
        this->value_ = value;
    }

    Constant &operator=(const T &value) {
        this->value_ = value;
        return *this;
    }

    void CalcGradient(const T & /*multiplier*/) const {}
};

/*------------------------------------------------------------------------------------------------*/
// Various operators

template<typename T, typename A, typename B>
bool operator<(const Expression<T, A> &a, const Expression<T, B> &b) {
    return a.GetValue() < b.GetValue();
}

template<typename T, typename A, typename B>
bool operator>(const Expression<T, A> &a, const Expression<T, B> &b) {
    return b < a;
}

template<typename T, typename A>
const Expression<T, A> &operator+(const Expression<T, A> &a) {
    return a;
}

#define Operator1(Op, op)                    \
    template<typename T, typename A>         \
    Op<T, A> op(const Expression<T, A> &a) { \
        return Op<T, A>(a);                  \
    }

#define Operator2(Op, op)                                                  \
    template<typename T, typename A, typename B>                           \
    Op<T, A, B> op(const Expression<T, A> &a, const Expression<T, B> &b) { \
        return Op<T, A, B>(a, b);                                          \
    }

template<typename T, typename A>
class Negative: public Expression<T, Negative<T, A>> {
    const A &a_;

public:
    Negative(const Expression<T, A> &a): a_(a) {
        this->value_ = -a_.GetValue();
    }

    void CalcGradient(const T &multiplier) const {
        a_.CalcGradient(-multiplier);
    }
};
Operator1(Negative, operator-);

template<typename T, typename A, typename B>
class Add: public Expression<T, Add<T, A, B>> {
    const A &a_;
    const B &b_;

public:
    Add(const Expression<T, A> &a, const Expression<T, B> &b): a_(a), b_(b) {
        this->value_ = a_.GetValue() + b_.GetValue();
    }

    void CalcGradient(const T &multiplier) const {
        a_.CalcGradient(multiplier);
        b_.CalcGradient(multiplier);
    }
};
Operator2(Add, operator+);

template<typename T, typename A, typename B>
class Subtract: public Expression<T, Subtract<T, A, B>> {
    const A &a_;
    const B &b_;

public:
    Subtract(const Expression<T, A> &a, const Expression<T, B> &b): a_(a), b_(b) {
        this->value_ = a_.GetValue() - b_.GetValue();
    }

    void CalcGradient(const T &multiplier) const {
        a_.CalcGradient(multiplier);
        b_.CalcGradient(-multiplier);
    }
};
Operator2(Subtract, operator-);

template<typename T, typename A, typename B>
class Multiply: public Expression<T, Multiply<T, A, B>> {
    const A &a_;
    const B &b_;

public:
    Multiply(const Expression<T, A> &a, const Expression<T, B> &b): a_(a), b_(b) {
        this->value_ = a_.GetValue() * b_.GetValue();
    }

    void CalcGradient(const T &multiplier) const {
        a_.CalcGradient(b_.GetValue() * multiplier);
        b_.CalcGradient(a_.GetValue() * multiplier);
    }
};
Operator2(Multiply, operator*);

template<typename T, typename A, typename B>
class Divide: public Expression<T, Divide<T, A, B>> {
    const A &a_;
    const B &b_;

public:
    Divide(const Expression<T, A> &a, const Expression<T, B> &b): a_(a), b_(b) {
        this->value_ = a_.GetValue() / b_.GetValue();
    }

    void CalcGradient(const T &multiplier) const {
        T rcp_b_mul = 1.f / b_.GetValue() * multiplier;
        a_.CalcGradient(rcp_b_mul);
        b_.CalcGradient(-rcp_b_mul * this->value_);
    }
};
Operator2(Divide, operator/);

#define OperatorClassBegin1(Op, op)                \
    template<typename T, typename A>               \
    class Op: public Expression<T, Op<T, A>> {     \
        const A &a_;                               \
                                                   \
    public:                                        \
        Op(const Expression<T, A> &a): a_(a) {     \
            this->value_ = std::op(a_.GetValue()); \
        }                                          \
                                                   \
        void CalcGradient(const T &multiplier) const {
#define OperatorClassBegin2(Op, op)                                              \
    template<typename T, typename A, typename B>                                 \
    class Op: public Expression<T, Op<T, A, B>> {                                \
        const A &a_;                                                             \
        const B &b_;                                                             \
                                                                                 \
    public:                                                                      \
        Op(const Expression<T, A> &a, const Expression<T, B> &b): a_(a), b_(b) { \
            this->value_ = std::op(a_.GetValue(), b_.GetValue());                \
        }                                                                        \
                                                                                 \
        void CalcGradient(const T &multiplier) const {
#define OperatorClassEnd \
    }                    \
    }                    \
    ;

OperatorClassBegin1(Sin, sin)
    a_.CalcGradient(std::cos(a_.GetValue()) * multiplier);
OperatorClassEnd
Operator1(Sin, sin);

OperatorClassBegin1(Asin, asin)
    T a_value = a_.GetValue();
    a_.CalcGradient(multiplier / std::sqrt(1.f - a_value * a_value));
OperatorClassEnd
Operator1(Asin, asin);

OperatorClassBegin1(Cos, cos)
    a_.CalcGradient(-std::sin(a_.GetValue()) * multiplier);
OperatorClassEnd
Operator1(Cos, cos);

OperatorClassBegin1(Acos, acos)
    T a_value = a_.GetValue();
    a_.CalcGradient(-multiplier / std::sqrt(1.f - a_value * a_value));
OperatorClassEnd
Operator1(Acos, acos);

OperatorClassBegin1(Tan, tan)
    T cos_a = std::cos(a_.GetValue());
    a_.CalcGradient(multiplier / (cos_a * cos_a));
OperatorClassEnd
Operator1(Tan, tan);

OperatorClassBegin1(Atan, atan)
    T a_value = a_.GetValue();
    a_.CalcGradient(multiplier / (1.f + a_value * a_value));
OperatorClassEnd
Operator1(Atan, atan);

OperatorClassBegin2(Atan2, atan2)
    T b_value = b_.GetValue();
    T ratio = a_.GetValue() / b_value;
    T d_atan = 1.f / (1.f + ratio * ratio);
    T rcp_b_mul = 1.f / b_value * multiplier;
    a_.CalcGradient(d_atan * rcp_b_mul);
    b_.CalcGradient(d_atan * -rcp_b_mul * ratio);
OperatorClassEnd
Operator2(Atan2, atan2);

OperatorClassBegin2(Pow, pow)
    T a_value = a_.GetValue(), b_value = b_.GetValue();
    T value_mul = this->value_ * multiplier;
    a_.CalcGradient(value_mul * b_value / a_value);
    b_.CalcGradient(value_mul * std::log(a_value));
OperatorClassEnd
Operator2(Pow, pow);

OperatorClassBegin1(Sqrt, sqrt)
    a_.CalcGradient(0.5f / this->value_ * multiplier);
OperatorClassEnd
Operator1(Sqrt, sqrt);

OperatorClassBegin1(Exp, exp)
    a_.CalcGradient(this->value_ * multiplier);
OperatorClassEnd
Operator1(Exp, exp);

OperatorClassBegin1(Log, log)
    a_.CalcGradient(1.f / a_.GetValue() * multiplier);
OperatorClassEnd
Operator1(Log, log);

OperatorClassBegin1(Abs, abs)
    T a_value = a_.GetValue();
    if(a_value < 0.f)
        a_.CalcGradient(-multiplier);
    else if(a_value == 0.f)
        a_.CalcGradient(0.f);
    else
        a_.CalcGradient(multiplier);
OperatorClassEnd
Operator1(Abs, abs);

} // namespace adept

/*------------------------------------------------------------------------------------------------*/
// tests

int main() {
    using namespace std;
    using namespace adept;
    using Float = float;
    // using Float = double;

#ifndef NDEBUG
    [[maybe_unused]] auto &adept = Adept<Float>::Get();
#endif

    /*--------------------------------------------------------------------------------------------*/
    // F0 and F1

    auto F0 = [](const auto &a,
                 const auto &b) -> Variable<Float> { // Must return Variable.
        Variable<Float> c = a * b;
        c = c * a + a * Constant<Float>{5.f} + Constant<Float>{13.f};
        return c;
    };
    auto F1 = [](const auto &a,
                 const auto &b) -> Variable<Float> { // Must return Variable.
        Variable<Float> c = a * b;
        c = c * b + b * Constant<Float>{7.f} + Constant<Float>{11.f};
        return c;
    };
    Variable<Float> a = 2.f, b = 3.f;
    Variable<Float> f0 = F0(a, b);
    Variable<Float> f1 = F1(a, b);

    /*--------------------------------------------------------------------------------------------*/
    // F0 and F1, forward mode

    Adept<Float>::ResetGradients();

    a.SetGradient(1.f);
    Adept<Float>::Forward();
    assert(a.GetGradient() == 1.f);
    assert(f0.GetGradient() == 17.f);
    assert(f1.GetGradient() == 9.f);

    Adept<Float>::ResetGradients();

    b.SetGradient(1.f);
    Adept<Float>::Forward();
    assert(b.GetGradient() == 1.f);
    assert(f0.GetGradient() == 4.f);
    assert(f1.GetGradient() == 19.f);

    /*--------------------------------------------------------------------------------------------*/
    // F0 and F1, reverse mode

    Adept<Float>::ResetGradients();

    f0.SetGradient(1.f);
    Adept<Float>::Reverse();
    assert(f0.GetValue() == 35.f);
    assert(f0.GetGradient() == 0.f);
    assert(a.GetGradient() == 17.f);
    assert(b.GetGradient() == 4.f);

    Adept<Float>::ResetGradients();

    f1.SetGradient(1.f);
    Adept<Float>::Reverse();
    assert(f1.GetValue() == 50.f);
    assert(f1.GetGradient() == 0.f);
    assert(a.GetGradient() == 9.f);
    assert(b.GetGradient() == 19.f);

    Adept<Float>::Clear();

    /*--------------------------------------------------------------------------------------------*/
    // F2, a complicated function using all operators

    auto F2 = [](const auto &a, const auto &b) -> Variable<Float> {
        // clang-format off
        return sin(atan(a * b)) * exp(acos(a + b))
            / (cos(log(a / b)) * tan(asin(a - b)))
            * sqrt(-a * -b + Constant<Float>{2.f}) * atan2(+a * Constant<Float>{7.f}, +b)
            / (pow(+a, -b / Constant<Float>{5.f}) * abs(-a + +b - Constant<Float>{3.f}))
            ;
        // clang-format on
    };
    Variable<Float> c = 0.2f, d = 0.3f;
    Variable<Float> f2 = F2(c, d);

    /*--------------------------------------------------------------------------------------------*/
    // F2, forward mode

    Adept<Float>::ResetGradients();

    c.SetGradient(1.f);
    Adept<Float>::Forward();
    Float df2_dc = f2.GetGradient();

    Adept<Float>::ResetGradients();

    d.SetGradient(1.f);
    Adept<Float>::Forward();
    Float df2_dd = f2.GetGradient();

    Adept<Float>::ResetGradients();

    c.SetGradient(1.f);
    d.SetGradient(1.f);
    Adept<Float>::Forward();
    Float d2f2_dc_dd = f2.GetGradient();

    cout << "Forward mode:" << endl;
    cout << "f2         = " << f2.GetValue() << endl;
    cout << "d2f2_dc_dd = " << d2f2_dc_dd << endl;
    cout << "df2_dc     = " << df2_dc << endl;
    cout << "df2_dd     = " << df2_dd << endl;

    if constexpr(std::is_same_v<Float, float>) {
        assert(std::abs(df2_dc + df2_dd - d2f2_dc_dd) < 1e-5f);
        assert(std::abs(df2_dc + df2_dd - d2f2_dc_dd) > 1e-6f);
    } else {
        assert(std::abs(df2_dc + df2_dd - d2f2_dc_dd) < 1e-14f);
        assert(std::abs(df2_dc + df2_dd - d2f2_dc_dd) > 1e-15f);
    }

    cout << endl;

    /*--------------------------------------------------------------------------------------------*/
    // F2, reverse mode

    Adept<Float>::ResetGradients();

    f2.SetGradient(1.f);
    Adept<Float>::Reverse();

    cout << "Reverse mode:" << endl;
    cout << "df2_dc     = " << c.GetGradient() << endl;
    cout << "df2_dd     = " << d.GetGradient() << endl;

    Adept<Float>::Clear();

    return 0;
}
