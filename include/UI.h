#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


class Button {

    public :

    sf::RectangleShape shape;
    sf::Text text;
    bool isPressed = false;


    Button(const sf::Vector2f& size, const sf::Font& font, const std::string& label) {
        shape.setSize(size);
        shape.setFillColor(sf::Color(200, 200, 200));
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


class UI {
private:

public:

    void setup(std::vector<std::string> &labels , std::vector<Button> &buttons , sf::Font &font , sf::RectangleShape &inputBox , sf::Text &inputText ) {


    //Setting the buttons and symbols

    float btnWidth = 100, btnHeight = 50;
    float startX = 20, startY = 300;
    int buttonsPerRow = 5;

    for (size_t i = 0; i < labels.size(); ++i) {
        Button button({btnWidth, btnHeight}, font, labels[i]);
        float x = startX + (i % buttonsPerRow) * (btnWidth + 10);
        float y = startY + (i / buttonsPerRow) * (btnHeight + 10);
        button.setPosition(x, y);
        buttons.push_back(button);
    }

    //Setting the white box and the string written inside it

    inputBox.setPosition(20, 20);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setOutlineThickness(2);

    inputText.setFont(font);
    inputText.setCharacterSize(24);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(30, 30);



    }


} ;




