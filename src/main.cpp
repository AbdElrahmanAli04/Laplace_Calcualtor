#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../include/UI.h" 
#include "Solve.cpp"


int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "Laplace Calculator" );
    sf::Texture Mango ; 

    Mango.loadFromFile("../assets/pngtree-sweet-mango-fruit-png-png-image_11495826.png");

    window.setFramerateLimit(60);

    sf::Sprite Mangosprite;
    Mangosprite.setTexture(Mango);
    Mangosprite.setScale(0.7,0.7);
    Mangosprite.setPosition(300,50) ;

    sf::Font font;
    if (!font.loadFromFile("../assets/ARIAL.TTF")) {
        return -1;
    }
    
    //Setting Cursor 
    sf::Clock clock;
    Cursor cursor ; 

    // Making the Calc buttons
    std::vector<std::wstring> labels = {L"sin", L"cos", L"sinh" , L"cosh" , L"e", L"t" };
    std::vector<Button> buttons;

    sf::RectangleShape inputBox(sf::Vector2f(760, 50));
    sf::Text inputText;
    std::wstring inputStr;
    std::string Result ;


    //Setting Up UI
    UI Ui ; 
    Ui.setup(labels , buttons , font , inputBox , inputText ) ;

    while (window.isOpen()) {
        sf::Event event;

        if (clock.getElapsedTime().asSeconds() >= cursor.getBlinkRate()) {
            cursor.ReverseState();
            clock.restart();
        }

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
                            std::wstring label = button.text.getString();
                            if (label == L"sin"|| label ==  L"cos" || label == L"sinh" || label == L"cosh" || label == L"^"  ) {
                                inputStr += label + L"(" ;
                                inputText.setString(inputStr);
                            }
                            else if (label == L"C")
                                inputStr.clear();
                            else if (label == L"del" && inputStr.size() != 0 ) {
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
                                Solve ComputeSoltion (inputStr) ;
                            }

                            else if (label != L"del") {
                                inputStr += label; 
                            }

                            inputText.setString(inputStr);
                            
                        }
                    }
                }
            }

        cursor.setPosition(inputText.getPosition().x + inputText.getLocalBounds().width + 5 , inputText.getPosition().y + 4);

        }

        // Rendering
        window.clear(sf::Color(50, 50, 50));
        window.draw(inputBox);
        window.draw(inputText );
        for (auto& button : buttons)
            button.draw(window);

        if (cursor.IsShown()){
            window.draw(cursor);
        }

        window.draw(Mangosprite) ;
        window.display();
    }

    return 0;
}
