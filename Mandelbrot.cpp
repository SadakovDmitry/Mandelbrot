#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

const int WIDTH  = 800;
const int HEIGHT = 600;
const float R   = 10000.f;

void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text, sf::VertexArray &pointmap);
void Set_Mandelbrot_Pixel_Arr( sf::VertexArray &pointmap, float x_center, float y_center, float scale);
void Print_FPS(sf::Text &text, float delta_time, char* str, FILE* file);
void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, float* scale);
void Calculate_iters(float x_0, float y_0, int* iter, float scale);
char* Set_Text_Format(sf::Text &text, sf::Font &font);

int main()
{
    float x_center = WIDTH / 2;
    float y_center = HEIGHT / 2;
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
        Check_Keyboard(window , &x_center, &y_center, &scale);

        float time_start = clock.restart().asSeconds();
        Set_Mandelbrot_Pixel_Arr(pointmap, x_center, y_center, scale);
        float delta_time = clock.restart().asSeconds();
        Print_FPS(text, delta_time, str, file_out);

        Show_Picture_in_Window(window, text, pointmap);
    }
    fclose(file_out);
    printf("end\n");
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

void Calculate_iters(float x_0, float y_0, int* iter, float scale)
{
    float x_n = x_0;
    float y_n = y_0;
    for ( ; *iter < 256; (*iter)++)
    {
        float x2 = x_n * x_n;
        float y2 = y_n * y_n;

        if(R < (x2) + (y2))
            break;

        float temp = x_n;
        x_n = x2 - y2 + x_0;
        y_n = 2 * (temp * y_n) + y_0;
    }
}

void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, float* scale)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))  *x_center += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))  *y_center += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))  *y_center -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  *x_center -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
        {
            *scale   *= 1.2;
            *x_center = (*x_center - WIDTH / 2) * 1.2 + WIDTH / 2;
            *y_center = (*y_center - HEIGHT / 2) * 1.2 + HEIGHT / 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
        {
            *scale   /= 1.2;
            *x_center = (*x_center - WIDTH / 2) / 1.2 + WIDTH / 2;
            *y_center = (*y_center - HEIGHT / 2) / 1.2 + HEIGHT / 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        if (event.type == sf::Event::Closed)
        {
            window.close();
            break;
        }
    }
}

void Set_Mandelbrot_Pixel_Arr(sf::VertexArray &pointmap, float x_center, float y_center, float scale)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x++)
        {
            float x_0 = (float) (point_number_x - x_center) / scale;
            float y_0 = (float) (point_number_y - y_center) / scale;

            int iter = 0;
            Calculate_iters(x_0, y_0, &iter, scale);

            int arr_index = point_number_x + WIDTH * point_number_y;
            pointmap[arr_index].position = sf::Vector2f(point_number_x, point_number_y);
            if(iter < 256)
                pointmap[arr_index].color = sf::Color(iter, iter % 8 * 32, 255 - iter);
            else
                pointmap[arr_index].color = sf::Color::Black;
        }
    }
}

void Print_FPS(sf::Text &text, float delta_time, char* str, FILE* file)
{
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
