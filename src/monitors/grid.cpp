#include <SFML/Graphics.hpp>
#include <vector>

#define DIMENSION 300

int main (){
    // It creates a window
    sf::RenderWindow window(sf::VideoMode(300, 300), "SFML Window");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear(); 

        sf::RectangleShape rectangle(sf::Vector2f(100.f, 50.f));
        rectangle.setFillColor(sf::Color::Green);
        window.draw(rectangle);
        window.display();
    }
}