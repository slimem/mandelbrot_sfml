#include "Mandelbrot.h"
#include <complex>
#include <cstdlib>
#include <iostream>
#include <iomanip>

using namespace std;

Mandelbrot::Mandelbrot(size_t width, size_t height):
	_width(width), _height(height) {
	//for microsoft only, for others, use std::aligned_alloc(alignment, size)
	_pfractal = (int*)_aligned_malloc(_width * _height * sizeof(int), 64);
	_pixMap.resize(_width * _height);
	//place all pixels and color them
	for (int i = 0; i < _width * _height; ++i) {
		_pixMap[i].position = sf::Vector2f(i % width, (i / _width) % _height);
		_pixMap[i].color = sf::Color(128, 128, 128);
	}

	_pixtl = {0, 0};
	_pixbr = {(double)_width, (double)_height};
	_fractl = {-2.0, -1.0 };
	_fracbr = { 2.0, 1.0 };

	_iters = 1024;
	initThreads();
}

Mandelbrot::~Mandelbrot() {
	// Stop worker threads
	for (int i = 0; i < NUM_THREAD; ++i)
	{
		_workers[i].setAlive(false);		 // Allow thread exit by stopping while loop
		_workers[i]._cvStart.notify_one();  // Fake starting gun
	}

	// Clean up worker threads
	for (int i = 0; i < NUM_THREAD; ++i)
		_workers[i]._thread.join();
		
	// Clean up memory
	_aligned_free(_pfractal);
}

void
Mandelbrot::step() {
	//createFractal();
	if (_reportFracCoordinates) {
		cout << "mandelbrot.startAt({" << setprecision(20) << _fractl.x << ", " << setprecision(20) << _fractl.y << "},";
		cout << "{" << setprecision(20) << _fracbr.x << "," << setprecision(20) << _fracbr.y << "});\n";
	}
	startThreads();
	render();
}

void
Mandelbrot::render() {
	for (int x = _pixtl.x; x < _pixbr.x; ++x) {
		for (int y = _pixtl.y; y < _pixbr.y; ++y) {
			float n = (float)_pfractal[utils::XY(x, y)];

			//different ways to color the fractal
#if 0
			// 1 - using sin
			float a = 0.1f;
			_pixMap[utils::XY(x, y)].color = sf::Color(
				(0.5f * sin(a * n) + 0.5f)*255.f,
				(0.5f * sin(a * n + 2.094f) + 0.5f)*255.f,
				(0.5f * sin(a * n + 4.188f) + 0.5f)*255.f);
#endif
#if 0
			// 2 - using HSV with changing hue/value and fixed saturaion
			float h = (n / _iters) * 360.f;
			float s = 1.f;
			float v = n <_iters ? 1.0f : 0.0f;
			_pixMap[XY(x, y)].color = hsv2rgb(n, s, v);
#endif
			// 3 - fixed hue (color) and changing saturation and value
			_pixMap[utils::XY(x, y)].color = utils::hsv2rgb(
				112, //green
				n < _iters ? (n / _iters) *2 : 0.0f,
				n < _iters ? (n/_iters) : 0.0f
			);
		}
	}
}

void
Mandelbrot::createFractal() {
	double xs = utils::scaleab(_fracbr.x, _fractl.x, _pixbr.x, _pixtl.x);
	double ys = utils::scaleab(_fracbr.y, _fractl.y, _pixbr.y, _pixtl.y);

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
			while (n < _iters && (zi*zi + zr*zr) < 4) {
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
}

void
Mandelbrot::initThreads() {
	for (int i = 0; i < NUM_THREAD; ++i) {
		_workers[i].setAlive(true);
		_workers[i]._thread = std::thread(&Worker::createFractal, &_workers[i]);
	}
}

void
Mandelbrot::startThreads() {
	int vSectionWidth = (_pixbr.x - _pixtl.x) / NUM_THREAD;
	int hSectionWidth = (_pixbr.y - _pixtl.y) / NUM_THREAD;
	double fractalWidth = (_fracbr.x - _fractl.x) / double(NUM_THREAD);
	Worker::workerComplete = 0;

	//TODO: Support checkerboard rendering. For now, the screen is split into thread columns
	// and it would be nice to have thread grids
	if (_isCheckerBoard) {
		//Thread grid implementation goes here
	}
	else {
		for (size_t i = 0; i < NUM_THREAD; ++i) {
			_workers[i].init(
				sf::Vector2<double>(_pixtl.x + vSectionWidth * i, _pixtl.y),
				sf::Vector2<double>(_pixtl.x + vSectionWidth * (i + 1), _pixbr.y),
				sf::Vector2<double>(_fractl.x + fractalWidth * double(i), _fractl.y),
				sf::Vector2<double>(_fractl.x + fractalWidth * double(i + 1), _fracbr.y),
				_pfractal,
				_iters);
		}
	}

	while (Worker::workerComplete < NUM_THREAD) // Wait for all our workers to complete
	{
	}
}