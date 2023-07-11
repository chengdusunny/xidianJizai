#include "Timer.h"

Timer::Timer(const Timeout& timeout)
	: _timeout(timeout) {

}

Timer::Timer(const Timeout& timeout, const Interval& interval, bool singleShot)
	:_timeout(timeout), _interval(interval), _isSingleShot(singleShot) {

}

void Timer::start(bool multiThread) {
	if (this->running() == true)
		return;

	_running = true;

	if (multiThread == true) {
		_thread = std::thread(
			&Timer::_temporize, this);
	}
	else {
		this->_temporize();
	}
}

void Timer::stop() {
	_running = false;
	_thread.join();
}

bool Timer::running() const{
	return _running;
}

void Timer::setSingleShot(bool singleShot) {
	if (this->running())
		return;

	_isSingleShot = singleShot;
}

bool Timer::isSingleShot() const {
	return _isSingleShot;
}

void Timer::setInterval(const Timer::Interval& interval) {
	if (this->running())
		return;
	
	_interval = interval;
}

const Timer::Interval& Timer::interval() const {
	return _interval;
}

void Timer::setTimeout(const Timeout& timeout) {
	if (this->running())
		return;
	_timeout = timeout;
}

const Timer::Timeout& Timer::timeout() const {
	return _timeout;
}

void Timer::_temporize() {
	if (_isSingleShot)
		this->_sleepThenTimeout();
	else
		while (this->running())
			this->_sleepThenTimeout();
}

void Timer::_sleepThenTimeout() {
	std::this_thread::sleep_for(_interval);
	if (this->running())
		this->timeout()();
}