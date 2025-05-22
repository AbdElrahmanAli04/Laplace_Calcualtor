#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../include/UI.h" 


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Laplace Calculator");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("../assets/Asana-Math.ttf")) {
        return -1;
    }
    
    // Making the Calc buttons
    std::vector<std::string> labels = {"sin", "cos", "e", "Pi", "s", "t", "+", "-", "*", "/", "Clear" };
    std::vector<Button> buttons;

    sf::RectangleShape inputBox(sf::Vector2f(760, 50));
    sf::Text inputText;
    std::string inputStr;


    //Setting Up UI
    UI Ui ; 
    Ui.setup(labels , buttons , font , inputBox , inputText ) ;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                for (auto& button : buttons) {
                    if (button.contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
                        button.press();
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                for (auto& button : buttons) {
                    if (button.isPressed) {
                        button.release();
                        if (button.contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
                            std::string label = button.text.getString();
                            if (label == "Clear")
                                inputStr.clear();
                            else if (label == "π")
                                inputStr += "3.1416";
                            else
                                inputStr += label;
                            inputText.setString(inputStr);
                        }
                    }
                }
            }
        }

        // Rendering
        window.clear(sf::Color(50, 50, 50));
        window.draw(inputBox);
        window.draw(inputText);
        for (auto& button : buttons)
            button.draw(window);
        window.display();
    }

    return 0;
}
