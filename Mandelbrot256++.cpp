#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>
#include <immintrin.h>

union Iterations
{
    __m256i reg;
    int arr[8];
};

const int    WIDTH    = 800;
const int    HEIGHT   = 600;
const __m256 RADIUS  = _mm256_set1_ps (10000.f);
const __m256 _76543210 = _mm256_set_ps (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

void Set_mandelbrot_pixel_arr(union Iterations* iter, unsigned int* pix_arr, float x_center, float y_center, __m256 scale);
void Calculate_iters(float x_0, float y_0, union Iterations* iter, __m256 scale);
void Check_keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m256* scale);
char* Set_fext_format(sf::Text &text, sf::Font &font);
void Print_fps(sf::Text &text, float delta_time, char* str, FILE* file);
void Show_picture_in_window(sf::RenderWindow &window, sf::Text &text, unsigned int* pix_arr, sf::Sprite &sprite, sf::Texture &texture);

int main()
{
    union Iterations iter = {};
    float x_center = WIDTH  / 2;
    float y_center = HEIGHT / 2;
    __m256 scale   = _mm256_set1_ps (120.f);

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

    char* str = Set_fext_format(text, font);
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
        Set_mandelbrot_pixel_arr(&iter, pix_arr, x_center, y_center, scale);
        float delta_time = clock.restart().asSeconds();
        Print_fps(text, delta_time, str, file_out);

        #ifndef TESTING_MODE
        Show_picture_in_window(window, text, pix_arr, sprite, texture);
        #endif
    }
    fclose(file_out);
}



void Calculate_iters(float x_0, float y_0, union Iterations* iter, __m256 scale)
{
    __m256 arr_x_0 = _mm256_div_ps( _mm256_add_ps( _mm256_set1_ps(x_0), _76543210 ), scale );
    __m256 arr_y_0 = _mm256_div_ps( _mm256_set1_ps(y_0), scale );
    __m256 x_n     = arr_x_0;
    __m256 y_n     = arr_y_0;

    iter -> reg = _mm256_set1_epi32(0);
    for (int n = 0 ; n < 256; n++)
    {
        __m256 x2 =  _mm256_mul_ps( x_n, x_n );
        __m256 y2 =  _mm256_mul_ps( y_n, y_n );
        __m256 xy =  _mm256_mul_ps( x_n, y_n );

        __m256 res_cmp = _mm256_cmp_ps(_mm256_add_ps( x2, y2 ), RADIUS, _CMP_LT_OS );
        int mask       = _mm256_movemask_ps(res_cmp);

        if(!mask) break;

        iter -> reg = _mm256_add_epi32( iter -> reg, _mm256_cvtps_epi32( _mm256_and_ps( res_cmp, _mm256_set1_ps( 1.f ))));
        __m256 temp = x_n;
        x_n = _mm256_add_ps( _mm256_sub_ps( x2, y2 ), arr_x_0 );
        y_n = _mm256_add_ps( _mm256_add_ps( xy, xy ), arr_y_0 );

    }
}

void Check_keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m256* scale)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *x_center += 20;
            else
                *x_center += 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *y_center += 20;
            else
                *y_center += 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *y_center -= 20;
            else
                *y_center -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
                *x_center -= 20;
            else
                *x_center -= 10;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
            {
                *scale     = _mm256_mul_ps(*scale, _mm256_set1_ps(2.2f));
                *x_center = (*x_center - WIDTH / 2) * 2.2 + WIDTH / 2;
                *y_center = (*y_center - HEIGHT / 2) * 2.2 + HEIGHT / 2;
            }
            else
            {
                *scale     = _mm256_mul_ps(*scale, _mm256_set1_ps(1.2f));
                *x_center = (*x_center - WIDTH / 2) * 1.2 + WIDTH / 2;
                *y_center = (*y_center - HEIGHT / 2) * 1.2 + HEIGHT / 2;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift ))
            {
                *scale     = _mm256_div_ps(*scale, _mm256_set1_ps(2.2f));
                *x_center = (*x_center - WIDTH / 2) / 2.2 + WIDTH / 2;
                *y_center = (*y_center - HEIGHT / 2) / 2.2 + HEIGHT / 2;
            }
            else
            {
                *scale     = _mm256_div_ps(*scale, _mm256_set1_ps(1.2f));
                *x_center = (*x_center - WIDTH / 2) / 1.2 + WIDTH / 2;
                *y_center = (*y_center - HEIGHT / 2) / 1.2 + HEIGHT / 2;
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

void Set_mandelbrot_pixel_arr(union Iterations* iter, unsigned int* pix_arr, float x_center, float y_center, __m256 scale)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x += 8)
        {
            float x_0 = (float) (point_number_x - x_center);
            float y_0 = (float) (point_number_y - y_center);

            Calculate_iters(x_0, y_0, iter, scale);

            //#ifndef TESTING_MODE
            for (int i = 0; i < 8; i++)
            {
                int pixel_index = point_number_x + WIDTH * point_number_y + i;

                if(iter -> arr[i] < 256)
                    pix_arr[pixel_index] = iter -> arr[i] | ((iter -> arr[i] % 8 * 32) << 8) | ((255 - iter -> arr[i]) << 16) | (255 << 24);
                else
                    pix_arr[pixel_index] = (255 << 24);
            }
            //#endif
        }
    }
}

char* Set_fext_format(sf::Text &text, sf::Font &font)
{
    font.loadFromFile("Tranrbi.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(20);
    char* str = (char*) calloc(15, sizeof(char));

    return str;
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
