#pragma once

#include <spdlog/spdlog.h>

using namespace std::chrono_literals;

namespace hpt::utils
{
class timer
{
public:
    explicit timer() { start(); }
    ~timer() { stop(); }

protected:
    void start()
    {
        is_running = true;
        timer_thread = std::thread([this]
        {
            unsigned i = 0; 
            while(is_running) 
            {
                spdlog::info("[{}]", i++);
                std::this_thread::sleep_for(1s);
            }
        });
    }

    void stop()
    {
        if (!is_running)
            return;

        is_running.store(false);        
        if (timer_thread.joinable())
            timer_thread.join();
    }

private:
    std::atomic_bool is_running;
    std::thread timer_thread;
};

}
