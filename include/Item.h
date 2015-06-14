#ifndef ITEM_H
#define ITEM_H
#include <SFML/Graphics.hpp>

using namespace sf;

class Item
{
    public:
        Item();
        void draw();
    protected:
    private:
        Vector2f position;
        int itemType;
};

#endif // ITEM_H
