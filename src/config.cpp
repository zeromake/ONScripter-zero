#include "config.hpp"

namespace onscripter {
    Defer::Defer(const std::function<void(void)>& act): action(act) {};
    Defer::Defer(const std::function<void(void)>&& act): action(std::move(act)) {};
    Defer::~Defer() {
        action();
    };
}
