#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <future>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>

template<typename FunctionType>
class ThreadPool
{
private:

	const int counter;
	std::atomic<bool> stop;

	std::mutex mtx;
	std::condition_variable cond;

	std::vector<std::thread> threads;
	std::queue<std::packaged_task<FunctionType()>> tasks;

public:

	ThreadPool();

	void join() noexcept;

	//std::future<FunctionType> addTask(std::function<FunctionType()>&& func); // аргументы в функцию передавай через bind

	template<typename lambda>
	std::future<FunctionType> addTask(const lambda& func);

};

template<typename FunctionType>
inline ThreadPool<FunctionType>::ThreadPool() : counter(std::thread::hardware_concurrency()), stop(false)
{
	for (int i = 0; i < counter; i++)
	{
		threads.emplace_back
		(
			[this]()
			{
				for (;;)
				{
					std::packaged_task<FunctionType()> task;

					{
						std::unique_lock<std::mutex>lock(this->mtx);
						cond.wait(lock, [this] {return !this->tasks.empty() || this->stop; });
						if (this->stop && this->tasks.empty())
							break;

						task = std::move(this->tasks.front());
						this->tasks.pop();

					}

					task();
				}
			}

		);
	}
}

template<typename FunctionType>
inline void ThreadPool<FunctionType>::join() noexcept
{
	stop.store(true);
	cond.notify_all();
	for (auto& el : threads)
	{
		if (el.joinable())
			el.join();
	}
}

//template<typename FunctionType>
//inline std::future<FunctionType> ThreadPool<FunctionType>::addTask(std::function<FunctionType()>&& func)
//{
//	std::packaged_task<FunctionType()> task(func);
//	auto res = task.get_future();
//	{
//		std::unique_lock<std::mutex> lock(mtx);
//		tasks.push(std::move(task));
//	}
//	cond.notify_one();
//	return res;
//}

template<typename FunctionType>
template<typename lambda>
inline std::future<FunctionType> ThreadPool<FunctionType>::addTask(const lambda& func)
{
	std::packaged_task<FunctionType()> task(func);
	auto res = task.get_future();
	{
		std::unique_lock<std::mutex> lock(mtx);
		tasks.push(std::move(task));
	}
	cond.notify_one();
	return res;
}
