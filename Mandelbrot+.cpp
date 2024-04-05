#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>

const int WIDTH    = 800;
const int HEIGHT   = 600;
const float RADIUS = 10000.f;

void Show_picture_in_window(sf::RenderWindow &window, sf::Text &text, unsigned int* pix_arr, sf::Sprite &sprite, sf::Texture &texture);
void Set_mandelbrot_pixel_arr( unsigned int* pix_arr, float x_center, float y_center, float scale, float shift_x, float shift_y);
void Print_fps(sf::Text &text, float delta_time, char* str, FILE* file);
void Check_keyboard(sf::RenderWindow &window , float* shift_x, float* shift_y, float* scale);
void Calculate_iters(float x_0, float y_0, int* iter, float scale, float shift_x, float shift_y);
char* Set_text_format(sf::Text &text, sf::Font &font);

int main()
{
    const float x_center = WIDTH / 2;
    const float y_center = HEIGHT / 2;
    float shift_x  = 0;
    float shift_y  = 0;
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
        Check_keyboard(window , &shift_x, &shift_y, &scale);
        #endif

        float time_start = clock.restart().asSeconds();
        Set_mandelbrot_pixel_arr(pix_arr, x_center, y_center, scale, shift_x, shift_y);
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
            {
                iter[i]++;
                flag++;
            }
        }

        if(!flag) break;


        float temp[4] = {x_n[0], x_n[1], x_n[2], x_n[3]};
        for (int i = 0; i < 4; i++) x_n[i] = x2[i] - y2[i] + arr_x_0[i];
        for (int i = 0; i < 4; i++) y_n[i] = 2 * (temp[i] * y_n[i]) + arr_y_0[i];

    }
}

void Check_keyboard(sf::RenderWindow &window , float* shift_x, float* shift_y, float* scale)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *shift_x += 20;
            else
                *shift_x += 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *shift_y += 20;
            else
                *shift_y += 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *shift_y -= 20;
            else
                *shift_y -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *shift_x -= 20;
            else
                *shift_x -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
            {
                *scale   *= 2.4;
                *shift_x *= 2.4;
                *shift_y *= 2.4;
            }
            else
            {
                *scale   *= 1.2;
                *shift_x *= 1.2;
                *shift_y *= 1.2;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
            {
                *scale   /= 2.4;
                *shift_x /= 2.4;
                *shift_y /= 2.4;
            }
            else
            {
                *scale   /= 1.2;
                *shift_x /= 1.2;
                *shift_y /= 1.2;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        if (event.type == sf::Event::Closed)
        {
            window.close();
            break;
        }
    }
}

void Set_mandelbrot_pixel_arr(unsigned int* pix_arr, float x_center, float y_center, float scale, float shift_x, float shift_y)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x += 4)
        {
            float x_0 = (float) (point_number_x - x_center - shift_x);
            float y_0 = (float) (point_number_y - y_center - shift_y) / scale;

            int iter[4] = {0, 0, 0, 0};
            Calculate_iters(x_0, y_0, iter, scale, shift_x, shift_y);

            #ifndef TESTING_MODE
            for (int i = 0; i < 4; i++)
            {
                int pixel_index = point_number_x + WIDTH * point_number_y + i;

                if(iter[i] < 256)
                    pix_arr[pixel_index] = iter[i] | ((iter[i] % 8 * 32) << 8) | ((255 - iter[i]) << 16) | (255 << 24);
                else
                    pix_arr[pixel_index] = (255 << 24);
            }
            #endif
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
