#include <SFML/Graphics.hpp>
#include "Game.h"
#include <fstream>
#include <string>

using namespace sf;

void celShade(Sprite sprite, RenderWindow* window, Color shadeColor);
int randint(int low, int high);
int randint(int low, int high, int seed);
void drawString(RenderWindow* window, std::string text, Vector2f position, Texture* fontTexture, Color color, int newLine);

Game::Game(RenderWindow* _window)
{
    window = _window;
}

void Game::initialize()
{
    loadTextures();
    loadWords();

    randomWordIndex = randint(0, words.size());
}

void Game::update()
{
    Event event;
    while (window->pollEvent(event))
    {

        if (event.type == sf::Event::Closed)
            window->close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
            {
                window->close();
            }
            if (event.key.code == Keyboard::F2)
            {
                Image Screen = window->capture();
                Screen.saveToFile("screenshot" + std::to_string(totalTime.asMicroseconds()) + ".png");
            }
            /* Cheating
            if (event.key.code == Keyboard::F1)
                player.setFuel(100);
            if (event.key.code == Keyboard::F3)
                player.addPosition(100);
            //*/
            if (gamestate == START)
            {
                if (event.key.code == Keyboard::Return)
                {
                    gamestate = PLAY;
                    enteredText = "";
                }
            }
            if (gamestate == GAMEOVER)
            {
                if (event.key.code == Keyboard::Y)
                {
                    gamestate = START;
                    player = Drill(&drillTexture);
                    items.clear();
                    gameStarted = false;
                }
                if (event.key.code == Keyboard::N)
                {
                    window->close();
                }
            }
        }
        if (event.type == sf::Event::TextEntered && gamestate == PLAY)
        {
            if (event.text.unicode == '\b' && enteredText.size() > 0)
                enteredText.erase(enteredText.size() - 1, 1);
            else if (event.text.unicode > 31 && event.text.unicode < 128 && enteredText.length() <= 23)
            {
                enteredText += static_cast<char>(event.text.unicode);
            }
            std::transform(enteredText.begin(), enteredText.end(), enteredText.begin(), toupper);
        }

        if (event.type == Event::Resized)
        {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            windowWidth = event.size.width;
            windowHeight = event.size.height;
            window->setView(View(visibleArea));
        }
    }

    dt = clock.restart();
    totalTime += dt;

    if (enteredText.compare(words[randomWordIndex]) == 0)
    {
        player.addFuel(words[randomWordIndex].size() * 7);
        randomWordIndex = randint(0, words.size());
        enteredText = "";
    }

    if (!gameStarted && player.getPosition().y > 3)
    {
        gameStarted = true;
    }
    if (gameStarted && player.getSpeed() == 0)
    {
        gamestate = GAMEOVER;
    }

    if (player.getPosition().y > 1000)
    {
        int spawnBound = (int)(windowWidth / 2 / 32 / 2) - 1;
        int itemScarcity = spawnBound / 2;

        for (int i = items.size() - 1; i >= 0; i--)
        {
            if (items.at(i).position.y < (player.getPosition().y - windowHeight / 2))
                items.erase(items.begin() + i);
        }
        if (items.size() < itemScarcity)
        {
            for (int i = 0; i < (itemScarcity - items.size()); i++)
            {


                Item item;
                float randomx;
                switch(randint(0, 1))
                {
                case(0):
                    randomx = -randint(2, spawnBound) * 32;
                    break;
                case(1):
                    randomx = randint(1, spawnBound) * 32;
                    break;
                }
                item.position = Vector2f(randomx, player.getPosition().y + 200 + randint(0, 20) * 32);
                item.type = randint(0, buriedStuffTextures.size() - 1);
                item.rotation = randint(0, 3) * 90;
                items.push_back(item);
            }
        }
    }

    player.update();

    frame++;
}

void Game::draw()
{
    window->clear(Color(51, 139, 255));

    view.setCenter(Vector2f(0, -128 + player.getPosition().y));
    view.setSize(windowWidth, windowHeight);
    view.zoom(1 / 2.0);

    /* Zooming
    if (Keyboard::isKeyPressed(Keyboard::Comma))
    {
        view.zoom(2);
    }
    if (Keyboard::isKeyPressed(Keyboard::Period))
    {
        view.zoom(8);
    }
    //*/

    window->setView(view);

    drawWorld();

    drawBuriedItems();

    player.draw(window, totalTime);

    drawMonitor();

    window->display();
}

void Game::drawWorld()
{
    int distanceDug = (int)(player.getPosition().y);

    if (distanceDug < (9800 + windowHeight))
    {
        RectangleShape leftGround(Vector2f(windowWidth / 4 - 16, windowHeight + distanceDug));
        leftGround.setTexture(&dirtTexture);
        leftGround.setTextureRect(IntRect(0, 0, leftGround.getSize().x, leftGround.getSize().y));
        leftGround.setPosition(Vector2f(view.getCenter().x - windowWidth / 4, 0));
        window->draw(leftGround);

        RectangleShape rightGround = leftGround;
        rightGround.setPosition(Vector2f(view.getCenter().x + 16, 0));
        window->draw(rightGround);

        RectangleShape groundbg(Vector2f(32, windowHeight + distanceDug));
        groundbg.setTexture(&dirtTexture);
        groundbg.setTextureRect(IntRect(0, 0, groundbg.getSize().x, groundbg.getSize().y));
        groundbg.setPosition(view.getCenter().x - 16, 3);
        window->draw(groundbg);

        groundbg.setFillColor(Color(20, 20, 20, 205));
        window->draw(groundbg);

        RectangleShape middleGround(Vector2f(32, windowHeight));
        middleGround.setTexture(&dirtTexture);
        middleGround.setTextureRect(IntRect(0, distanceDug, middleGround.getSize().x, middleGround.getSize().y));
        middleGround.setPosition(view.getCenter().x - 16, distanceDug);
        window->draw(middleGround);
    }

    if (distanceDug < windowHeight + 1000)
    {
        RectangleShape grassLeft(Vector2f(windowWidth / 4 - 16, 32));
        grassLeft.setTexture(&grassTexture);
        grassLeft.setTextureRect(IntRect(0, 0, grassLeft.getSize().x, grassLeft.getSize().y));
        grassLeft.setPosition(Vector2f(view.getCenter().x - windowWidth / 4, -28));
        window->draw(grassLeft);

        RectangleShape grassRight = grassLeft;
        grassRight.setPosition(view.getCenter().x + 16, -28);
        window->draw(grassRight);
    }

    if (distanceDug > (9800))
    {
        RectangleShape leftStone(Vector2f(windowWidth / 4 - 16, windowHeight + distanceDug - 10000));
        leftStone.setTexture(&stoneTexture);
        leftStone.setTextureRect(IntRect(0, 0, leftStone.getSize().x, leftStone.getSize().y));
        leftStone.setPosition(Vector2f(view.getCenter().x - windowWidth / 4, 10000));
        window->draw(leftStone);

        RectangleShape rightStone = leftStone;
        rightStone.setPosition(view.getCenter().x + 16, 10000);
        window->draw(rightStone);

        RectangleShape stonebg(Vector2f(32, windowHeight + distanceDug - 10000));
        stonebg.setTexture(&stoneTexture);
        stonebg.setTextureRect(IntRect(0, 0, stonebg.getSize().x, stonebg.getSize().y));
        stonebg.setPosition(view.getCenter().x - 16, 10000);
        window->draw(stonebg);

        stonebg.setFillColor(Color(20, 20, 20, 150));
        window->draw(stonebg);

        RectangleShape middleStone(Vector2f(32, windowHeight));
        middleStone.setTexture(&stoneTexture);
        middleStone.setTextureRect(IntRect(0, distanceDug, middleStone.getSize().x, middleStone.getSize().y));
        //if (distanceDug > 1200)
        middleStone.setPosition(view.getCenter().x - 16, distanceDug);
        window->draw(middleStone);
    }


}

void Game::drawMonitor()
{
    Vector2f monitorPos = view.getCenter() - Vector2f(200, 25);

    RectangleShape monitor(Vector2f(150, 104));
    monitor.setPosition(monitorPos);
    monitor.setFillColor(Color(15, 15, 15));
    monitor.setOutlineColor(Color(139, 145, 148));
    monitor.setOutlineThickness(2);
    window->draw(monitor);

    if (gamestate == START)
    {
        drawString(window, "I DIG TYPING TYPE THE GREEN WORD AND PRESS SPACE TO FUEL YOUR DRILL AND EXPLORE THE DEPTHS OF THE EARTH. DON'T LOSE SPEED OR IT'S GAME OVER. PRESS ENTER TO START.",
                   monitorPos + Vector2f(-3, 2), &fontTexture, Color(0, 200, 0), 21);
    }
    else if (gamestate == PLAY)
    {
        drawString(window, words[randomWordIndex], monitorPos + Vector2f(-3, 2), &fontTexture, Color(0, 200, 0), 100);
        Color displayColor(200, 0, 0);
        if (enteredText.compare(words[randomWordIndex]) == 0)
        {
            displayColor = Color(0, 200, 0);
        }
        drawString(window, enteredText, monitorPos + Vector2f(-3, 13), &fontTexture, displayColor, 100);

        drawString(window, "FUEL:  ", monitorPos + Vector2f(-3, 30), &fontTexture, Color(0, 200, 0), 40);
        drawString(window, "SPEED: " + std::to_string((int)player.getSpeed()), monitorPos + Vector2f(-3, 41), &fontTexture, Color(0, 200, 0), 40);
        drawString(window, "DEPTH: " + std::to_string((int)player.getPosition().y / 10), monitorPos + Vector2f(-3, 52), &fontTexture, Color(0, 200, 0), 40);

        RectangleShape fuelBar(Vector2f(player.getFuel(), 5.0f));
        fuelBar.setPosition(monitorPos + Vector2f(46, 31));
        fuelBar.setOutlineColor(Color(0, 200, 0));
        fuelBar.setOutlineThickness(1);
        fuelBar.setFillColor(Color(0, 0, 0));
        window->draw(fuelBar);
    }
    else if (gamestate == GAMEOVER)
    {
        drawString(window, "GAME OVER.", monitorPos + Vector2f(-3, 2), &fontTexture, Color(0, 200, 0), 21);
        drawString(window, "DEPTH: " + std::to_string((int)player.getPosition().y / 10), monitorPos + Vector2f(-3, 30), &fontTexture, Color(0, 200, 0), 40);
        drawString(window, "Y TO PLAY AGAIN& N TO QUIT", monitorPos + Vector2f(-3, 52), &fontTexture, Color(0, 200, 0), 21);
    }
}

void Game::drawBuriedItems()
{
    for (int i = 0; i < items.size(); i++)
    {
        Vector2u textureSize = buriedStuffTextures.at(items.at(i).type).getSize();

        Sprite itemSprite;
        itemSprite.setOrigin(Vector2f(textureSize.x / 2, textureSize.y / 2));
        itemSprite.setPosition(items.at(i).position);
        itemSprite.setTexture(buriedStuffTextures.at(items.at(i).type));
        itemSprite.setRotation(items.at(i).rotation);
        celShade(itemSprite, window, Color(0, 0, 0));
        window->draw(itemSprite);
    }
}

bool Game::isWindowOpen()
{
    return window->isOpen();
}

void Game::loadTextures()
{
    if (!drillTexture.loadFromFile("content/drill.png"))
        window->close();
    if (!dirtTexture.loadFromFile("content/dirt.png"))
        window->close();
    if (!grassTexture.loadFromFile("content/grasstop.png"))
        window->close();
    if (!fontTexture.loadFromFile("content/font.png"))
        window->close();
    if (!stoneTexture.loadFromFile("content/stone.png"))
        window->close();

    for (int i = 0; i < amountOfBuriedItemTextures; i++)
    {
        Texture buriedItem;
        if (!buriedItem.loadFromFile("content/buried/" + std::to_string(i) + ".png"))
            window->close();
        buriedStuffTextures.push_back(buriedItem);
    }
    buriedStuffTextures.at(0).setRepeated(true);
    dirtTexture.setRepeated(true);
    grassTexture.setRepeated(true);
    stoneTexture.setRepeated(true);
}


void Game::loadWords()
{
    std::ifstream file("content/words.txt");

    while(!file.eof())
    {
        std::string line;
        std::getline(file, line);
        if (line.length() > 1)
            words.push_back(line);
    }
    file.close();
}






