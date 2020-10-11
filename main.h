#pragma once
#include <SFML/Graphics.hpp>

#define WIDTH 1024
#define HEIGHT 512
#define WxH WIDTH*HEIGHT
#define NUM_THREAD 4

class utils {
public:
	static constexpr int XY(int x, int y) {
		return x + y * WIDTH;
	}

	static constexpr double scaleab(
		double a1, double a2,
		double b1, double b2) {
		return (a1 - a2) / (b1 - b2);
	}

	//h [0,360], s v [0,1], output rgb [0,1]
	static sf::Color hsv2rgb(float h, float s, float v) {
		h = std::min(h, 360.f);
		s = std::min(s, 1.f);
		v = std::min(v, 1.f);

		return sf::Color(
			fn_hsv2rgb(5, h, s, v) * 255,
			fn_hsv2rgb(3, h, s, v) * 255,
			fn_hsv2rgb(1, h, s, v) * 255);
	}

	//https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
	static float fn_hsv2rgb(float n, float h, float s, float v) {
		float k = fmod(n + (h / 60), 6);
		return v - v * s * std::max(0.f,
			std::min(std::min(k, 4 - k), 1.0f));
	}

	static constexpr unsigned int maxFps = 30;
};