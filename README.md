# Periodic caller for C++

This class allows you to run multiple functions in background on timer.

This class is written in pure C++11, so should be compatible with any compiler supporting C++11.
This class requires the usage of std::thread, so you may have to link against external threading libraries(such as with gcc and pthread).

## API
### Typedefs
* ```Periodic::Function = std::function<void()>```
	* Denotes the type of function that can be added to Periodic instance to call.

### Functions
* ```Periodic::restart()```
	* Restarts the timer for given instance.
	* Only affects functions that are called periodically.
* ```Periodic::addFunction(Periodic::Function function, uint64_t timeoutMicro, bool periodic)```
	*  Adds function to internal list to call after at least timeoutMicro has passed.
	* If periodic is ```false```, the function is removed from the list after it has been invoked.

Apart from those functions, the class provides generic copy, move, normal constructor as well as copy and move assignment operators.