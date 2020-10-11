#pragma once

#include "Worker.h"
#include <iostream>

using namespace std;

class Mandelbrot {
public:
	Mandelbrot() = delete;
	Mandelbrot(size_t width, size_t height);
	~Mandelbrot();

	void initThreads();
	void startThreads();
	
	void createFractal();
	void render();

	void step();

	void zoom(float amount = 1.05f) {
		centerAtf(_lastCenter.x, _lastCenter.y);
		_fractl.x /= amount;
		_fractl.y /= amount;
		_fracbr.x /= amount;
		_fracbr.y /= amount;

	}

	void startAt(const sf::Vector2<double>& tl, const sf::Vector2<double>& br) {
		_fractl = tl;
		_fracbr = br;
	}
	
	void pan(float x = 0.f, float y = 0.f) {
		_fractl.x += x;
		_fractl.y += y;
		_fracbr.x += x;
		_fracbr.y += y;
	}

	void centerAtf(double x, double y) {
		//keep same distance
		double dx = (_fracbr.x - _fractl.x) / 2.f;
		double dy = (_fracbr.y - _fractl.y) / 2.f;

		_fractl.x = x - dx;
		_fracbr.x = x + dx;

		_fractl.y = y - dy;
		_fracbr.y = y + dy;
		_lastCenter = {x,y};
	}

	void centerAtMouse(const sf::Vector2i& pos) {
		//convert to fractal space first
		double xs = utils::scaleab(_fracbr.x, _fractl.x, _pixbr.x, _pixtl.x);
		double ys = utils::scaleab(_fracbr.y, _fractl.y, _pixbr.y, _pixtl.y);
		centerAtf(xs,0);
	}

	const sf::Vector2<double> getMouseFractalCoordinates(const sf::Vector2i& pos) {
		double xs = utils::scaleab(_fracbr.x, _fractl.x, _pixbr.x, _pixtl.x);
		double ys = utils::scaleab(_fracbr.y, _fractl.y, _pixbr.y, _pixtl.y);
		return sf::Vector2<double> (
			pos.x * xs + _fractl.x,
			pos.y * ys + _fractl.y
		);
	}

	const sf::VertexArray& getPixels() const {
		return _pixMap;
	}

	void setReportFracCoordinates() {
		_reportFracCoordinates = true;
	}
	

private:
	size_t				_width;
	size_t				_height;
	int*				_pfractal;
	int					_iters;
	sf::VertexArray		_pixMap;
	bool				_reportFracCoordinates = false;
	bool				_isCheckerBoard = false;

	sf::Vector2<double>	_pixtl;
	sf::Vector2<double>	_pixbr;
	sf::Vector2<double>	_fractl;
	sf::Vector2<double>	_fracbr;

	sf::Texture			_texture;
	sf::Sprite			_sprite;

	sf::Vector2<double> _lastCenter;

	Worker				_workers[NUM_THREAD];

};