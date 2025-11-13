#pragma once

#include <vector>
#include <functional>

namespace agt::utils {

template <typename ...Args>
class Signal {
private:
    std::vector<std::function<void(Args...)>> subscribers;

public:
    void subscribe(std::function<void(Args...)> f) { subscribers.push_back(f); }

    void operator()(Args... args) {
        for(auto &f : subscribers) {
            f(args...);
        }
    }
};

} // namespace agt::utils
