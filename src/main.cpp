#include <iostream>
#include <vector>

#include "camera.h"
#include <SFML/Graphics.hpp>

constexpr size_t WINDOW_WIDTH = 1280;
constexpr size_t WINDOW_HEIGHT = 720;
constexpr float LINE_WIDTH = 4;
constexpr float PLAYER_WIDTH = 50;
constexpr float PLAYER_SPEED = 20;
constexpr float GRAVITY = 4;
constexpr float JUMP_SPEED = -50;

constexpr float FLOOR_HEIGHT = 140;
constexpr float WORLD_TEX_WIDTH = 576;
constexpr float WORLD_TEX_HEIGHT = 160;
constexpr float WORLD_WIDTH = WORLD_TEX_WIDTH * (WINDOW_HEIGHT / WORLD_TEX_HEIGHT);
constexpr float WORLD_HEIGHT = WINDOW_HEIGHT;
constexpr float SOFT_LIMIT_PERCENT = 1.f / 3;
constexpr float HARD_LIMIT_PERCENT = 1.f / 5;

constexpr float SOFT_CAMERA_ALPHA = 0.1f;

struct Player {
    Player()
        : position(WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_HEIGHT),
          velocity{},
          shape({PLAYER_WIDTH, PLAYER_WIDTH}),
          isJumping{false} {
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin({PLAYER_WIDTH / 2, PLAYER_WIDTH});
        shape.setPosition({WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_HEIGHT});
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(shape, states);
    }

    // should have a deltaTime here if we didn't have fixed fps.
    void update() {
        position += velocity;
        position.x = std::clamp(position.x, PLAYER_WIDTH * 0.5f, WORLD_WIDTH - PLAYER_WIDTH * 0.5f);
        if (position.y >= WINDOW_HEIGHT - FLOOR_HEIGHT) {
            position.y = WINDOW_HEIGHT - FLOOR_HEIGHT;
        }

        shape.setPosition(position);
    }

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    bool isJumping;
};

int main() {
    sf::RenderWindow window(sf::VideoMode{{WINDOW_WIDTH, WINDOW_HEIGHT}}, "Cameras");
    window.setFramerateLimit(60);

    // Load background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/grunge-tileset.png")) {
        return 1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    float backgroundScaleFactor = WORLD_HEIGHT / WORLD_TEX_HEIGHT;
    backgroundSprite.setScale({backgroundScaleFactor, backgroundScaleFactor});

    sf::RectangleShape leftSoftLine({4, 720});
    leftSoftLine.setFillColor(sf::Color(0, 255, 0, 128));
    leftSoftLine.setOrigin({LINE_WIDTH / 2, 0});
    leftSoftLine.setPosition({WINDOW_WIDTH / 3.f, 0});

    sf::RectangleShape rightSoftLine({4, 720});
    rightSoftLine.setFillColor(sf::Color(0, 255, 0, 128));
    rightSoftLine.setOrigin({LINE_WIDTH / 2, 0});
    rightSoftLine.setPosition({WINDOW_WIDTH * 2.f / 3.f, 0});

    sf::RectangleShape leftHardLine({4, 720});
    leftHardLine.setFillColor(sf::Color(255, 0, 0, 128));
    leftHardLine.setOrigin({LINE_WIDTH / 2, 0});
    leftHardLine.setPosition({WINDOW_WIDTH * HARD_LIMIT_PERCENT, 0});

    sf::RectangleShape rightHardLine({4, 720});
    rightHardLine.setFillColor(sf::Color(255, 0, 0, 128));
    rightHardLine.setOrigin({LINE_WIDTH / 2, 0});
    rightHardLine.setPosition({WINDOW_WIDTH * (1.f - HARD_LIMIT_PERCENT), 0});

    Player player;

    sf::View worldView = window.getView();
    sf::View guiView = window.getView();
    Camera camera(sf::Vector2f(window.getSize()), {WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f});

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                return 0;
            } else if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Q) {
                    return 0;
                }
            }
        }

        float ax = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
            ax += PLAYER_SPEED;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
            ax -= PLAYER_SPEED;
        }
        player.velocity.x = ax;
        if ((player.position.y >= WINDOW_HEIGHT - FLOOR_HEIGHT)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space)) {
                player.velocity.y += JUMP_SPEED;
            }
        } else {
            player.velocity.y += GRAVITY;
        }
        player.update();
        // camera

        // TODO: Remove this line and replace it with code that determines if the player's (center)
        // is between the two green lines (open space), in the soft exclusion zones (between the
        // green and red lines), or in the hard exclusion zones (beyond either of the red lines).
        //
        // If the player is in the open zone, the camera does not need to move.
        //
        // If the player is in the soft exclusion zones, the camera should softly translate over
        // multiple frames (via an exponential moving average) until the player's center is aligned
        // with the violated soft boundary. Use SOFT_CAMERA_ALPHA for the alpha value.
        //
        // If the player is in the hard exclusion zones, the camera should hard-adjust so that
        // the player's center is always kept at most *at* the red lines. This means that the
        // player center must NEVER pass the red lines.
        //
        // Use SOFT_LIMIT_PERCENT and HARD_LIMIT_PERCENT to parameterize the green and red
        // line positions.
        
        // 1. grab player position in window percent for soft limit percent and hard limit percent comparisons
        sf::Vector2f playerWorldPos{player.position};
        sf::Vector2f playerWindowPercentPos{camera.worldToWindowPercent(playerWorldPos)};
       
        // 2. determine if player's x pos is in the dead zone (between the 2 green lines)
        bool inDeadZone{false};
        // NOTE: just an x check, y doesn't need to be checked this prep I think
        if (playerWindowPercentPos.x > SOFT_LIMIT_PERCENT && playerWindowPercentPos.x < 1 - SOFT_LIMIT_PERCENT) {
            inDeadZone = true;
        }

        // 3. determine if player's x pos is in the the soft exclusion zone
        bool inSoftZone{false};
        if ((playerWindowPercentPos.x > HARD_LIMIT_PERCENT && playerWindowPercentPos.x < SOFT_LIMIT_PERCENT) ||
            (playerWindowPercentPos.x > 1 - SOFT_LIMIT_PERCENT && playerWindowPercentPos.x < 1 - HARD_LIMIT_PERCENT)) {
            inSoftZone = true;
        }

        // 4. determine if player's x pos is in the hard zone
        bool inHardZone{false};
        if (playerWindowPercentPos.x < HARD_LIMIT_PERCENT || playerWindowPercentPos.x > 1 - HARD_LIMIT_PERCENT) {
            inHardZone = true;
        }

        // 5. deal with the 3 cases
        if (inDeadZone) {
            // do literally nothing
        } else if (inSoftZone) {
            if (playerWindowPercentPos.x > HARD_LIMIT_PERCENT && playerWindowPercentPos.x < SOFT_LIMIT_PERCENT) {
                // Case 1: left line
                // NOTE: y set to 0 because I don't think it matters
                // camera.setCameraPercent(playerWorldPos, sf::Vector2f{SOFT_LIMIT_PERCENT, 0});
            } else {
                // Case 2: right line
                // camera.setCameraPercent(playerWorldPos, sf::Vector2f{1 - SOFT_LIMIT_PERCENT, 0});

            }

        } else if (inHardZone) {
            if (playerWindowPercentPos.x < HARD_LIMIT_PERCENT) {
                // Case 1: left line
                // we map the line TO the player's position!
                camera.setCameraPercent(playerWorldPos, sf::Vector2f{HARD_LIMIT_PERCENT, 0});
            } else {
                // Case 2: right line
                camera.setCameraPercent(playerWorldPos, sf::Vector2f{1 - HARD_LIMIT_PERCENT, 0});
            }
        }

        worldView.setCenter(camera.getCameraCenter());
        window.clear(sf::Color::Black);
        // world rendering
        window.setView(worldView);
        window.draw(backgroundSprite);
        player.draw(window, sf::RenderStates{});
        // gui rendering
        window.setView(guiView);
        window.draw(leftSoftLine);
        window.draw(rightSoftLine);
        window.draw(leftHardLine);
        window.draw(rightHardLine);
        // display to screen
        window.display();
    }
    return 0;
}
