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
    std::cout << words[randomWordIndex] << "\n";
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
                Screen.saveToFile("screenshot.png");
            }
            if (event.key.code == Keyboard::F1)
                player.setFuel(500);
            if (event.key.code == Keyboard::F3)
                player.addPosition(1000);
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

    player.update();

    frame++;
}

void Game::draw()
{
    window->clear(Color(51, 139, 255));

    view.setCenter(Vector2f(0, -128 + player.getPosition().y));
    view.setSize(windowWidth, windowHeight);
    view.zoom(1 / 2.0);

    if (Keyboard::isKeyPressed(Keyboard::Comma))
    {
        view.zoom(2);
    }
    if (Keyboard::isKeyPressed(Keyboard::Period))
    {
        view.zoom(8);
    }

    window->setView(view);

    drawWorld();

    //drawBuriedItems();

    player.draw(window, totalTime);

    drawMonitor();

    window->display();
}

void Game::drawWorld()
{
    int distanceDug = (int)(player.getPosition().y);

    if (distanceDug < (9800 + windowHeight))
    {
        RectangleShape leftGround(Vector2f(windowWidth / 2 - 16, windowHeight + distanceDug));
        leftGround.setTexture(&dirtTexture);
        leftGround.setTextureRect(IntRect(0, 0, leftGround.getSize().x, leftGround.getSize().y));
        leftGround.setPosition(Vector2f(view.getCenter().x - windowWidth / 2, 0));
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
        RectangleShape grassLeft(Vector2f(windowWidth / 2 - 16, 32));
        grassLeft.setTexture(&grassTexture);
        grassLeft.setTextureRect(IntRect(0, 0, grassLeft.getSize().x, grassLeft.getSize().y));
        grassLeft.setPosition(Vector2f(view.getCenter().x - windowWidth / 2, -28));
        window->draw(grassLeft);

        RectangleShape grassRight = grassLeft;
        grassRight.setPosition(view.getCenter().x + 16, -28);
        window->draw(grassRight);
    }

    if (distanceDug > (9800))
    {
        RectangleShape leftStone(Vector2f(windowWidth / 2 - 16, windowHeight + distanceDug - 10000));
        leftStone.setTexture(&stoneTexture);
        leftStone.setTextureRect(IntRect(0, 0, leftStone.getSize().x, leftStone.getSize().y));
        leftStone.setPosition(Vector2f(view.getCenter().x - windowWidth / 2, 10000));
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
        drawString(window, "TYPE THE GREEN WORD AND PRESS SPACE TO FUEL YOUR DRILL AND EXPLORE THE DEPTHS OF THE EARTH. DON'T LOSE SPEED OR IT'S GAME OVER. PRESS ENTER TO START.",
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
    Vector2f topLeft;// = view.getCenter() - Vector2f(windowWidth / 2, windowHeight / 2);
    topLeft.x = (int)(view.getCenter().x - (windowWidth  / 2.0)) - ((int)(view.getCenter().x - (windowWidth  / 2.0)) % 32);
    topLeft.y = (int)(view.getCenter().y - (windowHeight / 2.0)) - ((int)(view.getCenter().y - (windowHeight / 2.0)) % 32);

    for (int y = 0; y < windowHeight / 32; y++)
    {
        for (int x = 0; x < windowWidth / 32; x++)
        {
            int seed = ((int)(y + topLeft.y / 32) % 4096) * ((int)(x + topLeft.x / 32) % 4096) * 32;
            if (randint(0, 100, seed) == 0)
            {
                Sprite buried;
                buried.setPosition(x * 32 + topLeft.x, y * 32 + topLeft.y);
                buried.setTexture(buriedStuffTextures.at(randint(0, buriedStuffTextures.size() - 1, seed)));
                window->draw(buried);
            }
        }
    }
    std::cout << "\n";
}

bool Game::isWindowOpen()
{
    return window->isOpen();
}

void Game::loadTextures()
{
    if (!drillTexture.loadFromFile("drill.png"))
        window->close();
    if (!dirtTexture.loadFromFile("dirt.png"))
        window->close();
    if (!grassTexture.loadFromFile("grasstop.png"))
        window->close();
    if (!fontTexture.loadFromFile("font.png"))
        window->close();
    if (!stoneTexture.loadFromFile("stone.png"))
        window->close();

    for (int i = 0; i <3; i++)
    {
        Texture buriedItem;
        if (!buriedItem.loadFromFile("buried/" + std::to_string(i) + ".png"))
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
    std::ifstream file("words.txt");

    while(!file.eof())
    {
        std::string line;
        std::getline(file, line);
        if (line.length() > 1)
            words.push_back(line);
    }
    file.close();
}






