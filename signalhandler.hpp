#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP
#include <iostream>
#include <memory>
#include <array>
#include <vector>

#include <csignal>

struct ClassThatShouldHandleSignals{
    virtual void handleSignal(int signal) = 0;
};

class ConcreteSignalHandler : public ClassThatShouldHandleSignals
{
    void handleSignal(int signal) override{
        std::cout << "Signal " << signal << " handled" << std::endl;
    }
};

class SignalHandler
{
    SignalHandler();
public:
    //CONSTRUCTORS
    SignalHandler(const SignalHandler&) = delete;
    SignalHandler(SignalHandler&&) = delete;
    //OPERATORS
    SignalHandler& operator=(const SignalHandler&) = delete;
    SignalHandler& operator=(SignalHandler&&) = delete;
    //DESTRUCTOR
    ~SignalHandler() = default;

    //MODIFY API
    static void registerHandler(int signal, std::shared_ptr<ClassThatShouldHandleSignals> listener);
    static void raiseSignal(int signal, bool value = true);
    //READ API


protected:
    void _registerHandler(int signal, std::shared_ptr<ClassThatShouldHandleSignals> listener);
private:
    static SignalHandler &instance();
    static void _handleSignals(int signal);

#if defined __linux__
    std::array<std::vector<std::shared_ptr<ClassThatShouldHandleSignals>>, 31> m_listeners;
    std::array<bool, 31> m_signalThatShouldBeRaised;
    struct sigaction m_action;
#endif
#if defined __WIN32 || defined __WIN64
#endif
};

#endif // SIGNALHANDLER_HPP
