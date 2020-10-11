
#include "main.h"
#include "Mandelbrot.h"

#include <sstream>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>


using namespace std;
int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH,HEIGHT), "Mandelbrot-sfml");
    window.setFramerateLimit(utils::maxFps);
    Mandelbrot mandelbrot(WIDTH, HEIGHT);
    sf::sleep(sf::seconds(1.f));

    // Interesting fractal coordinates
    mandelbrot.startAt({ -0.75056121854648716329, 0.098723310563246277627 }, { -0.74624045119470161502,0.10088369423913957912 });
    bool center = false;
    bool paused = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    paused = !paused;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    center = true;
                }
            }
        }

        window.clear();
        
        sf::Vector2i position = sf::Mouse::getPosition(window);
        const auto pos = mandelbrot.getMouseFractalCoordinates(position);

        mandelbrot.centerAtf(-0.74840083487059438916, 0.099803502401192928373);
         
        if (center) {
            mandelbrot.centerAtf(pos.x, pos.y);
            cout << "mandelbrot.centerAtf(" << setprecision(20) << pos.x << "," << setprecision(20) << pos.y << ");" << endl;
            center = false;
        }
        if (!paused)
        mandelbrot.zoom();
        mandelbrot.centerAtf(-0.74840083487059438916, 0.099803502401192928373);

        sf::Font font;
        font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
 

        auto tp1 = std::chrono::high_resolution_clock::now();
        if (!paused) {
            mandelbrot.step();
        }
        auto tp2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = tp2 - tp1;
        window.draw(mandelbrot.getPixels());
        sf::Text timeText;
        timeText.setFont(font);
        timeText.setFillColor(sf::Color::Red);
        timeText.setPosition(10, 10);
        timeText.setCharacterSize(12);
        {
            ostringstream os;
            os << "Frame time : " << elapsedTime.count() << " seconds\n";
            os << "Mouse screen coordinates : " << "( " << position.x << "," << position.y << ")\n";
            os << "Mouse fractal coordinates: " << "( " << setprecision(5) << pos.x << "," << setprecision(5) << pos.y << ")\n";
            os << "Press space to pause\n";
            os << "Press enter to copy coordinates\n";
            timeText.setString(os.str());
        }
        window.draw(timeText);

#if 0
        //If you want to display stuff at mouse coordinates uncomment this section

        sf::Text mousePos; //fractal and window coordinates
        mousePos.setFont(font);
        mousePos.setFillColor(sf::Color::Red);
        mousePos.setPosition(position.x, position.y - 50);
        mousePos.setCharacterSize(16);
        {
            ostringstream os;
            os << "( " << position.x << "," << position.y << ")\n";
            os << "( " << setprecision(5) << pos.x << "," << setprecision(5) <<pos.y << ")\n";
            //cout << "( " << pos.x << "," << pos.y << ")\n";
            mousePos.setString(os.str());
        }
        window.draw(mousePos);
#endif
        window.display();
    }

    return 0;
}