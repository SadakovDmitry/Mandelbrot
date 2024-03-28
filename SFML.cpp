#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
/*
int main()
{
    sf::Image image;
    if (image.loadFromFile("./image.png"))
    {
        printf("Succses!!!\n");
    }
    else
    {
        printf("ERROR!!!");
    }
}
*/
void Draw_Arr();

int main()
{
    Draw_Arr();
    /*
    sf::RenderWindow window(sf::VideoMode(400, 400), "My window");

    sf::RectangleShape rectangle(sf::Vector2f(100.f, 100.f));

    while (window.isOpen())
    //for (int i = 0; i < 1000; i++)
    {
        //window.isOpen();
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                break;
        }

        window.clear(sf::Color::Black);

        rectangle.setFillColor(sf::Color(100, 250, 50));
        window.draw(rectangle);

        window.display();
    }
    */
    printf("end\n");
    return 0;
}

void Draw_Arr()
{
    sf::RenderWindow window(sf::VideoMode(400, 400), "My window");
    window.setFramerateLimit(30);
    sf::VertexArray pointmap(sf::Points, 400 * 400);
    for (register int a = 0; a < 400 * 400; a++)
    {
        pointmap[a].position = sf::Vector2f(a % 400, a / 400);
        pointmap[a].color    = sf::Color(250, 0, 200);
    }

    while (window.isOpen())
    {
        //window.isOpen();
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                break;
        }

        window.clear(sf::Color::Black);
        window.draw(pointmap);
        window.display();
    }
}
