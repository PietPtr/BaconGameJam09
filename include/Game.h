#include <SFML/Graphics.hpp>
#include <iostream>
#include "Drill.h"

using namespace sf;

enum GameState {
    START,
    PLAY,
    GAMEOVER
};

struct Item {
    Vector2f position;
    int rotation;
    int type;
};;

class Game
{
    public:
        Game(RenderWindow* window);
        void initialize();
        void update();
        void draw();
        void drawWorld();
        void drawMonitor();
        void drawBuriedItems();

        void loadTextures();
        void loadWords();

        bool isWindowOpen();
    protected:
    private:
        RenderWindow* window;
        View view;

        Clock clock;
        Time dt;
        Time totalTime;
        int frame = 0;

        int windowWidth = 1280;
        int windowHeight = 736;

        Texture drillTexture;
        Texture grassTexture;
        Texture stoneTexture;
        Texture dirtTexture;
        Texture fontTexture;

        int amountOfBuriedItemTextures = 9;
        std::vector<Texture> buriedStuffTextures;
        std::vector<Item> items;

        Drill player{ &drillTexture };

        std::vector<std::string> words;
        std::string enteredText;
        int randomWordIndex = 0;

        GameState gamestate = START;
        bool gameStarted = false;

};
