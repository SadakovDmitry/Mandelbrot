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

void Set_Mandelbrot_Pixel_Arr(union Iterations* iter, sf::VertexArray &pointmap, float x_center, float y_center, __m256 scale);
void Calculate_iters(float x_0, float y_0, union Iterations* iter, __m256 scale);
void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m256* scale);
char* Set_Text_Format(sf::Text &text, sf::Font &font);
void Print_FPS(sf::Text &text, float delta_time, char* str, FILE* file);
void Show_Picture_in_Window(sf::RenderWindow &window, sf::Text &text ,sf::VertexArray &pointmap);

int main()
{
    union Iterations iter = {};
    float x_center = WIDTH  / 2;
    float y_center = HEIGHT / 2;
    __m256 scale   = _mm256_set1_ps (100.f);

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
        Set_Mandelbrot_Pixel_Arr(&iter, pointmap, x_center, y_center, scale);
        float delta_time = clock.restart().asSeconds();
        Print_FPS(text, delta_time, str, file_out);

        Show_Picture_in_Window(window, text, pointmap);
    }
    fclose(file_out);
    printf("end\n");
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

void Check_Keyboard(sf::RenderWindow &window , float* x_center, float* y_center, __m256* scale)
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
            *scale    = _mm256_mul_ps( *scale, _mm256_set1_ps(1.2f));
            *x_center = (*x_center - WIDTH / 2) * 1.2 + WIDTH / 2;
            *y_center = (*y_center - HEIGHT / 2) * 1.2 + HEIGHT / 2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash))
        {
            *scale    = _mm256_div_ps( *scale, _mm256_set1_ps(1.2f));
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

void Set_Mandelbrot_Pixel_Arr(union Iterations* iter, sf::VertexArray &pointmap, float x_center, float y_center, __m256 scale)
{
    for (int point_number_y = 0; point_number_y < HEIGHT; point_number_y++)
    {
        for (int point_number_x = 0; point_number_x < WIDTH; point_number_x += 8)
        {
            float x_0 = (float) (point_number_x - x_center);
            float y_0 = (float) (point_number_y - y_center);

            Calculate_iters(x_0, y_0, iter, scale);

            for (int i = 0; i < 8; i++)
            {
                int pixel_index = point_number_x + WIDTH * point_number_y + i;
                pointmap[pixel_index].position  = sf::Vector2f(point_number_x + i, point_number_y);

                if(iter -> arr[i] < 256)
                    pointmap[pixel_index].color = sf::Color(iter -> arr[i], iter -> arr[i] % 8 * 32, 255 - iter -> arr[i]);
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
