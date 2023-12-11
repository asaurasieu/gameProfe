#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <chrono>

enum class Direction
{
    None,
    Left,
    Right,
    Up,
    Down
};

class Ghost
{
public:

    friend class GameTest; // Friend class for Google Test access
    float x, y;
    sf::Sprite sprite;

    Ghost(float x, float y, const sf::Texture &texture);

    void draw(sf::RenderWindow &window);
    void updateghost(float maxX, float maxY, const std::vector<sf::FloatRect> &walls, std::mt19937 &rng, sf::Time elapsedTime);

private:
    sf::Time moveTimer; // Timer to control movement interval
};

class Game
{
public:
    Game();
    int run();

private:
    sf::Texture backgroundTexture;
    sf::Texture playerTexture;
    sf::Texture ghostTexture;
    sf::Sprite endGameBackground;
    sf::Texture endGameBackgroundTexture;
    bool gameEnded = false;

    std::random_device rd;
    std::mt19937 rng;
    std::uniform_real_distribution<float> distX, distY;

    sf::Clock gameClock;
    sf::Text timerText;
    sf::Font font;

    sf::Time lastUpdateTime;
    static const sf::Time UpdateInterval;
    static const std::string FONT_PATH;

    int ghostsEaten;
    int wallCollisions;
    bool isColliding;

    static const float RADIUS;

    bool rightPressed;
    bool leftPressed;
    bool upPressed;
    bool downPressed;

    bool leftBlocked;
    bool rightBlocked;
    bool upBlocked;
    bool downBlocked;

    bool touchingLeftBorder = false;
    bool touchingRightBorder = false;
    bool touchingTopBorder = false;
    bool touchingBottomBorder = false;

    Direction lastDirection;

public:
    sf::CircleShape player;
    sf::RenderWindow window;
    sf::Sprite background;
    std::vector<Ghost> ghosts;
    static const float PLAYER_START_X;
    static const float PLAYER_START_Y;
    static const float SCENE_WIDTH;
    static const float SCENE_HEIGHT;
    int initWindow();
    int initBackground();
    int initPlayer();
    void processInput();
    void update();
    void render();
    void initGhosts();
    void initTimer();
    void updateTimer();
    void spawnNewGhost();
    int initGhostTexture();
    bool isValidPosition(float x, float y);
    void resetMovementFlags();
    void endGame();
};