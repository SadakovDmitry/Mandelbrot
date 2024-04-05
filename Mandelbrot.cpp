#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

const int WIDTH  = 800;
const int HEIGHT = 600;
const float R   = 10000.f;

void Show_picture_in_window(sf::RenderWindow &window, sf::Text &text, unsigned int* pix_arr, sf::Sprite &sprite, sf::Texture &texture);
void Set_mandelbrot_pixel_arr( unsigned int* pix_arr, float x_center, float y_center, float scale);
void Print_fps(sf::Text &text, float delta_time, char* str, FILE* file);
void Check_keyboard(sf::RenderWindow &window , float* x_center, float* y_center, float* scale);
void Calculate_iters(float x_0, float y_0, int* iter, float scale);
char* Set_text_format(sf::Text &text, sf::Font &font);

int main()
{
    float x_center = WIDTH / 2;
    float y_center = HEIGHT / 2;
    float scale = 120.f;

    #ifndef TESTING_MODE
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    #endif
    sf::Text    text;
    sf::Font    font;
    sf::Clock   clock;
    sf::Sprite  sprite;
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sprite.setTexture(texture);

    char* str = Set_text_format(text, font);
    unsigned int* pix_arr = (unsigned int*) calloc(WIDTH * HEIGHT, sizeof(unsigned int));
    FILE* file_out = fopen("tests.txt", "w");

    #ifndef TESTING_MODE
    while(window.isOpen())
    #else
    for (int k = 0; k < 100; k++)
    #endif
    {
        #ifndef TESTING_MODE
        Check_keyboard(window , &x_center, &y_center, &scale);
        #endif

        float time_start = clock.restart().asSeconds();
        Set_mandelbrot_pixel_arr(pix_arr, x_center, y_center, scale);
        float delta_time = clock.restart().asSeconds();
        Print_fps(text, delta_time, str, file_out);

        #ifndef TESTING_MODE
        Show_picture_in_window(window, text, pix_arr, sprite, texture);
        #endif
    }
    fclose(file_out);
}

char* Set_text_format(sf::Text &text, sf::Font &font)
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

void Check_keyboard(sf::RenderWindow &window , float* x_center, float* y_center, float* scale)
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

void Set_mandelbrot_pixel_arr(unsigned int* pix_arr, float x_center, float y_center, float scale)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x++)
        {
            float x_0 = (float) (point_number_x - x_center) / scale;
            float y_0 = (float) (point_number_y - y_center) / scale;

            int iter = 0;
            Calculate_iters(x_0, y_0, &iter, scale);

            //#ifndef TESTING_MODE
            int pixel_index = point_number_x + WIDTH * point_number_y;
            if(iter < 256)
                    pix_arr[pixel_index] = iter | ((iter % 8 * 32) << 8) | ((255 - iter) << 16) | (255 << 24);
                else
                    pix_arr[pixel_index] = (255 << 24);
            //#endif
        }
    }
}

void Print_fps(sf::Text &text, float delta_time, char* str, FILE* file)
{
    float fps = 1 / delta_time;
    #ifndef TESTING_MODE
    sprintf(str, "fps: %f", fps);
    text.setString(str);
    #else
    fprintf(file, "%f\n", fps);
    #endif
}

void Show_picture_in_window(sf::RenderWindow &window, sf::Text &text, unsigned int* pix_arr, sf::Sprite &sprite, sf::Texture &texture)
{
    window.clear(sf::Color::Black);
    texture.update((uint8_t*) pix_arr);
    window.draw(sprite);
    window.draw(text);
    window.display();
}
