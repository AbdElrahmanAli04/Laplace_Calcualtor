#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Button {

    public :

    sf::RectangleShape shape;
    sf::Text text;
    bool isPressed = false;


    Button(const sf::Vector2f& size, const sf::Font& font, const std::wstring& label   ) {
        shape.setSize(size);
        shape.setFillColor(sf::Color(244, 187, 68)); 
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
    }

    void setPosition(float x, float y) {
        shape.setPosition(x, y);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            x + (shape.getSize().x - textBounds.width) / 2 - textBounds.left,
            y + (shape.getSize().y - textBounds.height) / 2 - textBounds.top
        );
    }

    bool contains(const sf::Vector2f& point) const {
        return shape.getGlobalBounds().contains(point);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    void press() {
        shape.move(2, 2);
        isPressed = true;
    }

    void release() {
        if (isPressed) {
            shape.move(-2, -2);
            isPressed = false;
        }
    }
};

class Cursor : public sf::RectangleShape {
    private : 
        float _blinkRate = 0.5f; 
        bool _showCursor = true;


    public : 

    Cursor() {
        this->setSize(sf::Vector2f(2, 24)) ;
        this->setFillColor(sf::Color::Black);
    }
    
    bool IsShown () {
        return _showCursor ;
    }

    float getBlinkRate() {
        return _blinkRate ;
    }

    void ReverseState() {
        _showCursor = !_showCursor ;
    }

};


class UI {
private:

    void setupLabels(std::vector<std::wstring> &labels , std::vector<Button> &buttons , sf::Font &font ) {
    
    std::vector<std::wstring> Operators = {L" * ", L" + ", L" - ", L" / ", L".", L"^", L"0", L"(", L")", L"del", L"C" , L"="};

    //Setting the Functions and symbols

    float btnWidth = 70, btnHeight = 50;
    float startX = 20, startY = 300;
    int buttonsPerRow = 3;

    for (int i = 0; i < labels.size(); ++i) {
        Button button({btnWidth, btnHeight}, font, labels[i]);
        float x = startX + (i % buttonsPerRow) * (btnWidth + 10);
        float y = startY + (i / buttonsPerRow) * (btnHeight + 10);
        button.setPosition(x, y);
        buttons.push_back(button);
        }

    //Setting numbers and operators
    btnWidth = 50;
    btnHeight = 50;
    startX = 500;
    startY = 300;
    buttonsPerRow = 5;  
    int number = 7;     // Starts at 7 (top-left)
    int opCounter = 0;  // Tracks operator index

    for (int j = 0; j < Operators.size()+9; j++) {
        int row = j / buttonsPerRow;  
        int col = j % buttonsPerRow;  

        // Determine if this is a number or operator
        std::wstring label;

        if (col < 3 && j < 15 ) {
            // Numbers: 7,8,9 → 4,5,6 → 1,2,3
            label = std::to_wstring(number - (row * 3) + col);
        } 
        
        else {
            // Operators:
            if (opCounter < Operators.size()) {
                label = (Operators[opCounter++]) ; 
            } else {
                label = L"?";  // Fallback if no more operators
            }
        }

        // Create and position the button
        Button button({btnWidth, btnHeight}, font, label);
        float x = startX + col * (btnWidth + 10);
        float y = startY + row * (btnHeight + 10);
        button.setPosition(x, y);
        buttons.push_back(button);

    
    }

}

    void setupInputBox(sf::RectangleShape &inputBox) {
        inputBox.setPosition(20, 20);
        inputBox.setFillColor(sf::Color::White);
        inputBox.setOutlineColor(sf::Color::Black);
        inputBox.setOutlineThickness(2);
    }

    void setupInputText( sf::Font &font  , sf::Text &inputText ) {
        inputText.setFont(font);
        inputText.setCharacterSize(24);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(30, 30);

    }


public:

    void setup(std::vector<std::wstring> &labels , std::vector<Button> &buttons , sf::Font &font , sf::RectangleShape &inputBox , sf::Text &inputText ) {

    setupLabels(labels , buttons , font) ;

    //Setting the white box and the string written inside it

    setupInputBox(inputBox) ;
    setupInputText(font,inputText) ;




    }


} ;




