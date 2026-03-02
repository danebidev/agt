#include <agt/event.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <cstring>
#include <unistd.h>

using namespace dwhbll::console;

namespace agt::utils {

EventLoop::EventLoop() : events(16) {
    epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
        dwhbll::debug::panic("epoll_create1: {}", strerror(errno));
}

EventLoop::~EventLoop() {
    close(epoll_fd);
}

void EventLoop::start() {
    TRACE_FUNC("starting event loop");
    running = true;

    while(running) {
        TRACE_FUNC("event loop");
        pre_poll();

        int n = epoll_wait(epoll_fd, events.data(), events.size(), 50);
        if(n == -1) {
            if(errno == EINTR)
                continue;
            dwhbll::debug::panic("epoll_wait: {}", strerror(errno));
        }

        for(int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            callbacks[fd](events[i].events);
        }

        post_poll();
    }
}

void EventLoop::add(int fd, uint32_t ev, cb&& callback) {
    epoll_event e {};
    e.events = ev;
    e.data.fd = fd;
    callbacks[fd] = std::move(callback);

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &e) == -1)
        dwhbll::debug::panic("EPOLL_CTL_ADD: {}", strerror(errno));
}

void EventLoop::mod(int fd, uint32_t ev, cb&& callback) {
    epoll_event e {};
    e.events = ev;
    e.data.fd = fd;
    callbacks[fd] = std::move(callback);

    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &e) == -1)
        dwhbll::debug::panic("EPOLL_CTL_MOD: {}", strerror(errno));
}

void EventLoop::remove(int fd) {
    if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1)
        dwhbll::debug::panic("EPOLL_CTL_DEL: {}", strerror(errno));

    callbacks.erase(fd);
}

} // namespace agt::utils
