#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <runtime/core/instance_util.h>
#include <editor/renderer_console.h>

class ThreadPool {
public:
	ThreadPool(size_t);
	template<class F, class... Args>
	auto AddTask(const char* task_name, F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>;
	~ThreadPool();
private:
	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;
	// the task queue
	std::queue< std::function<void()> > tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};

class ThreadPoolManager : Singleton<ThreadPoolManager>
{
public:
    ThreadPool* pool;
    ThreadPoolManager()
    {
        pool = new ThreadPool(10);
    }

    ~ThreadPoolManager()
    {
        delete pool;
        pool = nullptr;
    }

    template<class F, class... Args>
    static auto AddTask(const char* task_name, F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        return GetInstance()->pool->AddTask(task_name, f, std::forward<Args>(args)...);
    }
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
	: stop(false)
{
	for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
			{
				for (;;)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						if (this->stop && this->tasks.empty())
							return;
						task = std::move(this->tasks.front());
						this->tasks.pop();
					}

					task();
				}
			}
	);
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::AddTask(const char* task_name, F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    Console->AddWarn("Begin Task: %s", task_name);
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");
		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
    Console->AddWarn("End Task: %s", task_name);
	return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : workers)
		worker.join();
}