#include "Widget.hpp"

#include <iostream>

class Widget::WidgetImpl {
    int n_;

public:
    WidgetImpl(int n) : n_(n) {}

    void Draw(const Widget& w) {
        if(w.Shown()) // This call to public member function requires the back-reference.
            std::cout << "Drawing a non-const widget " << n_ << '\n';
    }
    void Draw(const Widget& w) const {
        if(w.Shown())
            std::cout << "Drawing a     const widget " << n_ << '\n';
    }
};

Widget::Widget(int n) :
    pimpl_{std::make_unique<WidgetImpl>(n)},
    pimpl_no_propagate_const_{std::make_unique<WidgetImpl>(n)} {}
Widget::~Widget() = default;

Widget::Widget(Widget&&) = default;

Widget& Widget::operator=(Widget&&) = default;

void Widget::Draw() {
    pimpl_->Draw(*this);                    // Call non-const `Draw()`.
    pimpl_no_propagate_const_->Draw(*this); // Call non-const `Draw()`.
}
void Widget::Draw() const {
    pimpl_->Draw(*this);                    // Call     const `Draw()`.
    pimpl_no_propagate_const_->Draw(*this); // Call non-const `Draw()`.
}
