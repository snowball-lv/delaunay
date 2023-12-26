#include <dgen/App.h>

void App::processInput(const sf::Event &e) {
    if (event.type == sf::Event::Closed
            || (event.type == sf::Event::KeyReleased
                    && event.key.code == sf::Keyboard::Escape)) {
        closeWindow = true;
        return;
    }
    else if (event.type == sf::Event::Resized) {
        window.setView(sf::View(sf::FloatRect(
                0, 0,
                event.size.width, event.size.height)));
    }
}

void App::quit() {
    if (window.isOpen())
        window.close();
}

void App::run() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(1280, 720), "Delauney",
            sf::Style::Default, settings);
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i(
        (desktop.width - window.getSize().x) / 2,
        (desktop.height - window.getSize().y) / 2));
    load();
    sf::Clock clock;
    while (window.isOpen() && !closeWindow) {
        delta = clock.restart();
        while (window.pollEvent(event)) {
            processInput(event);
        }
        update();
        window.clear();
        draw();
        window.display();
    }
    quit();
}