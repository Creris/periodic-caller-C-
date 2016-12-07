#include "Periodic.hpp"

#include <thread>
#include <future>
#include <chrono>
#include <iostream>

auto currentTimeMicro()
{
	auto nowUndef = std::chrono::high_resolution_clock::now().time_since_epoch();
	auto nowMicro = std::chrono::duration_cast<std::chrono::microseconds>(nowUndef);
	return nowMicro.count();
}

class _SingleTimedInstance{
	class _LockedVector{
		std::mutex lock;
		std::vector<Periodic*> vector;
	public:
		std::vector<Periodic*>& acquire()
		{
			lock.lock();
			return vector;
		}

		void release()
		{
			lock.unlock();
		}
	};
	_LockedVector vector;
	std::thread looper;
	std::atomic_bool doStop;
public:
	void subscribe(Periodic* periodic)
	{
		auto& vec = vector.acquire();
		vec.push_back(periodic);
		vector.release();
	}

	void unsubscribe(Periodic* periodic)
	{
		auto& vec = vector.acquire();
		vec.erase(std::remove(vec.begin(), vec.end(), periodic), vec.end());
		vector.release();
	}

	_SingleTimedInstance()
	{
		doStop.store(false);
		looper.swap(std::thread{ std::bind(&_SingleTimedInstance::periodicCaller, this) });
	}

	void periodicCaller()
	{
		while(!doStop.load())
		{
			auto& vec = vector.acquire();

			auto time = currentTimeMicro();
			for(auto& periodic : vec)
			{
				if(periodic && periodic->funcs.size())
					periodic->_check(time);
			}

			vector.release();

			std::this_thread::yield();
		}
	}

	~_SingleTimedInstance()
	{
		doStop.store(true);
		looper.join();
		vector.acquire();
		vector.release();
	}
};

_SingleTimedInstance _timedInstance;

void Periodic::_check(std::uint64_t now)
{
	for(std::size_t i = 0; i < funcs.size(); ++i)
	{
		_Data& dat = funcs[i];

		if(now >= dat.nextInvoke)
		{
			dat.func();
			if(!dat.flags.test(0))
			{
				funcs.erase(funcs.begin() + i);
				--i;
			}
			else
			{
				dat.nextInvoke += dat.timeout;
			}
		}
	}
}

void Periodic::_removeFunc(size_t index)
{
	funcs.erase(funcs.begin() + index);
}

void Periodic::_removeFunc()
{
	funcs.clear();
}

void Periodic::_commonInit()
{
	_timedInstance.subscribe(this);
	startTime = currentTimeMicro();
}

Periodic::Periodic()
{
	_commonInit();
}

Periodic::Periodic(Periodic && p) : startTime{ p.startTime }, funcs{ p.funcs }
{
	_commonInit();
}

Periodic::Periodic(const Periodic & p) : startTime{ p.startTime }, funcs{ p.funcs }
{
	_commonInit();

}

Periodic::~Periodic()
{
	_timedInstance.unsubscribe(this);
}

void Periodic::restart()
{
	startTime = currentTimeMicro();
}

void Periodic::addFunction(Function func, std::uint64_t timeoutMicroseconds, bool periodic)
{
	_Data dat;
	dat.func = func;
	dat.timeout = timeoutMicroseconds;
	dat.nextInvoke = dat.timeout + currentTimeMicro();
	dat.flags.set(0, periodic);

	funcs.push_back(dat);
}
