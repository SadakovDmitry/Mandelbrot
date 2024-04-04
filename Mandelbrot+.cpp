#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

const int WIDTH    = 600;
const int HEIGHT   = 600;
const float RADIUS = 10000.f;

void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text, sf::VertexArray &pointmap);
void Set_Mandelbrot_Pixel_Arr( sf::VertexArray &pointmap, float x_center, float y_center, float scale, float shift_x, float shift_y);
void Print_FPS(sf::Text &text, float delta_time, char* str, FILE* file);
void Check_Keyboard(sf::RenderWindow &window , float* shift_x, float* shift_y, float* scale);
void Calculate_iters(float x_0, float y_0, int* iter, float scale, float shift_x, float shift_y);
char* Set_Text_Format(sf::Text &text, sf::Font &font);

int main()
{
    const float x_center = WIDTH / 2;
    const float y_center = HEIGHT / 2;
    float shift_x  = 0;
    float shift_y  = 0;
    float scale = 100.f;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    sf::VertexArray pointmap(sf::Points, WIDTH * HEIGHT);
    sf::Text text;
    sf::Font font;
    sf::Clock clock;
    char* str = Set_Text_Format(text, font);
    FILE* file_out = fopen("tests.txt", "w");

    while(window.isOpen())
    {
        Check_Keyboard(window , &shift_x, &shift_y, &scale);

        float time_start = clock.restart().asSeconds();
        Set_Mandelbrot_Pixel_Arr(pointmap, x_center, y_center, scale, shift_x, shift_y);
        float delta_time = clock.restart().asSeconds();
        Print_FPS(text, delta_time, str, file_out);

        Show_Picture_in_Window(window, text, pointmap);
    }
    fclose(file_out);
    printf("end");
}

char* Set_Text_Format(sf::Text &text, sf::Font &font)
{
    font.loadFromFile("Tranrbi.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(20);
    char* str = (char*) calloc(15, sizeof(char));

    return str;
}

void Calculate_iters(float x_0, float y_0, int* iter, float scale, float shift_x, float shift_y)
{
    float arr_x_0[4] = {x_0 / scale, (x_0 + 1) / scale , (x_0 + 2) / scale, (x_0 + 3) / scale};
    float arr_y_0[4] = {y_0        , y_0              , y_0              , y_0              };
    float x_n[4] = {};
    float y_n[4] = {};
    memcpy(x_n, arr_x_0, 4 * sizeof(float));
    memcpy(y_n, arr_y_0, 4 * sizeof(float));

    for (int n = 0 ; n < 256; n++)
    {
        float x2[4] = {};
        for (int i = 0; i < 4; i++) x2[i] = x_n[i] * x_n[i];
        float y2[4] = {};
        for (int i = 0; i < 4; i++) y2[i] = y_n[i] * y_n[i];

        int flag = 0;
        for (int i = 0; i < 4; i++)
        {
            if(RADIUS >= x2[i] + y2[i])
                iter[i]++;
                flag++;
        }

        if(!flag) break;

        float temp[4] = {x_n[0], x_n[1], x_n[2], x_n[3]};
        for (int i = 0; i < 4; i++) x_n[i] = x2[i] - y2[i] + arr_x_0[i];
        for (int i = 0; i < 4; i++) y_n[i] = 2 * (temp[i] * y_n[i]) + arr_y_0[i];

    }
}

void Check_Keyboard(sf::RenderWindow &window , float* shift_x, float* shift_y, float* scale)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))  *shift_x += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))  *shift_y += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))  *shift_y -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  *shift_x -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
        {
            *scale   *= 1.2;
            *shift_x *= 1.2;
            *shift_y *= 1.2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash ))
        {
            *scale   /= 1.2;
            *shift_x /= 1.2;
            *shift_y /= 1.2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        if (event.type == sf::Event::Closed)
        {
            window.close();
            break;
        }
    }
}

void Set_Mandelbrot_Pixel_Arr(sf::VertexArray &pointmap, float x_center, float y_center, float scale, float shift_x, float shift_y)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x += 4)
        {
            float x_0 = (float) (point_number_x - x_center - shift_x);
            float y_0 = (float) (point_number_y - y_center - shift_y) / scale;

            int iter[4] = {0, 0, 0, 0};
            Calculate_iters(x_0, y_0, iter, scale, shift_x, shift_y);

            for (int i = 0; i < 4; i++)
            {
                int pixel_index = point_number_x + WIDTH * point_number_y + i;
                pointmap[pixel_index].position  = sf::Vector2f(point_number_x + i, point_number_y);

                if(iter[i] < 256)
                    pointmap[pixel_index].color = sf::Color(iter[i], iter[i] % 8 * 32, 255 - iter[i]);
                else
                    pointmap[pixel_index].color = sf::Color::Black;
            }

        }
    }
}

void Print_FPS(sf::Text &text, float delta_time, char* str, FILE* file)
{
    //float delta_time = clock.restart().asSeconds();
    float fps = 1 / delta_time;
    sprintf(str, "fps: %f", fps);
    fprintf(file, "%f\n", fps);
    text.setString(str);
}

void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text, sf::VertexArray &pointmap)
{
    window.clear(sf::Color::Black);
    window.draw(pointmap);
    window.draw(text);
    window.display();
}
