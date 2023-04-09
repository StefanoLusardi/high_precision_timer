#pragma once

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <future>
#include <thread>
#include <memory>

using namespace std::chrono_literals;

namespace hpt
{
using clock = std::chrono::steady_clock;

class timer
{
private:
    class task
    {
    private:
        struct task_base
        {
            virtual ~task_base() = default;
            virtual void invoke() = 0;
        };

        template<typename FunctionType>
        struct task_impl : public task_base
        {
            explicit task_impl(FunctionType&& f)
                : _func{ std::move(f) } 
            { 
                static_assert(std::is_invocable_v<decltype(f)>); 
            }
            void invoke() override { _func(); }
            FunctionType _func;
        };

    public:
        template<typename FunctionType>
        explicit task(FunctionType&& f) 
            : _impl{ std::make_unique<task_impl<FunctionType>>(std::move(f)) } 
        { }

        task(task&& other) noexcept 
            : _impl{ std::move(other._impl) } 
        { }

        task(task&) = delete;
        task(const task&) = delete;
        task& operator=(const task&) = delete;
        task& operator=(task&&) = delete;
        ~task() = default;
        
        void invoke() { _impl->invoke(); }

    private:
        std::unique_ptr<task_base> _impl;
    };

public:
    explicit timer()
        : _is_running{true}
        , _interval{hpt::clock::duration::max()}
    { }

    timer(timer&) = delete;
    timer(const timer&) = delete;
    timer(timer&&) noexcept = delete;
    timer& operator=(const timer&) = delete;
    timer& operator=(timer&&) = delete;
    ~timer() { stop(); }
    
    bool start(hpt::clock::duration&& interval)
    {
        if(!_task)
            return false;
        
        _next_task_timepoint = hpt::clock::now() + interval;
        _interval = interval;
        
        std::promise<void> thread_started_notifier;
        std::future<void> thread_started_watcher = thread_started_notifier.get_future();
        
        _worker_thread = std::thread([this, &thread_started_notifier]{
            thread_started_notifier.set_value(); worker();
        });

        thread_started_watcher.wait();
        return true;
    }
    
    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(_worker_mutex);
            _is_running = false;
        }

        _worker_cv.notify_all();
        
        if (_worker_thread.joinable())
            _worker_thread.join();
    }

    template <typename TaskFunction, typename... Args>
    void set_callback(TaskFunction &&func, Args &&... args)
    {
        auto task_func = [t = std::forward<TaskFunction>(func), params = std::make_tuple(std::forward<Args>(args)...)] 
        {
            return std::apply(t, params);
        };

        _task = std::make_unique<hpt::timer::task>(std::move(task_func));
    }

private:
    bool _is_running;
    hpt::clock::duration _interval;
    std::unique_ptr<hpt::timer::task> _task;
    hpt::clock::time_point _next_task_timepoint;
    std::thread _worker_thread;
    std::condition_variable _worker_cv;
    std::mutex _worker_mutex;

    void worker()
    {
        std::unique_lock lock(_worker_mutex);
        while (_is_running)
        {
            if (_worker_cv.wait_until(lock, _next_task_timepoint, [this]{return !_is_running; }))
                break;

            _task->invoke();
            update_next_start_time();
        }
    }

    void update_next_start_time()
    {        
        auto task_next_start_time = _next_task_timepoint;
        const auto now = hpt::clock::now();
        while(now > task_next_start_time)
            task_next_start_time += _interval;

        _next_task_timepoint = task_next_start_time;
    }
};

}
