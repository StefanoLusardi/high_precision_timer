#include <hpt/timer.hpp>
#include <spdlog/spdlog.h>
#include "utils.hpp"

int main() 
{
    spdlog::set_pattern("[%H:%M:%S.%e] %v");

    hpt::timer s;
    s.set_callback([]{ spdlog::info("@"); });
    s.start(10ms);
    
    hpt::utils::timer t;
    
    std::this_thread::sleep_for(5s);

    spdlog::info("shutdown");
    s.stop();

    spdlog::info("finished");
    return 0;
}
