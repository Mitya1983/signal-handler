#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP
#include <iostream>
#include <functional>
#include <array>
#include <vector>
#include <memory>

#include <csignal>

namespace tristan {

    class SignalHandler {
        SignalHandler();

    public:
        SignalHandler(const SignalHandler&) = delete;
        SignalHandler(SignalHandler&&) = delete;
        SignalHandler& operator=(const SignalHandler&) = delete;
        SignalHandler& operator=(SignalHandler&&) = delete;
        ~SignalHandler() = default;

        static void registerHandler(int signal, std::function< void() > handler);

        template < class Object > static void registerHandler(int signal, std::weak_ptr< Object > object, void (Object::*functor)()) {
            SignalHandler::registerHandler(signal, [object, functor] {
                if (auto l_object = object.lock()) {
                    std::invoke(functor, l_object);
                }
            });
        }

        template < class Object > static void registerHandler(int signal, Object* object, void (Object::*functor)()) {
            SignalHandler::registerHandler(signal, [object, functor] {
                if (object != nullptr) {
                    std::invoke(functor, object);
                }
            });
        }

        template < class Object > static void registerHandler(int signal, Object& object, void (Object::*functor)()) {
            SignalHandler::registerHandler(signal, [object, functor] {
                std::invoke(functor, object);
            });
        }
        template < class Object > static void registerHandler(int signal, const Object& object, void (Object::*functor)()) {
            SignalHandler::registerHandler(signal, [object, functor] {
                std::invoke(functor, object);
            });
        }

        static void raiseSignal(int signal, bool value = true);

    protected:
    private:
        static SignalHandler& instance();
        static void _handleSignals(int signal);

#if defined __linux__
        std::array< std::vector< std::function< void() > >, 31 > m_handlers;
        std::array< bool, 31 > m_signalThatShouldBeRaised;
        struct sigaction m_action;
#endif
#if defined __WIN32 || defined __WIN64
#endif
    };

}  // namespace tristan
#endif  // SIGNAL_HANDLER_HPP
