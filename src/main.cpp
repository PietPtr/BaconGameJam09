#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>

using namespace sf;
Time globalTime;
Clock globalClock;

int main()
{
    srand(time(NULL));

    int windowWidth = 1280;
    int windowHeigth = 736;
    RenderWindow window(VideoMode(windowWidth, windowHeigth), "I dig typing");
    window.setVerticalSyncEnabled(true);

    Game game(&window);

    game.initialize();

    while (window.isOpen())
    {
        globalTime += globalClock.restart();

        game.update();
        game.draw();
    }

    return 0;
}

int randint(int low, int high, int seed)
{
    srand(seed);
    int value = rand() % (high + 1 - low) + low;
    return value;
}

int randint(int low, int high)
{
    int value = rand() % (high + 1 - low) + low;
    srand(globalTime.asMicroseconds());
    globalTime += globalClock.restart();

    return value;
}

void celShade(Sprite sprite, RenderWindow* window, Color shadeColor)
{
    sprite.setColor(shadeColor);
    Vector2f spritePos = sprite.getPosition();
    sprite.setPosition(Vector2f(spritePos.x - 1, spritePos.y));
    window->draw(sprite);
    sprite.setPosition(Vector2f(spritePos.x + 1, spritePos.y));
    window->draw(sprite);
    sprite.setPosition(Vector2f(spritePos.x, spritePos.y - 1));
    window->draw(sprite);
    sprite.setPosition(Vector2f(spritePos.x, spritePos.y + 1));
    window->draw(sprite);
}

void drawString(RenderWindow* window, std::string text, Vector2f position, Texture* fontTexture, Color color, int newLine)
{
    bool shouldStartNewLine = false;

    float drawY = position.y;
    float drawX = position.x;

    int letterWidth = 5;
    int letterHeight = 7;

    int charactersSinceNewline = 0;
    for (int i = 0; i < text.length(); i++)
    {
        int num = (int)(text[i]);

        Sprite charSprite;
        charSprite.setTexture(*fontTexture);

        drawX += letterWidth + 1;

        if (num >= 48 && num <= 57)
            charSprite.setTextureRect(IntRect((num - 48) * letterWidth, 0, letterWidth, letterHeight));
        else if (num >= 65 && num <= 90)
            charSprite.setTextureRect(IntRect((num - 55) * letterWidth, 0, letterWidth, letterHeight));
        else if (num >= 97 && num <= 122)
            charSprite.setTextureRect(IntRect((num - 61) * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 58)
            charSprite.setTextureRect(IntRect(62 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 46)
            charSprite.setTextureRect(IntRect(63 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 44)
            charSprite.setTextureRect(IntRect(64 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 32)
            charSprite.setTextureRect(IntRect(66 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 45)
            charSprite.setTextureRect(IntRect(67 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 39)
            charSprite.setTextureRect(IntRect(68 * letterWidth, 0, letterWidth, letterHeight));
        else if (num == 38)
        {
            shouldStartNewLine = true;
            charSprite.setTextureRect(IntRect(66 * letterWidth, 0, letterWidth, letterHeight));
        }
        else
            charSprite.setTextureRect(IntRect(65 * letterWidth, 0, letterWidth, letterHeight));

        charSprite.setColor(color);
        charSprite.setPosition(Vector2f(drawX, drawY));
        window->draw(charSprite);

        charactersSinceNewline++;

        if (newLine != -1 && charactersSinceNewline % newLine == 0 && i != 0)
        {
            shouldStartNewLine = true;
        }
        if (shouldStartNewLine && num == 32)
        {
            shouldStartNewLine = false;
            drawY += letterHeight + 4;
            drawX = position.x;
            charactersSinceNewline = 0;
        }
    }
}
