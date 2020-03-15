#ifndef PROXY_ASIO_THREADPOOL_HPP
#define PROXY_ASIO_THREADPOOL_HPP

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <iostream>

namespace proxy_asio {
    class ThreadPool {
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> task_queue;
        std::mutex queue_mutex;
        std::condition_variable queue_not_empty_cv;
        bool stop = false;

    public:
        ThreadPool(unsigned thread_num_multiplier = 1) {
            if (thread_num_multiplier == 0) {
                throw std::runtime_error("Zero thread num multiplier");
            }
            auto concurrent_threads_num = std::thread::hardware_concurrency() * thread_num_multiplier;
            if (concurrent_threads_num == 0) {
                concurrent_threads_num = 2;
            }

            workers.reserve(concurrent_threads_num);
            for (unsigned i = 0; i < concurrent_threads_num; i++) {
                workers.emplace_back([this]() {
                    while (!stop) {
                        std::function<void()> task;

                        std::unique_lock<std::mutex> lock(queue_mutex);
                        queue_not_empty_cv.wait(lock, [this] {
                            return stop || !task_queue.empty();
                        });

                        if (stop && task_queue.empty()) {
                            return;
                        }

                        task = std::move(task_queue.front());
                        task_queue.pop();
                        lock.unlock();

                        task();
                    }
                });
            }
        }

        ~ThreadPool() { Wait(); }

        template<typename F, typename... Args>
        std::future<typename std::result_of<F(Args...)>::type>
        Enqueue(F &&f, Args &&... args) {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args...>(args...)));

            auto res = task->get_future();
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (stop) {
                    throw std::runtime_error("Can't add task to stopped threadpool");
                }
                task_queue.push([task]() { (*task)(); });
            }
            queue_not_empty_cv.notify_one();
            return res;
        }

        void Wait() {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                stop = true;
            }
            queue_not_empty_cv.notify_all();
            for (auto &worker : workers) {
                worker.join();
            }
        }

        size_t GetWorkersNum() { return workers.size(); }
    };
}

#endif //PROXY_ASIO_THREADPOOL_HPP
