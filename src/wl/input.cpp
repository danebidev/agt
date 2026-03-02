#include <agt/backend/wl-input.hpp>

#include <dwhbll/console/debug.hpp>
#include <dwhbll/console/Logging.h>

#include <sys/mman.h>
#include <unistd.h>

using namespace dwhbll::console;

namespace agt::wayland {

void wl_pointer_enter(void* data, wl_pointer* wl_pointer, uint32_t serial, wl_surface* surface, 
                      wl_fixed_t x, wl_fixed_t y) {
    // trace("wl_pointer_enter");

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::ENTER);
    pointer->ptr_state.enter_surface = surface;
    pointer->ptr_state.serial = serial;
    pointer->ptr_state.enter_serial = serial;
    pointer->ptr_state.x = wl_fixed_to_double(x);
    pointer->ptr_state.y = wl_fixed_to_double(y);
}

void wl_pointer_leave(void* data, wl_pointer* wl_pointer, uint32_t serial, wl_surface* surface) {
    // trace("wl_pointer_leave");

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::LEAVE);
    pointer->ptr_state.serial = serial;
    pointer->ptr_state.leave_surface = surface;
}

void wl_pointer_motion(void* data, wl_pointer* wl_pointer, uint32_t serial, 
                       wl_fixed_t x, wl_fixed_t y) {
    // trace("wl_pointer_motion");

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::MOTION);
    pointer->ptr_state.serial = serial;
    pointer->ptr_state.x = wl_fixed_to_double(x);
    pointer->ptr_state.y = wl_fixed_to_double(y);
}

void wl_pointer_button(void* data, wl_pointer* wl_pointer, uint32_t serial, 
                       uint32_t time, uint32_t button, uint32_t state) {
    // trace("wl_pointer_button");

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::BUTTON);
    pointer->ptr_state.button = button;
    pointer->ptr_state.button_state = state;
    pointer->ptr_state.serial = serial;
}

void wl_pointer_axis(void* data, wl_pointer* wl_pointer, uint32_t time,
                     uint32_t axis, wl_fixed_t value) {
    // trace("wl_pointer_axis | axis: {} value: {}", axis, wl_fixed_to_double(value));

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::AXIS);
    pointer->ptr_state.time = time;

    pointer->ptr_state.axes[axis].valid = true;
    pointer->ptr_state.axes[axis].value = wl_fixed_to_int(value);
}

void wl_pointer_frame(void* data, wl_pointer* wl_pointer) {
    // trace("wl_pointer_frame");

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    bool enter = pointer->ptr_state.events & static_cast<uint32_t>(PointerMask::ENTER);

    if(enter) {
        pointer->input.ptr_enter(pointer->ptr_state.enter_serial, 
                                 pointer->ptr_state.enter_surface);
    }

    if(pointer->ptr_state.events & static_cast<uint32_t>(PointerMask::LEAVE)) {
        pointer->input.ptr_leave(pointer->ptr_state.serial, pointer->ptr_state.leave_surface);
        if(!enter || pointer->ptr_state.enter_surface == pointer->ptr_state.leave_surface) {
            pointer->ptr_state.reset();
            return;
        }
    }

    if(pointer->ptr_state.events & static_cast<uint32_t>(PointerMask::MOTION)) {
        pointer->input.ptr_motion(pointer->ptr_state.serial, pointer->ptr_state.x,
                                  pointer->ptr_state.y);
    }

    if(pointer->ptr_state.events & static_cast<uint32_t>(PointerMask::BUTTON)) {
        pointer->input.ptr_click(pointer->ptr_state.serial, pointer->ptr_state.time,
                                 pointer->ptr_state.button, pointer->ptr_state.button_state);
    }

    if(pointer->ptr_state.events & static_cast<uint32_t>(PointerMask::AXIS)) {
        int hscroll = 0, vscroll = 0;
        for(int i = 0; i < 2; i++) {
            if(!pointer->ptr_state.axes[i].valid)
                continue;

            if(i == 0)
                vscroll = pointer->ptr_state.axes[i].value;
            else
                hscroll = pointer->ptr_state.axes[i].value;

            pointer->ptr_state.axes[i].valid = false;
        }
        pointer->input.ptr_scroll(pointer->ptr_state.time, hscroll, vscroll);
    }

    pointer->ptr_state.reset();
}

void wl_pointer_axis_source(void* data, wl_pointer* wl_pointer, uint32_t source) {
    // trace("wl_pointer_axis_source | source: {}", source);

    Pointer* pointer = static_cast<Pointer*>(data);
    ASSERT(wl_pointer == pointer->pointer.get());

    pointer->ptr_state.events |= static_cast<uint32_t>(PointerMask::AXIS_SOURCE);
    pointer->ptr_state.axis_source = source;
}

void wl_pointer_axis_stop(void* data, wl_pointer* wl_pointer, uint32_t time, uint32_t axis) {
    // trace("wl_pointer_axis_stop | time: {} axis: {}", time, axis);

    // TODO
    warn("Unimplemented (wl_pointer_axis_stop)");
}

void wl_pointer_axis_value120(void* data, wl_pointer* pointer, uint32_t axis, int32_t value) {
    // trace("wl_pointer_axis_value120 axis: {} value: {}", axis, value);

    // TODO
    warn("Unimplemented (wl_pointer_axis_value120)");
}

const struct wl_pointer_listener wl_pointer_list = {
    .enter = wl_pointer_enter,
    .leave = wl_pointer_leave,
    .motion = wl_pointer_motion,
    .button = wl_pointer_button,
    .axis = wl_pointer_axis,
    .frame = wl_pointer_frame,
    .axis_source = wl_pointer_axis_source,
    .axis_stop = wl_pointer_axis_stop,
    .axis_value120 = wl_pointer_axis_value120
};

void wl_keyboard_keymap(void* data, wl_keyboard* wl_keyboard, uint32_t format,
                        int fd, uint32_t size) {
    trace("wl_keyboard_keymap");

    Keyboard* keyboard = static_cast<Keyboard*>(data);
    ASSERT(wl_keyboard == keyboard->keyboard.get());

    // TODO: proper error checking
    ASSERT(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);

    char* addr = (char*)mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    // TODO: proper error checking
    ASSERT(addr);
    std::string s(addr, size);

    keyboard->xkb_state.reset();
    keyboard->xkb_keymap.reset();

    ASSERT(keyboard->xkb_context);
    keyboard->xkb_keymap.reset(xkb_keymap_new_from_string(keyboard->xkb_context.get(), s.c_str(), 
                                                          XKB_KEYMAP_FORMAT_TEXT_V1,
                                                          XKB_KEYMAP_COMPILE_NO_FLAGS));

    munmap(addr, size);
    close(fd);

    keyboard->xkb_state.reset(xkb_state_new(keyboard->xkb_keymap.get()));
}

void wl_keyboard_enter(void* data, wl_keyboard* wl_keyboard, uint32_t serial,
                       wl_surface* surface, wl_array* keys) {
    // TRACE_FUNC(wl_keyboard_enter);

    Keyboard* keyboard = static_cast<Keyboard*>(data);
    ASSERT(wl_keyboard == keyboard->keyboard.get());

    keyboard->input.kb_enter(serial, surface);

    uint32_t *key;
    // wl_array_for_each, but expanded because it's missing a cast and C++ complains
    for (key = (uint32_t*) (keys)->data;
         (keys)->size != 0 &&
         (const char *)key < ((const char *)(keys)->data + (keys)->size);
         (key)++) {
        // TODO
    }
}

void wl_keyboard_leave(void* data, wl_keyboard* wl_keyboard, uint32_t serial,
                        wl_surface* surface) {
    // TRACE_FUNC(wl_keyboard_leave);

    Keyboard* keyboard = static_cast<Keyboard*>(data);
    ASSERT(wl_keyboard == keyboard->keyboard.get());

    keyboard->input.kb_leave(serial, surface);
}

void wl_keyboard_key(void* data, wl_keyboard* wl_keyboard, uint32_t serial,
                        uint32_t time, uint32_t key, uint32_t state) {
    // TRACE_FUNC(wl_keyboard_key);

    Keyboard* keyboard = static_cast<Keyboard*>(data);
    ASSERT(wl_keyboard == keyboard->keyboard.get());

    keyboard->input.kb_key(serial, time, key, state);
}

void wl_keyboard_modifiers(void* data, wl_keyboard* keyboard, uint32_t serial,
                     uint32_t mods_pressed, uint32_t mods_latched,
                     uint32_t mods_locked, uint32_t group) {
    // TRACE_FUNC(wl_keyboard_modifiers);

    // TODO
    warn("Unimplemented (wl_keyboard_modifiers)");
}

void wl_keyboard_repeat_info(void* data, wl_keyboard* keyboard, int rate, int delay) {
    // TRACE_FUNC(wl_keyboard_repeat_info);

    // TODO
    warn("Unimplemented (wl_keyboard_repeat_info)");
}

const struct wl_keyboard_listener wl_keyboard_list = {
    .keymap = wl_keyboard_keymap,
    .enter = wl_keyboard_enter,
    .leave = wl_keyboard_leave,
    .key = wl_keyboard_key,
    .modifiers = wl_keyboard_modifiers,
    .repeat_info = wl_keyboard_repeat_info,
};

Pointer::Pointer(InputManager& input_, wl_pointer* pointer_) 
        : input(input_),
          pointer(pointer_, &wl_pointer_release),
          cursor_shape_pointer(nullptr, wp_cursor_shape_device_v1_destroy) {
    cursor_shape_pointer.reset(wp_cursor_shape_manager_v1_get_pointer(input.cursor_shape.get(),
                                                                      pointer.get()));
    wl_pointer_add_listener(pointer.get(), &wl_pointer_list, this);

    if(!cursor_shape_pointer)
        dwhbll::debug::panic("failed to get a cursor_shape_device for the wl_pointer");
}

Keyboard::Keyboard(InputManager& input_, wl_keyboard* keyboard_) 
    : input(input_),
      keyboard(keyboard_, &wl_keyboard_release), 
      xkb_context(nullptr, &xkb_context_unref),
      xkb_keymap(nullptr, &xkb_keymap_unref),
      xkb_state(nullptr, &xkb_state_unref) {
    wl_keyboard_add_listener(keyboard.get(), &wl_keyboard_list, this);
    xkb_context.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
}

void wl_seat_capabilities(void* data, wl_seat* wl_seat, uint32_t cap) {
    trace("wl_seat_capabilities");

    InputManager* input = static_cast<InputManager*>(data);
    ASSERT(wl_seat == input->seat.get());

    bool has_ptr = cap & WL_SEAT_CAPABILITY_POINTER;
    bool has_kb = cap & WL_SEAT_CAPABILITY_KEYBOARD;

    if(has_ptr && !input->pointer) {
        input->pointer = std::make_unique<Pointer>(*input, wl_seat_get_pointer(wl_seat));
    }
    else if(!has_ptr && input->pointer) {
        input->pointer.reset();
    }

    if(has_kb && !input->keyboard) {
        input->keyboard = std::make_unique<Keyboard>(*input, wl_seat_get_keyboard(wl_seat));
    }
    else if(!has_kb && input->keyboard) {
        input->keyboard.reset();
    }
}

const struct wl_seat_listener wl_seat_list = {
    .capabilities = wl_seat_capabilities,
    .name = [](void*, wl_seat*, const char*) {}
};

InputManager::InputManager(Display& display_) 
    : display(display_), 
      seat(nullptr, &wl_seat_release),
      cursor_shape(nullptr, &wp_cursor_shape_manager_v1_destroy) {
    display.global_event.subscribe([&](auto unsub, std::string interface, uint32_t name) {
        if(interface == wl_seat_interface.name) {
            TRACE_FUNC("binding wl_seat (version 8)");
            seat.reset((wl_seat*) wl_registry_bind(display.registry(), name,
                                                   &wl_seat_interface, 8));
            wl_seat_add_listener(seat.get(), &wl_seat_list, this);
        }
        else if(interface == wp_cursor_shape_manager_v1_interface.name) {
            TRACE_FUNC("binding wp_cursor_shape_manager_v1 (version 1)");
            cursor_shape.reset((wp_cursor_shape_manager_v1*) wl_registry_bind(display.registry(), name,
                                                                &wp_cursor_shape_manager_v1_interface, 1));
        }
    });
}

} // namespace agt::wl
