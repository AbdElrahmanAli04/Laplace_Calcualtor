#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../include/UI.h" 


int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "Laplace Calculator");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("../assets/ARIAL.TTF")) {
        return -1;
    }
    
    sf::RectangleShape cursor(sf::Vector2f(2, 24)); // Thin vertical line
    cursor.setFillColor(sf::Color::Black);
    sf::Clock clock;
    bool showCursor = true;
    float blinkRate = 0.5f; 


    // Making the Calc buttons
    std::vector<std::wstring> labels = {L"sin", L"cos", L"sinh" , L"cosh" , L"e" , L"π", L"t"};
    std::vector<Button> buttons;

    sf::RectangleShape inputBox(sf::Vector2f(760, 50));
    sf::Text inputText;
    std::wstring inputStr;



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

            if (clock.getElapsedTime().asSeconds() >= blinkRate) {
                showCursor = !showCursor;
                clock.restart();
            }


            if (event.type == sf::Event::MouseButtonReleased) {
                for (auto& button : buttons) {
                    if (button.isPressed) {
                        button.release();
                        if (button.contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
                            std::wstring label = button.text.getString();
                            if (label == L"sin"|| label ==  L"cos" || label == L"sinh" || label == L"cosh" ) {
                                inputStr += label + L"(" ;
                                inputText.setString(inputStr);
                            }
                            else if (label == L"C")
                                inputStr.clear();
                            else if (label == L"del") {
                                if ( inputStr.back() == L's'|| inputStr.back() == L'n' ) {
                                    for (int i = 0 ; i<3 ; i++) {
                                        inputStr.pop_back();
                                    }
                                }
                                else if (inputStr.back() == L'h') {
                                    for (int i = 0 ; i<4 ; i++) {
                                        inputStr.pop_back();
                                    }                                    
                                }
                                else {
                                inputStr.pop_back() ;
                                }
                            }
                            else if (label == L"=") {
                                //Solve function , takes the string as argument by ref and replace it by the solution 
                            }
                            else
                                inputStr += label;
                            inputText.setString(inputStr);
                        }
                    }
                }
            }

        cursor.setPosition(inputText.getPosition().x + inputText.getLocalBounds().width , inputText.getPosition().y + 5);

        }

        // Rendering
        window.clear(sf::Color(50, 50, 50));
        window.draw(inputBox);
        window.draw(inputText  /* + Cursor*/);
        for (auto& button : buttons)
            button.draw(window);

        if (showCursor)
            window.draw(cursor);

        window.display();
    }

    return 0;
}
