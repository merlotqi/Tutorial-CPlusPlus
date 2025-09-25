#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>

class ThreadPool
{
    std::size_t _thread_sz;
    std::size_t _max_task_sz;
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _queue;
    std::mutex _que_mtx;
    std::condition_variable _condition;
    bool _stop;

public:
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency(), size_t maxTaskSize = 128)
        : _stop(false), _thread_sz(threadCount), _max_task_sz(maxTaskSize)
    {
        Start();
    }
    size_t &MaxTaskSize()
    {
        return _max_task_sz;
    }
    size_t size()
    {
        return _thread_sz;
    }

    template<typename F, typename... Arg>
    auto enqueue(F &&func, Arg &&...arg) -> std::future<typename std::result_of<F(Arg...)>::type>
    {
        typedef typename std::result_of<F(Arg...)>::type return_type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(func), std::forward<Arg>(arg)...));
        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> l(this->_que_mtx);

            this->_condition.wait(l, [this] { return this->_stop || this->_queue.size() < _max_task_sz; });
            if (_stop)
                return res;
            _queue.push([task]() { (*task)(); });
        }
        _condition.notify_one();

        return res;
    }

    ~ThreadPool()
    {
        Stop();
    }


    void Stop()
    {
        std::unique_lock<std::mutex> l(_que_mtx);
        _stop = true;
        _condition.notify_all();
        for (std::thread &worker: _workers)
        {
            if (worker.joinable())
                worker.join();
        }
    }

private:
    void Start()
    {
        auto threadFunc = [this]() {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> l(this->_que_mtx);
                    this->_condition.wait(l, [this] { return this->_stop || !this->_queue.empty(); });
                    if (this->_queue.empty())
                        return;
                    task = std::move(this->_queue.front());
                    this->_queue.pop();
                }
                this->_condition.notify_one();
                task();
            }
        };
        for (size_t i = 0; i < _thread_sz; i++) _workers.emplace_back(threadFunc);
    }
};