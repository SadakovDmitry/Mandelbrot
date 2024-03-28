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

int main()
{
    int wide  = 400;
    int hight = 400;
    //__m128 x_center = _mm_set_ps1 (wide  / 2);
    //__m128 y_center = _mm_set_ps1 (hight / 2);
    float x_center = wide / 2;
    float y_center = hight / 2;
    __m128 R = _mm_set_ps1 (10000.f);
    __m128 scale = _mm_set_ps1 (100.f);
    __m128 _3210 = _mm_set_ps(3.f, 2.f, 1.f, 0.f);
    //float scale = 100.f;

    sf::RenderWindow window(sf::VideoMode(wide, hight), "Mondelbrot");
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
            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale    *= 1.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale  = _mm_mul_ps(scale, _mm_set_ps1(1.5f));
            //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash ))  scale    /= 1.5;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal))  scale  = _mm_div_ps(scale, _mm_set_ps1(1.5f));
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
                float y_0 = (float) (point_number_y - y_center);
                __m128 arr_x_0 = _mm_div_ps( _mm_add_ps( _mm_set_ps1(x_0), _3210 ), scale );
                __m128 arr_y_0 = _mm_div_ps( _mm_set_ps1(y_0), scale );
                __m128 x_n = arr_x_0;
                __m128 y_n = arr_y_0;
                fprintf(stderr, "x_n: <%lld, %lld, %lld, %lld>\n", N[0], N[1], N[2], N[3]);


                __m128i N = _mm_set1_epi32(0);
                for (int n = 0 ; n < 256; n++)
                {

                    __m128 res_cmp = _mm_cmple_ps(R, _mm_add_ps( _mm_mul_ps(x_n, x_n), _mm_mul_ps(y_n, y_n)));
                    int mask = _mm_movemask_ps(res_cmp);
                    fprintf(stderr, "1)<%lld, %lld, %lld, %lld>\n", N[0], N[1], N[2], N[3]);
                    //getchar();

                    if(!mask) break;

                    fprintf(stderr, "LOL\n");
                    N = _mm_add_epi32(N, _mm_cvtps_epi32(_mm_and_ps(res_cmp, _mm_set_ps1(1.f))));
                    fprintf(stderr, "<%lld, %lld, %d, %lld>\n", N[0], N[1], (int) N[2], N[3]);
                    getchar();
                    //N = _mm_sub_epi32(N, _mm_castps_si128(res_cmp));
                    __m128 temp = x_n;
                    __m128 x_n = _mm_add_ps( _mm_sub_ps( _mm_mul_ps( x_n, x_n ) , _mm_mul_ps( y_n, y_n )) , arr_x_0);
                    __m128 y_n = _mm_add_ps( _mm_add_ps( _mm_mul_ps( temp, y_n ), _mm_mul_ps( temp, y_n )), arr_y_0);

                }
                for (int i = 0; i < 4; i++)
                {
                    int arr_index = point_number_x + wide * point_number_y + i;
                    pointmap[arr_index].position = sf::Vector2f(point_number_x + i, point_number_y);
                    if((int) N[i] < 256)
                    //if((mask >> i * 8) & 1 == 1)
                    {
                        pointmap[arr_index].color = sf::Color((int) N[i], (int) N[i] % 2 * 64, 255 - (int) N[i]);
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
