#include "Drill.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;

void celShade(Sprite sprite, RenderWindow* window, Color shadeColor);

Drill::Drill(Texture* _drillTexture)
{
    drillTexture = _drillTexture;
}

void Drill::update()
{
    if (fuel > 0)
    {
        speed += 0.1;
        fuel -= (position.y / 20000);
    }

    int maxSpeed = 5;
    position.y += speed;
    speed = speed < 0 ? 0 : speed - 0.07f;         //friction;
    speed = speed < 0 ? 0 : speed;                 //the drill can't go up, so speed can never be negative.
    speed = speed > maxSpeed ? maxSpeed : speed;   //limit speed;

    if (fuel > 100)
        fuel = 100;
}

void Drill::draw(RenderWindow* window, Time totalTime)
{
    int animationFrame = 0;

    if (speed > 0)
    {
        int frameTime = 50;
        int moduloTime = totalTime.asMilliseconds() % (3 * frameTime);
        if (moduloTime > 0 && moduloTime <= frameTime)
            animationFrame = 0;
        else if (moduloTime > frameTime && moduloTime <= 2 * frameTime)
            animationFrame = 1;
        else if (moduloTime > 2 * frameTime && moduloTime <= 3 * frameTime)
            animationFrame = 2;
    }

    Sprite drillSprite;
    drillSprite.setPosition(position);
    drillSprite.setTexture(*drillTexture);
    drillSprite.setTextureRect(IntRect(32 * animationFrame, 0, 32, 64));
    drillSprite.setOrigin(Vector2f(16, 48));
    celShade(drillSprite, window, Color(0, 0, 0));
    window->draw(drillSprite);

}
