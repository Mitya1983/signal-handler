#include "signalhandler.hpp"


#include <cassert>

SignalHandler::SignalHandler()
{
#if defined __linux__ || defined __APPLE__
    m_action.sa_handler = &SignalHandler::_handleSignals;
    for (auto &var : m_signalThatShouldBeRaised){
        var = false;
    }
#endif
#if defined __WIN32 || defined __WIN64
#endif
}

void SignalHandler::registerHandler(int signal, std::shared_ptr<ClassThatShouldHandleSignals> listener)
{
    SignalHandler::instance()._registerHandler(signal, std::move(listener));
}

void SignalHandler::raiseSignal(int signal, bool value)
{
    SignalHandler::instance().m_signalThatShouldBeRaised[signal] = value;
}

void SignalHandler::_registerHandler(int signal, std::shared_ptr<ClassThatShouldHandleSignals> listener)
{

#if defined __linux__

    assert(signal > 0 && signal <= 31 && "Signal should be in range of [1 : 31]");
    assert(signal != SIGKILL && signal != SIGSTOP && "The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.");

    m_listeners[signal].emplace_back(std::move(listener));
    sigaction(signal, &m_action, nullptr);
#endif
#if defined __WIN32 || defined __WIN64
#endif

}

SignalHandler &SignalHandler::instance()
{
    static SignalHandler handler;

    return handler;
}

void SignalHandler::_handleSignals(int signal)
{
    auto listeners = SignalHandler::instance().m_listeners.at(signal);
    bool raiseSignal = SignalHandler::instance().m_signalThatShouldBeRaised.at(signal);
    for (const auto &listener : listeners){
        listener->handleSignal(signal);
    }
    if (raiseSignal){
        struct sigaction defaultAction;
        defaultAction.sa_handler = SIG_DFL;
        sigaction(signal, &defaultAction, nullptr);
        std::raise(signal);
    }
}
