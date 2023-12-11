#include "game.h"
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

class GameTest : public ::testing::Test {
protected:
    Game game;

    void SetUp() override {
        game.initWindow();
        game.initBackground();
        game.initPlayer();
        game.initGhostTexture();
        game.initGhosts();
    }
};

TEST_F(GameTest, GameComponentsInitialization) {
    game.initWindow();
    EXPECT_NE(&game.window, nullptr);  // Directly accessing the public member 'window'

    game.initBackground();
    EXPECT_TRUE(game.background.getTexture() != nullptr);  // Directly accessing the public member 'background'
}

TEST_F(GameTest, PlayerInitialPosition) {
    game.initPlayer();
    EXPECT_FLOAT_EQ(game.player.getPosition().x, Game::PLAYER_START_X);  // Directly accessing the public member 'player'
    EXPECT_FLOAT_EQ(game.player.getPosition().y, Game::PLAYER_START_Y);
}

class GhostTest : public ::testing::Test {
protected:
    sf::Texture texture;
    Ghost ghost{0, 0, texture};

    void SetUp() override {

    }
};

TEST_F(GhostTest, GhostInitialization) {
    EXPECT_FLOAT_EQ(ghost.x, 0);
    EXPECT_FLOAT_EQ(ghost.y, 0);
    // Check if sprite is set correctly
    EXPECT_TRUE(ghost.sprite.getTexture() == &texture);
}

TEST_F(GameTest, GhostMovementLogic) {  
    sf::Time elapsedTime = sf::seconds(1.0f); 
    std::vector<sf::FloatRect> walls; 
    std::mt19937 rng;
    float maxX = Game::SCENE_WIDTH;  
    float maxY = Game::SCENE_HEIGHT;
    for (auto &ghost : game.ghosts) {  
        sf::Vector2f oldPos = ghost.sprite.getPosition();
        ghost.updateghost(maxX, maxY, walls, rng, elapsedTime);
        sf::Vector2f newPos = ghost.sprite.getPosition();
        // Check if the ghost has moved
        EXPECT_FALSE(oldPos == newPos);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
