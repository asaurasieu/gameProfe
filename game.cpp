
#include "game.h"
#include <iostream>
#include <random>

Ghost::Ghost(float x, float y, const sf::Texture &texture) : x(x), y(y), moveTimer(sf::Time::Zero)
{
    sprite.setTexture(texture);
    float originalWidth = sprite.getLocalBounds().width;
    float originalHeight = sprite.getLocalBounds().height;
    float desiredSize = 40.0f;
    sprite.setScale(desiredSize / originalWidth, desiredSize / originalHeight); // Scale the sprite
    sprite.setPosition(x, y);
}

void Ghost::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}
class Wall
{
public:
    float x, y, width, height;

    static std::vector<Wall> createWalls()
    {
        std::vector<Wall> walls;
        walls.push_back({200.0f, 100.0f, 20.0f, 202.0f});
        walls.push_back({400.0f, 300.0f, 300.0f, 22.0f});
        walls.push_back({131.0f, 600.0f, 300.0f, 22.0f});
        walls.push_back({600.0f, 500.0f, 20.0f, 202.0f});
        walls.push_back({750.0f, 140.0f, 100.0f, 22.0f});
        walls.push_back({130.0f, 500.0f, 20.0f, 102.0f});
        walls.push_back({800.0f, 400.0f, 100.0f, 20.0f});

        // magic walls
        walls.push_back({145.0f, 511.0f, 5.0f, 89.0f});
        walls.push_back({762.0f, 140.0f, 76.0f, 5.0f});

        return walls;
    }
};

std::vector<Wall> walls = Wall::createWalls();

const float Game::SCENE_WIDTH = 1000.0f;
const float Game::SCENE_HEIGHT = 801.0f;
const float Game::PLAYER_START_X = 500.0f;
const float Game::PLAYER_START_Y = 400.0f;
const float Game::RADIUS = 20.0f;
const sf::Time Game::UpdateInterval = sf::seconds(1.0f);
const std::string Game::FONT_PATH = "/Users/cristina/Desktop/C++ folder game/2D-game/arial.ttf";

Game::Game() : rng(rd()), distX(0.0f, SCENE_WIDTH), distY(0.0f, SCENE_HEIGHT), ghostsEaten(0), wallCollisions(0) // Initialize wallCollisions
{
    initWindow();
    initBackground();
    initPlayer();
    initGhostTexture();
    initGhosts();
    initTimer(); // Initialize the timer

    player.setPosition(PLAYER_START_X, PLAYER_START_Y);

    rightPressed = false;

    leftPressed = false;
    upPressed = false;
    downPressed = false;
}
void Ghost::updateghost(float maxX, float maxY, const std::vector<sf::FloatRect> &walls, std::mt19937 &rng, sf::Time elapsedTime)
{
    moveTimer += elapsedTime;

    if (moveTimer.asSeconds() >= 1.0f)
    {
        // Define the step size for each movement
        const float stepSize = 50.0f; // SPEED FOR GHOST SPEED

        // Create distributions for random movement
        std::uniform_real_distribution<float> distX(-stepSize, stepSize);
        std::uniform_real_distribution<float> distY(-stepSize, stepSize);

        // Variables to hold the new position
        float newX, newY;
        bool validPosition = false;

        while (!validPosition)
        {
            newX = (x + distX(rng));
            newX = (x + distX(rng));
            newX = (x + distX(rng));
            newX = (x + distX(rng)); // Random step in X
            newY = (y + distY(rng));
            newY = (y + distY(rng));
            newY = (y + distY(rng));
            newY = (y + distY(rng)); // Random step in Y

            // Clamp the new position within the game bounds
            newX = std::max(0.0f, std::min(newX, maxX - sprite.getLocalBounds().width * sprite.getScale().x));
            newY = std::max(0.0f, std::min(newY, maxY - sprite.getLocalBounds().height * sprite.getScale().y));

            // Create a rectangle for the new position to check for wall collisions
            sf::FloatRect newBounds(newX, newY, sprite.getLocalBounds().width * sprite.getScale().x, sprite.getLocalBounds().height * sprite.getScale().y);

            // Check for collisions with walls
            validPosition = true;
            for (const auto &wall : walls)
            {
                if (newBounds.intersects(wall))
                {
                    validPosition = false;
                    break;
                }
            }
        }

        // Update ghost position
        x = newX;
        y = newY;
        sprite.setPosition(x, y);
        moveTimer = sf::Time::Zero; // Reset the timer here
    }
}

int Game::initGhostTexture()
{
    if (!ghostTexture.loadFromFile("resources/ghost.png"))
    {
        return 1; // Error loading texture
    }
    return 0; // Successfully loaded
}
int Game::initWindow()
{
    window.create(sf::VideoMode(SCENE_WIDTH, SCENE_HEIGHT), "Updated 2D Game");
    window.setFramerateLimit(120);
    return 0;
}

int Game::initBackground()
{
    if (!backgroundTexture.loadFromFile("resources/background4.png"))
    {
        return 1;
    }
    backgroundTexture.setRepeated(true);
    background.setTexture(backgroundTexture);
    background.setTextureRect(sf::IntRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT));
    return 0;
}

int Game::initPlayer()
{
    player.setRadius(RADIUS);
    player.setOrigin(RADIUS, RADIUS);
    if (!playerTexture.loadFromFile("resources/pacman.png"))
    {
        return 1;
    }
    player.setTexture(&playerTexture);
    return 0;
}

void Game::initGhosts()
{
    for (int i = 0; i < 6; ++i)
    {
        float x, y;
        do
        {
            x = distX(rng);
            y = distY(rng);
        } while (!isValidPosition(x, y)); // Updated to a new method
        ghosts.emplace_back(x, y, ghostTexture);
    }
}

void Game::initTimer()
{
    if (!font.loadFromFile("/Users/cristina/Desktop/C++ folder game/2D-game/arial.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return; // Handle the error as appropriate
    }

    timerText.setFont(font);
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(10, 10); // Set position on the screen
}
void Game::update()
{
    sf::Time elapsedTime = gameClock.restart();

    updateTimer();

    float speed = 3.0f;

    bool leftWallCollision = false;
    bool rightWallCollision = false;
    bool upWallCollision = false;
    bool downWallCollision = false;

    bool newCollision = false;

    sf::FloatRect playerBounds = player.getGlobalBounds();

    // Check for border collisions
    if (player.getPosition().x - RADIUS <= 0)
    {
        if (!touchingLeftBorder)
        {
            wallCollisions++;
            touchingLeftBorder = true;
        }
    }
    else
    {
        touchingLeftBorder = false;
    }

    if (player.getPosition().x + RADIUS >= SCENE_WIDTH)
    {
        if (!touchingRightBorder)
        {
            wallCollisions++;
            touchingRightBorder = true;
        }
    }
    else
    {
        touchingRightBorder = false;
    }

    if (player.getPosition().y - RADIUS <= 0)
    {
        if (!touchingTopBorder)
        {
            wallCollisions++;
            touchingTopBorder = true;
        }
    }
    else
    {
        touchingTopBorder = false;
    }

    if (player.getPosition().y + RADIUS >= SCENE_HEIGHT)
    {
        if (!touchingBottomBorder)
        {
            wallCollisions++;
            touchingBottomBorder = true;
        }
    }
    else
    {
        touchingBottomBorder = false;
    }

    sf::Time now = gameClock.getElapsedTime();
    if (now - lastUpdateTime >= UpdateInterval)
    {
        lastUpdateTime = now;
        timerText.setString("Time: " + std::to_string(static_cast<int>(now.asSeconds())));
    }
    std::string displayText = "Time: " + std::to_string(static_cast<int>(gameClock.getElapsedTime().asSeconds()));
    displayText += " | Ghosts Eaten: " + std::to_string(ghostsEaten);
    displayText += " | Wall Collisions: " + std::to_string(wallCollisions);
    timerText.setString(displayText);

    // Check for end game condition

    for (const auto &wall : walls)
    {
        std::vector<sf::FloatRect> wallRects;
        for (const Wall &wall : walls)
        {
            wallRects.emplace_back(wall.x, wall.y, wall.width, wall.height);
        }

        for (Ghost &ghost : ghosts)
        {
            ghost.updateghost(SCENE_WIDTH, SCENE_HEIGHT, wallRects, rng, elapsedTime); // Pass the elapsed time to updateghost
        }

        sf::FloatRect wallBounds(wall.x, wall.y, wall.width, wall.height);
        if (playerBounds.intersects(wallBounds))
        {
            // Check if the wall is a red wall
            bool isRedWall = (wall.x == 145.0f && wall.y == 511.0f) || (wall.x == 762.0f && wall.y == 140.0f);

            if (!isColliding && !isRedWall)
            {
                newCollision = true;
            }

            // Check if it's one of the special walls
            if (wall.x == 145.0f && wall.y == 511.0f) // First special wall
            {
                player.setPosition(800.0f, 120.0f - RADIUS * 2);
                resetMovementFlags();
                upPressed = true;
            }
            else if (wall.x == 762.0f && wall.y == 140.0f) // Second special wall
            {
                player.setPosition(115.0f + wall.width + RADIUS * 2, 551.0f);
                resetMovementFlags();
                rightPressed = true;
            }
            else
            {
                if (player.getPosition().x < wall.x)
                {
                    rightWallCollision = true;
                }
                else if (player.getPosition().x > wall.x + wall.width)
                {
                    leftWallCollision = true;
                }

                if (player.getPosition().y < wall.y)
                {
                    downWallCollision = true;
                }
                else if (player.getPosition().y > wall.y + wall.height)
                {
                    upWallCollision = true;
                }
            }
            if (playerBounds.intersects(wallBounds))
            {
                // Check if the wall is a red wall
                bool isRedWall = (wall.x == 145.0f && wall.y == 511.0f) || (wall.x == 762.0f && wall.y == 140.0f);

                // Decrement wallCollisions if it's a red wall
                if (isRedWall)
                {
                    wallCollisions = std::max(0, wallCollisions - 1);
                }
            }
            isColliding = true;
        }
    }

    if (isColliding)
    {
        bool stillColliding = false;
        for (const auto &wall : walls)
        {
            sf::FloatRect wallBounds(wall.x, wall.y, wall.width, wall.height);
            if (playerBounds.intersects(wallBounds))
            {
                stillColliding = true;
                break;
            }
        }

        if (!stillColliding)
        {
            isColliding = false;
        }
    }

    if (!leftWallCollision && leftPressed)
    {
        if (player.getPosition().x - speed - RADIUS >= 0)
        {
            player.move(-speed, 0.0f);
        }
    }

    if (!rightWallCollision && rightPressed)
    {
        if (player.getPosition().x + speed + RADIUS <= SCENE_WIDTH)
        {
            player.move(speed, 0.0f);
        }
    }

    if (!upWallCollision && upPressed)
    {
        if (player.getPosition().y - speed - RADIUS >= 0)
        {
            player.move(0.0f, -speed);
        }
    }

    if (!downWallCollision && downPressed)
    {
        if (player.getPosition().y + speed + RADIUS <= SCENE_HEIGHT)
        {
            player.move(0.0f, speed);
        }
    }

    for (auto it = ghosts.begin(); it != ghosts.end();)
    {
        if (player.getGlobalBounds().intersects(it->sprite.getGlobalBounds()))
        {
            it = ghosts.erase(it);
            spawnNewGhost();
            ghostsEaten++; // Increment the count
        }
        else
        {
            ++it;
        }
    }
    if (newCollision)
    {
        wallCollisions++;
        if (wallCollisions >= 5)
        {
            update();
            render();
            sf::sleep(sf::seconds(1));

            endGame();
            return;
        }
    }
}

void Game::updateTimer()
{
    sf::Time now = gameClock.getElapsedTime();
    if (now - lastUpdateTime >= UpdateInterval)
    {
        lastUpdateTime = now;
        timerText.setString("Time: " + std::to_string(static_cast<int>(now.asSeconds())));
    }
}

void Game::resetMovementFlags()
{
    leftPressed = false;
    rightPressed = false;
    upPressed = false;
    downPressed = false;
}
void Game::processInput()
{
    sf::Event event;

    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Left)
            {
                if (!leftBlocked)
                {
                    leftPressed = true;
                    lastDirection = Direction::Left;
                    rightPressed = false;
                    upPressed = false;
                    downPressed = false;
                    rightBlocked = false;
                    upBlocked = false;
                    downBlocked = false;
                }
            }
            else if (event.key.code == sf::Keyboard::Right)
            {
                if (!rightBlocked)
                {
                    rightPressed = true;
                    lastDirection = Direction::Right;
                    leftPressed = false;
                    upPressed = false;
                    downPressed = false;
                    leftBlocked = false;
                    upBlocked = false;
                    downBlocked = false;
                }
            }
            else if (event.key.code == sf::Keyboard::Up)
            {
                if (!upBlocked)
                {
                    upPressed = true;
                    lastDirection = Direction::Up;
                    leftPressed = false;
                    rightPressed = false;
                    downPressed = false;
                    leftBlocked = false;
                    rightBlocked = false;
                    downBlocked = false;
                }
            }
            else if (event.key.code == sf::Keyboard::Down)
            {
                if (!downBlocked)
                {
                    downPressed = true;
                    lastDirection = Direction::Down;
                    leftPressed = false;
                    rightPressed = false;
                    upPressed = false;
                    leftBlocked = false;
                    rightBlocked = false;
                    upBlocked = false;
                }
            }
            break;
        default:
            break;
        }
    }
}

void Game::render()
{
    window.clear(sf::Color::White);

    // Draw the standard or the end game background based on the game state
    if (!gameEnded)
    {
        window.draw(background);

        for (auto &ghost : ghosts)
        {
            ghost.draw(window);
        }

        window.draw(player);
    }
    else
    {
        window.draw(endGameBackground);
    }

    // Always draw the timerText on top, so it's visible in both game states
    window.draw(timerText);

    window.display();
}

void Game::spawnNewGhost()
{
    float x, y;
    do
    {
        x = distX(rng);
        y = distY(rng);
    } while (!isValidPosition(x, y));
    ghosts.emplace_back(x, y, ghostTexture);
}

bool Game::isValidPosition(float x, float y)
{
    float ghostSize = 50.0f; // Size of the ghosts

    // Check if the position is within the game window
    if (x < 0 || x + ghostSize > SCENE_WIDTH || y < 0 || y + ghostSize > SCENE_HEIGHT)
    {
        return false;
    }

    sf::FloatRect newGhostBounds(x, y, ghostSize, ghostSize);

    // Check for intersection with walls
    for (const auto &wall : walls)
    {
        sf::FloatRect wallBounds(wall.x, wall.y, wall.width, wall.height);
        if (newGhostBounds.intersects(wallBounds))
        {
            return false;
        }
    }

    // Check for intersection with existing ghosts
    for (const auto &ghost : ghosts)
    {
        if (newGhostBounds.intersects(ghost.sprite.getGlobalBounds()))
        {
            return false;
        }
    }

    return true; // Position is valid if all checks are passed
}

int Game::run()
{
    while (window.isOpen())
    {
        processInput();
        update();
        updateTimer();
        render();
    }
    return 0;
}
void Game::endGame()
{

    if (!endGameBackgroundTexture.loadFromFile("resources/background000.png"))
    {
        std::cerr << "Failed to load end game background" << std::endl;
        return;
    }
    // Set the texture for the end game background
    endGameBackground.setTexture(endGameBackgroundTexture);

    // Scale the sprite to cover the entire window
    float scaleX = SCENE_WIDTH / endGameBackgroundTexture.getSize().x;
    float scaleY = SCENE_HEIGHT / endGameBackgroundTexture.getSize().y;
    endGameBackground.setScale(scaleX, scaleY);

    gameEnded = true; // Set the game ended flag

    render(); // Render one last time with the new background

    sf::sleep(sf::seconds(10)); // Wait for 10 seconds

    window.close(); // Close the game window
}
