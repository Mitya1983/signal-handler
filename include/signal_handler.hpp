#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP
#include <functional>
#include <array>
#include <vector>
#include <memory>

#include <csignal>

namespace mt::signal_handler {

    void registerHandler(int32_t p_signal, std::function< void() > p_handler);

    template < class Object > void registerHandler(int32_t p_signal, std::weak_ptr< Object > p_object, void (Object::*p_handler)()) {
        registerHandler(p_signal, [ p_object, p_handler ] {
            if (auto l_object = p_object.lock()) {
                std::invoke(p_handler, l_object);
            }
        });
    }

    template < class Object > void registerHandler(int32_t p_signal, Object *p_object, void (Object::*p_handler)()) {
        registerHandler(p_signal, [ p_object, p_handler ] {
            if (p_object != nullptr) {
                std::invoke(p_handler, p_object);
            }
        });
    }

    template < class Object > void registerHandler(int32_t p_signal, Object& p_object, void (Object::*p_handler)()) {
        registerHandler(p_signal, [ p_object, p_handler ] {
            std::invoke(p_handler, p_object);
        });
    }

    template < class Object > void registerHandler(int32_t p_signal, const Object& p_object, void (Object::*p_handler)()) {
        registerHandler(p_signal, [ p_object, p_handler ] {
            std::invoke(p_handler, p_object);
        });
    }

    void raiseSignalAfterHandler(int32_t p_signal, bool p_value = true);

}  // namespace mt::signal_handler
#endif  // SIGNAL_HANDLER_HPP
