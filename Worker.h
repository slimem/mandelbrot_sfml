#pragma once

#include "main.h"
#include <thread>
#include <mutex>
#include <condition_variable>


class Worker {
public:
	Worker();
	Worker(const Worker&) = delete;
	Worker(const Worker&&) = delete;

	constexpr double scaleab(
		double a1, double a2,
		double b1, double b2) {
		return (a1 - a2) / (b1 - b2);
	}

	void init(
		const sf::Vector2<double>& pixtl,
		const sf::Vector2<double>& pixbr,
		const sf::Vector2<double>& fractl,
		const sf::Vector2<double>& fracbr,
		int* pfractal,
		const int iters
	);

	void createFractal();

	void setAlive(bool isAlive) {
		_alive = isAlive;
	}

	static std::atomic<int>		workerComplete;
	std::thread					_thread;
	std::condition_variable		_cvStart;
private:
	//thread related data
	std::mutex					_mutex;
	bool						_alive;

	size_t						_width;
	size_t						_height;
	int*						_pfractal;
	int							_iters;

	sf::Vector2<double>			_pixtl;
	sf::Vector2<double>			_pixbr;
	sf::Vector2<double>			_fractl;
	sf::Vector2<double>			_fracbr;
};