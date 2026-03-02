#pragma once

#include <sys/epoll.h>

#include <memory>
#include <vector>
#include <functional>

namespace agt::utils {

template <typename ...Args>
struct Signal {
public:
    struct Callback {
        std::function<void(Args...)> f;
        bool active = true;
    };
    using Unsub = std::function<void()>;

    template <typename F>
    void subscribe(F&& f) {
        auto cb = std::make_shared<Callback>();

        std::weak_ptr<Callback> weak = cb;

        Unsub unsub = [weak]() {
            if (auto s = weak.lock()) {
                s->active = false;
            }
        };

        cb->f = [f = std::forward<F>(f), unsub](Args... args) mutable {
            f(unsub, args...);
        };

        subscribers.push_back(std::move(cb));
    }

    void operator()(Args... args) {
        ++depth;
        for(auto &f : subscribers) {
            if(f->active)
                f->f(args...);
        }
        --depth;

        if(depth == 0) {
            subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(),
                                             [](const std::shared_ptr<Callback>& s) {
                                                return !s->active;
                                             }), subscribers.end());
        }
    }

private:
    std::vector<std::shared_ptr<Callback>> subscribers;
    int depth = 0;
};

struct EventLoop {
    using cb = std::function<void(uint32_t events)>;
public:
    EventLoop();
    ~EventLoop();

    void start();
    void stop() { running = false; }

    void add(int fd, uint32_t ev, cb&& callback);
    void mod(int fd, uint32_t ev, cb&& callback);
    void remove(int fd);

    Signal<> pre_poll;
    Signal<> post_poll;

private:
    int epoll_fd = -1;
    bool running = false;

    std::vector<epoll_event> events;
    std::unordered_map<int, cb> callbacks;
};

} // namespace agt::utils
