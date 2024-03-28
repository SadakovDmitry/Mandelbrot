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

int main()
{
    union Iterations iter = {};
    int   wide     = 600;
    int   hight    = 600;
    float x_center = wide  / 2;
    float y_center = hight / 2;
    __m256 R       = _mm256_set1_ps (10000.f);
    __m256 scale   = _mm256_set1_ps (100.f);
    __m256 _76543210   = _mm256_set_ps  (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

    sf::RenderWindow window(sf::VideoMode(wide, hight), "Mandelbrot");
    sf::VertexArray pointmap(sf::Points, wide * hight);
    sf::Text text;
    sf::Font font;
    sf::Clock clock;
    font.loadFromFile("Tranrbi.ttf");
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setCharacterSize(20);
    char* str = (char*) calloc(15, sizeof(char));

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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale     = _mm256_mul_ps(scale, _mm256_set1_ps(1.2f));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))   scale     = _mm256_div_ps(scale, _mm256_set1_ps(1.2f));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        for (int point_number_y = 0; point_number_y < wide; point_number_y++)
        {
            for (int point_number_x = 0; point_number_x < hight; point_number_x += 8)
            {
                float x_0 = (float) (point_number_x - x_center);
                float y_0 = (float) (point_number_y - y_center);
                __m256 arr_x_0 = _mm256_div_ps( _mm256_add_ps( _mm256_set1_ps(x_0), _76543210 ), scale );
                __m256 arr_y_0 = _mm256_div_ps( _mm256_set1_ps(y_0), scale );
                __m256 x_n     = arr_x_0;
                __m256 y_n     = arr_y_0;

                iter.reg = _mm256_set1_epi32(0);
                for (int n = 0 ; n < 256; n++)
                {
                    __m256 res_cmp = _mm256_cmp_ps(_mm256_add_ps( _mm256_mul_ps( x_n, x_n ), _mm256_mul_ps( y_n, y_n )), R, _CMP_LT_OS);
                    int mask       = _mm256_movemask_ps(res_cmp);

                    if(!mask) break;

                    iter.reg = _mm256_add_epi32( iter.reg, _mm256_cvtps_epi32( _mm256_and_ps( res_cmp, _mm256_set1_ps( 1.f ))));
                    __m256 temp = x_n;
                    x_n = _mm256_add_ps( _mm256_sub_ps( _mm256_mul_ps( x_n, x_n ) , _mm256_mul_ps( y_n, y_n )) , arr_x_0);
                    y_n = _mm256_add_ps( _mm256_add_ps( _mm256_mul_ps( temp, y_n ), _mm256_mul_ps( temp, y_n )), arr_y_0);

                }
                for (int i = 0; i < 8; i++)
                {
                    int pixel_index = point_number_x + wide * point_number_y + i;
                    pointmap[pixel_index].position = sf::Vector2f(point_number_x + i, point_number_y);
                    if((iter.arr)[i] < 256)
                    {
                        pointmap[pixel_index].color = sf::Color((iter.arr)[i], (iter.arr)[i] % 8 * 32, 255 - (iter.arr)[i]);
                    }
                    else
                    {
                        pointmap[pixel_index].color = sf::Color::Black;
                    }
                }

            }
        }

        window.clear(sf::Color::Black);
        window.draw(pointmap);
        window.draw(text);
        window.display();
    }
    printf("end\n");
}
