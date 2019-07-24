#pragma once

#include <memory>
#include <experimental/propagate_const>

class Widget {
    class WidgetImpl;

    std::experimental::propagate_const<
        std::unique_ptr<WidgetImpl>
    > pimpl_;

    std::unique_ptr<WidgetImpl> pimpl_no_propagate_const_; // Bat usage, just for test.

public:
    Widget(int);
    ~Widget(); // Defined in the implementation file, where `WidgeImpl` is a complete type.

    // Note: calling Draw() on moved-from object is UB.
    Widget(Widget&&); // Defined in the implementation file.
    Widget(const Widget&) = delete;

    Widget& operator=(Widget&&); // Defined in the implementation file.
    Widget& operator=(const Widget&) = delete;

    void Draw(); // Public API that will be forwarded to the implementation.
    void Draw() const;

    // Public API that implementation has to call, hence back-reference required.
    bool Shown() const { return true; }
};
