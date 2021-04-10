#include <iostream>
#include "signalhandler.hpp"
#include <thread>
using namespace std;

int main()
{
    auto test = std::make_shared<ConcreteSignalHandler>();
    auto test1 = std::make_shared<ConcreteSignalHandler>();

    SignalHandler::registerHandler(SIGINT, test);

    SignalHandler::registerHandler(SIGUSR1, test1);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Message" << std::endl;
    }

    return 0;
}
