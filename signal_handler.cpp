#include "signal_handler.hpp"

tristan::SignalHandler::SignalHandler() :
    m_handlers(),
    m_signalThatShouldBeRaised(),
    m_action() {
#if defined __linux__ || defined __APPLE__
    m_action.sa_handler = &SignalHandler::_handleSignals;
    for (auto& var: m_signalThatShouldBeRaised) {
        var = false;
    }
#endif
#if defined __WIN32 || defined __WIN64
#endif
}

void tristan::SignalHandler::registerHandler(int signal, std::function<void()> handler) {

    auto& signal_handler = tristan::SignalHandler::instance();

#if defined __linux__

    if (signal <= 0 || signal > 31){
        throw std::runtime_error("Signal should be in range of [1 : 31]");
    }
    if (signal == SIGKILL || signal == SIGSTOP){
        throw std::runtime_error("The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored");
    }
    signal_handler.m_handlers[signal].emplace_back(std::move(handler));
    sigaction(signal, &signal_handler.m_action, nullptr);
#endif
#if defined __WIN32 || defined __WIN64
#endif
}

[[maybe_unused]] void tristan::SignalHandler::raiseSignal(int signal, bool value) { SignalHandler::instance().m_signalThatShouldBeRaised[signal] = value; }


tristan::SignalHandler& tristan::SignalHandler::instance() {
    static tristan::SignalHandler handler;

    return handler;
}

void tristan::SignalHandler::_handleSignals(int signal) {
    auto handlers = SignalHandler::instance().m_handlers.at(signal);
    bool raiseSignal = SignalHandler::instance().m_signalThatShouldBeRaised.at(signal);
    for (const auto& handler: handlers) {
        handler();
    }
    if (raiseSignal) {
        struct sigaction defaultAction { };

        defaultAction.sa_handler = SIG_DFL;
        sigaction(signal, &defaultAction, nullptr);
        std::raise(signal);
    }
}
