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
    __m128i reg;
    int arr[4];
};

const int   wide     = 600;
const int   hight    = 600;
const __m128 R       = _mm_set1_ps (10000.f);
const __m128 _3210   = _mm_set_ps  (3.f, 2.f, 1.f, 0.f);

void Set_Pixel_Arr(union Iterations* iter, sf::VertexArray &pointmap, float x_center, float y_center, __m128 scale);
void Calculate_iters(float x_0, float y_0, union Iterations* iter, __m128 scale);
void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m128* scale);
char* Set_Text_Format(sf::Text &text, sf::Font &font);
void Print_FPS(sf::Text &text, sf::Clock &clock, char* str);
void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text ,sf::VertexArray &pointmap);

int main()
{
    union Iterations iter = {};
    float x_center = wide  / 2;
    float y_center = hight / 2;
    __m128 scale   = _mm_set1_ps (100.f);

    sf::RenderWindow window(sf::VideoMode(wide, hight), "Mandelbrot");
    sf::VertexArray pointmap(sf::Points, wide * hight);
    sf::Text text;
    sf::Font font;
    sf::Clock clock;
    char* str = Set_Text_Format(text, font);

    while(window.isOpen())
    {
        Print_FPS(text, clock, str);

        Check_Keyboard(window , &x_center, &y_center, &scale);

        Set_Pixel_Arr(&iter, pointmap, x_center, y_center, scale);

        Show_Picture_in_Window(window, text, pointmap);
    }
    printf("end\n");
}

void Calculate_iters(float x_0, float y_0, union Iterations* iter, __m128 scale)
{
    __m128 arr_x_0 = _mm_div_ps( _mm_add_ps( _mm_set_ps1(x_0), _3210 ), scale );
    __m128 arr_y_0 = _mm_div_ps( _mm_set_ps1(y_0), scale );
    __m128 x_n     = arr_x_0;
    __m128 y_n     = arr_y_0;

    iter -> reg = _mm_set1_epi32(0);
    for (int n = 0 ; n < 256; n++)
    {
        __m128 res_cmp = _mm_cmple_ps(_mm_add_ps( _mm_mul_ps( x_n, x_n ), _mm_mul_ps( y_n, y_n )), R);
        int mask       = _mm_movemask_ps(res_cmp);

        if(!mask) break;

        iter -> reg = _mm_add_epi32( iter -> reg, _mm_cvtps_epi32( _mm_and_ps( res_cmp, _mm_set_ps1( 1.f ))));
        __m128 temp = x_n;
        x_n = _mm_add_ps( _mm_sub_ps( _mm_mul_ps( x_n, x_n ) , _mm_mul_ps( y_n, y_n )) , arr_x_0);
        y_n = _mm_add_ps( _mm_add_ps( _mm_mul_ps( temp, y_n ), _mm_mul_ps( temp, y_n )), arr_y_0);
    }
}

void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m128* scale)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left ))  *x_center += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up   ))  *y_center += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down ))  *y_center -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  *x_center -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  *scale     = _mm_mul_ps(*scale, _mm_set_ps1(1.2f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))   *scale     = _mm_div_ps(*scale, _mm_set_ps1(1.2f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        if (event.type == sf::Event::Closed)
        {
            window.close();
            break;
        }
    }
}

void Set_Pixel_Arr(union Iterations* iter, sf::VertexArray &pointmap, float x_center, float y_center, __m128 scale)
{
    for (int point_number_y = 0; point_number_y < wide; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < hight; point_number_x += 4)
        {
            float x_0 = (float) (point_number_x - x_center);
            float y_0 = (float) (point_number_y - y_center);

            Calculate_iters(x_0, y_0, iter, scale);

            for (int i = 0; i < 4; i++)
            {
                int pixel_index = point_number_x + wide * point_number_y + i;
                pointmap[pixel_index].position  = sf::Vector2f(point_number_x + i, point_number_y);

                if(iter -> arr[i] < 256)
                    pointmap[pixel_index].color = sf::Color(iter -> arr[i], iter -> arr[i] % 4 * 64, 255 - iter -> arr[i]);
                else
                    pointmap[pixel_index].color = sf::Color::Black;
            }

        }
    }
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

void Print_FPS(sf::Text &text, sf::Clock &clock, char* str)
{
    float delta_time = clock.restart().asSeconds();
    float fps = 1 / delta_time;
    sprintf(str, "fps: %f", fps);
    text.setString(str);
}

void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text, sf::VertexArray &pointmap)
{
    window.clear(sf::Color::Black);
    window.draw(pointmap);
    window.draw(text);
    window.display();
}
