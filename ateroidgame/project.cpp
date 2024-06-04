#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

constexpr float TURN_SPEED = 200.0f;
constexpr float PLAYER_SPEED = 200.0f;
constexpr float M_PI = 3.14159f;
constexpr float BULLET_SPEED = 400.0f;

class Entity {
public:
    Entity(sf::Vector2f position, float angle)
        : position(position), angle(angle) {}

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    sf::Vector2f position;
    float angle;
};

class Bullet : public Entity {
public:
    Bullet(sf::Vector2f position, sf::Vector2f direction)
        : shape(1.0f), direction(direction), Entity(position, 0.0f) {
        shape.setFillColor(sf::Color::Yellow);
    }

    void update(float deltaTime) override {
        position += direction * BULLET_SPEED * deltaTime;
    }

    void render(sf::RenderWindow& window) override {
        window.draw(shape, sf::Transform().translate(position));
    }

private:
    sf::Vector2f direction;
    sf::CircleShape shape;
};

class Player : public Entity {
public:
    Player()
        : Entity(sf::Vector2f(500, 500), 0), array(sf::Quads, 4) {
        array[0].position = sf::Vector2f(20, 0);
        array[1].position = sf::Vector2f(-40, -20);
        array[2].position = sf::Vector2f(-50, 0);
        array[3].position = sf::Vector2f(-40, 20);

        for (size_t i = 0; i < array.getVertexCount(); i++) {
            array[i].color = sf::Color::Red;
        }
    }

    void update(float deltaTime) override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            angle -= TURN_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            angle += TURN_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float radians;
            radians = (M_PI / 180) * angle;

            position.x += cos(radians) * PLAYER_SPEED * deltaTime;
            position.y += sin(radians) * PLAYER_SPEED * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            float radians = angle * (M_PI / 180.0f);
            bullets.push_back(new Bullet(position, sf::Vector2f(cos(radians), sin(radians))));
        }
    }

    void render(sf::RenderWindow& window) override {
        window.draw(array, sf::Transform().translate(position).rotate(angle));
    }

    std::vector<Bullet*>& getBullets() {
        return bullets;
    }

private:
    sf::VertexArray array;
    std::vector<Bullet*> bullets;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 900), "Space Warcraft", sf::Style::Close | sf::Style::Titlebar);
    sf::Clock clock;

    std::vector<Entity*> entities;
    Player player;
    entities.push_back(&player);

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event e{};
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        player.update(deltaTime);
        player.render(window);

        for (Bullet* bullet : player.getBullets()) {
            bullet->update(deltaTime);
            bullet->render(window);
        }

        window.display();
    }

    // Cleanup
    for (Entity* entity : entities) {
        delete entity;
    }

    return 0;
}