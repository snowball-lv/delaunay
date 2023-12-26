#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class App {
    sf::RenderWindow window;
    sf::Event event;
    sf::Time delta;
    bool closeWindow = false;
    void quit();
protected:
    virtual void load() {};
    virtual void update() {}
    virtual void draw() {}
    virtual void processInput(const sf::Event &e);
    sf::Event getEvent() const { return event; };
public:
    sf::RenderWindow& getWindow() { return window; }
    sf::Time getDelta() const { return delta; };
    void run();
};
