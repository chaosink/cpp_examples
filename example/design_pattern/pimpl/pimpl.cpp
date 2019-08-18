// Pimpl (Private Implementation / Pointer to Implementation)

#include "Widget.hpp"

int main() {
    Widget w0(0);
    w0.Draw();

    const Widget w1(1);
    w1.Draw();
}
