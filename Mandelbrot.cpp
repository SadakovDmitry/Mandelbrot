#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

int main()
{
    int wide  = 600;
    int hight = 600;
    float x_center = wide / 2;
    float y_center = hight / 2;
    float R = 10000.f;
    float scale = 100.f;

    sf::RenderWindow window(sf::VideoMode(wide, hight), "Mandelbrot");
    sf::VertexArray pointmap(sf::Points, wide * hight);
    sf::Text text;
    sf::Font font;
    font.loadFromFile("Tranrbi.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(14);
    char* str = (char*) calloc(20, sizeof(char));
    sf::Clock clock;

    while(window.isOpen())
    {
        float delta_time = clock.restart().asSeconds();
        float fps = 1 / delta_time;
        sprintf(str, "fps: %f", fps);
        text.setString(str);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))  x_center += 10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))  y_center += 10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))  y_center -= 10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  x_center -= 10;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale    *= 1.2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash ))  scale    /= 1.2;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        for (int point_number_y = 0; point_number_y < wide; point_number_y++)
        {
            for (int point_number_x = 0; point_number_x < hight; point_number_x++)
            {
                float x_0 = (float) (point_number_x - x_center) / scale;
                float y_0 = (float) (point_number_y - y_center) / scale;
                float x_n = x_0;
                float y_n = y_0;

                int n = 0;
                for ( ; n < 256; n++)
                {
                    if(R < (x_n * x_n) + (y_n * y_n))
                        break;

                    float temp = x_n;
                    x_n = (x_n * x_n) - (y_n * y_n) + x_0;
                    y_n = 2 * (temp * y_n) + y_0;
                }

                int arr_index = point_number_x + wide * point_number_y;
                pointmap[arr_index].position = sf::Vector2f(point_number_x, point_number_y);
                if(n < 256)
                {
                    pointmap[arr_index].color = sf::Color(n, n % 4 * 64, 255 - n);
                }
                else
                {
                    pointmap[arr_index].color = sf::Color::Black;
                }

            }
        }

        window.clear(sf::Color::Black);
        window.draw(pointmap);
        window.draw(text);
        window.display();
    }
    printf("end");
}
