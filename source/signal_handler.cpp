#include "signal_handler.hpp"

#include <mutex>

#if __has_include("mutex/mt_mutex.hpp")
  #include "mutex/mutex.hpp"
using mutex = mt::mutex::Mutex;
#else
using mutex = std::mutex;
#endif


namespace {
    void handleSignal(int32_t p_signal);

    struct SignalHandler {
        SignalHandler() {
#if defined __linux__ || defined __APPLE__
            m_action.sa_handler = &handleSignal;
            for (auto& var: m_signalThatShouldBeRaised) {
                var = false;
            }
#endif
#if defined __WIN32 || defined __WIN64
#endif
        }

        SignalHandler(const SignalHandler&) = delete;
        SignalHandler(SignalHandler&&) = delete;
        SignalHandler& operator=(const SignalHandler&) = delete;
        SignalHandler& operator=(SignalHandler&&) = delete;
        ~SignalHandler() = default;
        mutex m_mutex;
#if defined __linux__
        std::array< std::vector< std::function< void() > >, 32 > m_handlers{};
        std::array< bool, 32 > m_signalThatShouldBeRaised{};
        struct sigaction m_action{};
#endif
#if defined __WIN32 || defined __WIN64
#endif
    };

    inline SignalHandler g_signal_handler;
}  // namespace

void mt::signal_handler::registerHandler(const int32_t p_signal, std::function< void() > p_handler) {
#if defined __linux__
    if (p_signal <= 0 || p_signal > 31) {
        throw std::range_error("Signal should be in range of [1 : 31]");
    }
    if (p_signal == SIGKILL || p_signal == SIGSTOP) {
        throw std::invalid_argument("The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored");
    }
    std::scoped_lock lock(g_signal_handler.m_mutex);
    g_signal_handler.m_handlers[ p_signal ].emplace_back(std::move(p_handler));
    sigaction(p_signal, &g_signal_handler.m_action, nullptr);
#endif
#if defined __WIN32 || defined __WIN64
#endif
}

void mt::signal_handler::raiseSignalAfterHandler(const int32_t p_signal, const bool p_value) {
    g_signal_handler.m_signalThatShouldBeRaised[ p_signal ] = p_value;
}

namespace {
    void handleSignal(const int32_t p_signal) {
        std::scoped_lock lock(g_signal_handler.m_mutex);
        auto handlers = g_signal_handler.m_handlers.at(p_signal);
        const bool raiseSignal = g_signal_handler.m_signalThatShouldBeRaised.at(p_signal);
        for (const auto& handler: handlers) {
            handler();
        }
        if (raiseSignal) {
            struct sigaction defaultAction{};

            defaultAction.sa_handler = SIG_DFL;
            sigaction(p_signal, &defaultAction, nullptr);
            std::raise(p_signal);
        }
    }
}  // namespace
