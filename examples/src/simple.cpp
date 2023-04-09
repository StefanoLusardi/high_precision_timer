#include <hpt/timer.hpp>
#include <chrono>
#include <iostream>

int main() 
{
    auto now = []{
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    };

    hpt::timer s;
    s.set_callback([&]{ std::cout << now() << std::endl; });
    s.start(500ms);
    
    std::this_thread::sleep_for(5s);

    std::cout << "shutdown" << std::endl;
    s.stop();

    std::cout << "finished" << std::endl;
    return 0;
}
