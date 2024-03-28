#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale    *= 1.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash ))  scale    /= 1.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        for (int point_number_y = 0; point_number_y < wide; point_number_y++)
        {
            for (int point_number_x = 0; point_number_x < hight; point_number_x += 4)
            {
                float x_0 = (float) (point_number_x - x_center);
                float y_0 = (float) (point_number_y - y_center) / scale;
                float arr_x_0[4] = {x_0 / scale, (x_0 + 1) / scale, (x_0 + 2) / scale, (x_0 + 3) / scale};
                float arr_y_0[4] = {y_0        , y_0              , y_0              , y_0              };
                float x_n[4] = {};
                float y_n[4] = {};
                memcpy(x_n, arr_x_0, 4 * sizeof(float));
                memcpy(y_n, arr_y_0, 4 * sizeof(float));

                int N[4] = {0, 0, 0, 0};
                for (int n = 0 ; n < 256; n++)
                {
                    int flag = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        if(R >= (x_n[i] * x_n[i]) + (y_n[i] * y_n[i]))
                            N[i]++;
                            flag++;
                    }

                    if(!flag) break;

                    float temp[4] = {x_n[0], x_n[1], x_n[2], x_n[3]};
                    for (int i = 0; i < 4; i++) x_n[i] = (x_n[i] * x_n[i]) - (y_n[i] * y_n[i]) + arr_x_0[i];
                    for (int i = 0; i < 4; i++) y_n[i] = 2 * (temp[i] * y_n[i]) + arr_y_0[i];

                }
                for (int i = 0; i < 4; i++)
                {
                    int arr_index = point_number_x + wide * point_number_y + i;
                    pointmap[arr_index].position = sf::Vector2f(point_number_x + i, point_number_y);
                    if(N[i] < 256)
                    {
                        pointmap[arr_index].color = sf::Color(N[i], N[i] % 4 * 64, 255 - N[i]);
                    }
                    else
                    {
                        pointmap[arr_index].color = sf::Color::Black;
                    }
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
