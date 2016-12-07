
#ifndef HEADER_PERIODIC
#define HEADER_PERIODIC

#include <vector>
#include <functional>
#include <bitset>

class Periodic{
public:
	using Function = std::function<void()>;
private:
	friend class _SingleTimedInstance;
	struct _Data{
		Function func;
		std::uint64_t timeout;
		std::uint64_t nextInvoke;
		std::bitset<8> flags;
	};

	std::vector<_Data> funcs;
	std::uint64_t startTime;

	void _check(std::uint64_t now);
	void _removeFunc(size_t index);
	void _removeFunc();
	void _commonInit();
public:
	Periodic();
	Periodic(Periodic&&);
	Periodic(const Periodic&);
	Periodic& operator=(const Periodic&) = default;
	Periodic& operator=(Periodic&&) = default;
	~Periodic();

	void restart();
	void addFunction(Function func, std::uint64_t timeOut, bool periodic);
};

#endif	//HEADER_PERIODIC