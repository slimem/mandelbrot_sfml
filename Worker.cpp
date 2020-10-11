#include "Worker.h"


std::atomic<int> Worker::workerComplete = 0;

Worker::Worker() :
	_pixtl({0.f, 0.f}),
	_pixbr({ 0.f, 0.f }),
	_fractl({ 0.f, 0.f }),
	_fracbr({ 0.f, 0.f }),
	_iters(0),
	_alive(false),
	_width(0),
	_height(0),
	_pfractal(nullptr) {}

void
Worker::init(
	const sf::Vector2<double>& pixtl,
	const sf::Vector2<double>& pixbr,
	const sf::Vector2<double>& fractl,
	const sf::Vector2<double>& fracbr,
	int* pfractal,
	const int iters) {
	
	_pixtl = pixtl;
	_pixbr = pixbr;
	_fractl = fractl;
	_fracbr = fracbr;
	_pfractal = pfractal;
	_iters = iters;

	std::unique_lock<std::mutex> lock_mutex(_mutex);
	_cvStart.notify_one();
}

void
Worker::createFractal() {
	while (_alive) {
		//critical section (exclusive access signaled by lifetime of _mutex)
		std::unique_lock<std::mutex> lock_mutex(_mutex);
		_cvStart.wait(lock_mutex); //wait until notified

		double xs = scaleab(_fracbr.x, _fractl.x, _pixbr.x, _pixtl.x);
		double ys = scaleab(_fracbr.y, _fractl.y, _pixbr.y, _pixtl.y);

		double xPos = _fractl.x; //starting X position in fractal space
		double yPos = _fractl.y; //starting Y position in fractal space

		int n; //iterations calculator (if it diverges or not)
		double cr = 0;
		double ci = 0;
		double zr = 0;
		double zi = 0;
		double nr, ni; //new real, new imaginary

		for (int x = _pixtl.x; x < _pixbr.x; ++x) {
			//we start at the same Y at each X iteration
			yPos = _fractl.y;
			//calculate for pixel at [x,y] -> [x*scale,y*scale]
			for (int y = _pixtl.y; y < _pixbr.y; ++y) {
				//c = (x * xs + _fractl.x, y * ys + _fractl.y);
				//ci = xPos;
				cr = xPos;
				ci = yPos;
				zr = 0;
				zi = 0;
				n = 0;
				while (n < _iters && (zi * zi + zr * zr) < 4) {
					//calculate new z
					nr = zr * zr - zi * zi + cr;
					ni = zr * zi * 2.0 + ci;
					//update z for next interation
					zr = nr;
					zi = ni;
					++n;
				}
				_pfractal[utils::XY(x, y)] = n;
				yPos += ys;
			}
			xPos += xs;
		}
		workerComplete++;
	}
}