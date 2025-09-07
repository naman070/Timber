#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

const int NUM_BRANCHES = 6;

enum class side {LEFT, RIGHT, NONE};
vector<side> branchPositions (NUM_BRANCHES);

void createSpriteForBranches(vector<sf::Sprite>& branches){
    for(int i=0; i<NUM_BRANCHES; i++){
        // Need to put them out of our window's resolution
        branches[i].setPosition({-2000, -2000});
        branches[i].setOrigin({220, 20});
    }
}

void updateBranches(vector<sf::Sprite>& branches, vector<side>& branchPositions){
    // Update the branches sprites
    for(int i=0; i<NUM_BRANCHES; i++){
        float height = i*150;
        if(branchPositions[i] == side::LEFT){
            // Move the sprite to the left side
            branches[i].setPosition({610, height});
            // Flip the sprite round the other way
            branches[i].setRotation(sf::degrees(180));
        }
        else if(branchPositions[i] == side::RIGHT){
            // Move the sprite to the right side
            branches[i].setPosition({1330, height});
            branches[i].setRotation(sf::degrees(0));
        }
        else branches[i].setPosition({3000, height});
    }
}

void updateBranchPositions(int seed){
    // Move all the branches down one place
    srand((int)time(0) + seed);
    for(int j=0; j<NUM_BRANCHES; j++){
        int r = rand()%5;
        switch(r){
            case 0:
                branchPositions[j] = side::LEFT;
                break;
            case 1:
                branchPositions[j] = side::RIGHT;
                break;
            default:
                branchPositions[j] = side::NONE;
                break;
        }
    }
};


int main(){
    // Create a videomode object and render a window for the game.
    sf::RenderWindow window(sf::VideoMode({1920u, 1080u}), "Timber");

    // Create a texture to hold a graphic on the GPU
    // Set the sprite background to cover the screen, so set the position to {0, 0}
    sf::Texture textureBackground;
    (void)textureBackground.loadFromFile("graphics/background.png");
    sf::Sprite spriteBackground(textureBackground);
    spriteBackground.setPosition({0, 0});

    // Create a Tree Sprite
    sf::Texture textureTree;
    (void)textureTree.loadFromFile("graphics/tree.png");
    sf::Sprite spriteTree(textureTree);
    spriteTree.setPosition({810, 0});

    // Create a bee sprite.
    sf::Texture textureBee;
    (void)textureBee.loadFromFile("graphics/bee.png");
    sf::Sprite spriteBee(textureBee);
    spriteBee.setPosition({0, 800});

    bool beeActive = false;
    float beeSpeed = 0.0f;

    // Create 3 sprite cloud sprites from 1 texture
    sf::Texture textureCloud;
    (void)textureCloud.loadFromFile("graphics/cloud.png");
    sf::Sprite spriteCloud1(textureCloud), spriteCloud2(textureCloud), spriteCloud3 (textureCloud);
    spriteCloud1.setPosition({0, 0});
    spriteCloud2.setPosition({0, 250});
    spriteCloud3.setPosition({0, 500});
    std::vector<sf::Sprite> cloudList {spriteCloud1, spriteCloud2, spriteCloud3};
    std::vector<pair<bool, float>> cloudInfoList (3, {false, 0.0f});

    // prepare the branches.
    sf::Texture textureBranch;
    (void)textureBranch.loadFromFile("graphics/branch.png");
    vector<sf::Sprite> branches (NUM_BRANCHES, sf::Sprite(textureBranch));
    createSpriteForBranches(branches);

    // Track player's score.
    int score = 0;

    // Create a font.
    sf::Font font;
    (void)font.openFromFile("fonts/KOMIKAP_.ttf");

    // Once we loaded a font, we need a SFML text object.
    sf::Text messageText(font), scoreText (font);
    messageText.setString("Press Enter to Start!");
    scoreText.setString("Score = 0");
    
    // Make it really big
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // Choose a color
    messageText.setFillColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::White);

    // Position the text
    sf::FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin(
        {textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f}
    );
    messageText.setPosition({1920/2.0f, 1080/2.0f});
    scoreText.setPosition({20, 20});

    // Variables to control the time itself (JEDI POWERS UNLOCKED)
    sf::Clock clock;

    // Time bar
    sf::RectangleShape timeBar;
    float timeBarStartWidth = 400, timeBarHeight = 80;
    timeBar.setSize({timeBarStartWidth, timeBarHeight});
    timeBar.setFillColor(sf::Color::Red);
    timeBar.setPosition({(1920/2) - timeBarStartWidth/2, 980});
    
    sf::Time gameTimeTotal;
    float timeRemaining = 6.0f, timeBarWidthPerSecond = timeBarStartWidth/timeRemaining;

    // Track whether the game is running. Game will be paused when it is started.
    bool paused = true;

    srand((int)time(0));

    updateBranchPositions(rand()%10);

    // Create a main game loop
    while(window.isOpen()){
        /*
            --------------------------------------------------
            HANDLE THE PLAYERS INPUT
            --------------------------------------------------
        */
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape)) window.close();
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Enter)){
            paused = false;

            // Reset the timer and the score.
            score = 0;
            timeRemaining = 6;
        }

        /*
            --------------------------------------------------
            UPDATE THE SCENE
            --------------------------------------------------
        */

        if(!paused){
            // Measure Time
            sf::Time dt = clock.restart();

            timeRemaining -= dt.asSeconds();

            // size up the timebar
            timeBar.setSize({timeBarWidthPerSecond*timeRemaining, timeBarHeight});

            if(timeRemaining <= 0.0f){
                // pause the game
                paused = true;

                // Change the message shown to the player
                messageText.setString("Out of time!!");

                // Reposition the text based on new size
                sf::FloatRect textRectNew = messageText.getLocalBounds();
                messageText.setOrigin(
                    {textRectNew.position.x + textRectNew.size.x / 2.0f, textRectNew.position.y + textRectNew.size.y / 2.0f}
                );
                messageText.setPosition({1920/2.0f, 1080/2.0f});
            }

            // setup the bee
            if(!beeActive){
                // How fast is the bee
                beeSpeed = (rand() % 200) + 200;

                // How high is the bee
                float height = (rand()%500) + 500;
                spriteBee.setPosition({2000, height});
                beeActive = true;
            }
            else{
                // Move the Bee
                spriteBee.setPosition(
                    {spriteBee.getPosition().x - beeSpeed * dt.asSeconds(), spriteBee.getPosition().y}
                );
                // Has the bee reached the left edge of the screen
                if(spriteBee.getPosition().x < -100) beeActive = false;
            }

            for(int i=0; i<3; i++){
                if(!cloudInfoList[i].first){
                    // Random speed 50 - 150
                    cloudInfoList[i].second = 50 + rand() % 100;  

                    // Random height: top third, middle third, bottom third
                    float height = 50 + i * 20 + rand() % 200;
                    cloudList[i].setPosition({-200, height});

                    cloudInfoList[i].first = true;
                }
                else{
                    // Move the cloud
                    cloudList[i].setPosition(
                        {cloudList[i].getPosition().x + cloudInfoList[i].second * dt.asSeconds(), cloudList[i].getPosition().y}
                    );
                    // Has the cloud reached the right hand of the screen.
                    if(cloudList[i].getPosition().x > 1920) cloudInfoList[i].first = false;
                }
            }

            // Update the Score text
            std::stringstream ss;
            ss<<"Score = "<<score;
            scoreText.setString(ss.str());

            updateBranches(branches, branchPositions);
        }

       /*
            --------------------------------------------------
            DISPLAY THE WINDOW
            --------------------------------------------------
       */
        window.clear();
        window.draw(spriteBackground);
        window.draw(spriteTree);
        window.draw(spriteBee);
        window.draw(timeBar);
        for(int i=0; i<3; i++) window.draw(cloudList[i]);
        window.draw(scoreText);
        for(int i=0; i<NUM_BRANCHES; i++) window.draw(branches[i]);
        if(paused) window.draw(messageText);
        window.display();
    }
}
