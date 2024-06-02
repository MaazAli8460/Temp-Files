/*
#################
Ahmed AMIN
i200602
#################
*/

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <queue>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>


using namespace std;
using namespace sf;

#define COINCOUNT 10;
#define _TIMER 30;


class Tile {
    RectangleShape tile;
    Texture tex;
    float width, height;
    float x, y;
public:
    Tile() {
        tile.setFillColor(Color::White);
        tile.setPosition(50, 50);
        tile.setSize(Vector2f(99.7, 99.7));
        x = y = 50;
        width = height = 100;
    }

    void smooth(bool flag) {
        tex.setSmooth(flag);
        tile.setTexture(&tex, true);
    }

    void tileTexture(string image) {
        tex.loadFromFile(image);
        tex.setSmooth(false);
        tex.setRepeated(true);
        tile.setTexture(&tex, true);
    }

    void Size(float w, float h) {
        width = w;
        height = h;
        tile.setSize(Vector2f(100, 100));
    }

    void Position(float x, float y) {
        tile.setPosition(x, y);
    }

    void Draw(RenderWindow& win) {
        win.draw(tile);
    }
};

class Entity {
    Sprite sp;
    Texture tex;
    bool hide;
    float X, Y, sx, sy;
public:
    Entity(string image = "temp.png") {
        tex.loadFromFile(image);
        sp.setTexture(tex, true);
        sp.setOrigin(sp.getLocalBounds().width/2.0, sp.getGlobalBounds().height/2.0);
        hide = false;
        X = Y = 0;
    }

    void Origin() {
        sp.setOrigin(sp.getLocalBounds().width/2.0, sp.getGlobalBounds().height/2.0);
    }

    void entityTexture(string image) {
        tex.loadFromFile(image);
        sp.setTexture(tex, true);
    }

    float getWidth() {
        return sp.getLocalBounds().width;
    }

    float getHeight() {
        return sp.getLocalBounds().height;
    }

    float getLeft() {
        sp.setOrigin(0, 0);
        float temp = sp.getPosition().x;
        Origin();
        return temp;
    }

    float getTop() {
        sp.setOrigin(0, 0);
        float temp = sp.getPosition().y;
        Origin();
        return temp;
    }

    void Color(Color c) {
        sp.setColor(c);
    } 

    void Color(int R, int G, int B, int T) {
        sp.setColor(sf::Color(R, G, B, T));
    }

    void Hide() {
        hide = true;
    }

    void Unhide() {
        hide = false;
    }

    bool isHidden() {
        return hide;
    }

    void setX(float x) {
        X = x;
        sp.setPosition(x, Y);
    }

    void setY(float y) {
        Y = y;
        sp.setPosition(X, y);
    }

    void Position(float x, float y) {
        X = x;
        Y = y;
        sp.setPosition(x, y);
    }

    void Move(float x, float y) {
        X += x;
        Y += y;
        sp.move(x, y);
    }

    void Scale(float scale) { // Scale both dimension
        sx = sy = scale;
        sp.setScale(scale, scale);
    }

    void scaleX(float X) {
        sx = X;
        sp.scale(X, sy);
    }

    void scaleY(float Y) {
        sy = Y;
        sp.scale(sx, Y);
    }

    float getsX() {
        return sx;
    }

    float getsY() {
        return sy;
    }

    float getX() {
        return X;
    }

    float getY() {
        return Y;
    }

    void display() {
        cout << X << " " << Y << endl;
    }

    void setRotate(float angle) { 
        sp.setRotation(angle);
    }

    void Rotate(float angle) { //Clockwise
        sp.rotate(angle);
    }

    float Draw(RenderWindow& win) {
        if (!isHidden())
            win.draw(sp);
    }
};

struct playerThread {
    Entity* P;
    Keyboard::Key keys[4];
    int number;
};

struct coinThread {
    Entity* C;
    int count;
};

struct tileThread {
    Tile* tile;
    int type;
};

struct scoreThread {
    Entity *P, *C;
    bool end;
};

struct timerStruct {
    Text* text;
    bool* flag;
};


void* Timer(void* arg) {
    bool* flag = ((timerStruct*)arg)->flag;
    Text* text = ((timerStruct*)arg)->text;
    sf::Font f;
    Clock clock;
    sf::Time elapsed;
    int sec = _TIMER;

    f.loadFromFile("QuixoticSans-Bold.otf");

    text->setCharacterSize(50);
    text->setFillColor(Color::Black);
    text->setStyle(Text::Bold);
    text->setFont(f);
    text->setPosition(530, 20);
    // cout << clock.getElapsedTime().asSeconds() << endl;

    while ((int)elapsed.asSeconds() <= sec) {
        elapsed = clock.getElapsedTime();
        // sec = 
        string _timer = to_string(sec - (int)elapsed.asSeconds());
        text->setString(_timer);
    }
    *flag = true;
}


bool checkCollision(Entity* E1, Entity* E2) {
    Rect<float> f1(E1->getLeft(), E1->getTop(), 30, 40);
    Rect<float> f2(E2->getLeft(), E2->getTop(), 32, 35);

    return f1.intersects(f2);
}

void* scoreCalculate(void* arg) {
    Entity* player = ((scoreThread*)arg)->P;
    Entity* coins = ((scoreThread*)arg)->C;
    queue<int>* pQ = new queue<int>;
    bool ip = 0; int maxc = COINCOUNT;
    bool collide = false;

    SoundBuffer buffer;
    if(!buffer.loadFromFile("coin.wav"))
    cout<<"SOUND not found!";
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(10);

    while (!((scoreThread*)arg)->end) {
        sleep(sf::milliseconds(50));
        collide = false;
        for (int ic = 0; ic < maxc; ic++) {
            collide = false;
            collide = checkCollision(&player[ip], &coins[ic]);
            if (collide && !coins[ic].isHidden()) {
                // cout << "Coin Picked by Player " << ip + 1 << endl;
               // sound.play();
                pQ->push(ip);
                coins[ic].Hide();
            }
        }
        ip = !ip;
    }
    pthread_exit((void*)pQ);
}

void* displayCoin(void* arg) {
    Entity* coins = ((coinThread*)arg)->C; 
    int count = ((coinThread*)arg)->count;
    string states = "coin1.png";
    int coincount = COINCOUNT;

    for (int i = 0; i < coincount; i++) {
        coins[i].Scale(0.15);
        coins[i].Origin();
        coins[i].entityTexture(states);
        coins[i].Position((rand() % 29 + 7)*25, (rand() % 29 + 7)*25);
    }
    

    while(1) {
        sf::sleep((sf::milliseconds(50)));
        for (int i = 0; i < coincount; i++) {
            if (coins[i].isHidden()) {
                // sf::sleep((sf::milliseconds(100)));
                coins[i].Position((rand() % 19 + 5)*25, (rand() % 19 + 5)*25);
                coins[i].Unhide();
            }
            coins[i].entityTexture(states);
            coins[i].Origin();
        }
    }
}

void* playerMovement(void* arg){
    Entity* p = ((playerThread*)arg)->P;
    Keyboard::Key* _key = ((playerThread*)arg)->keys;

    while(1) {
        sf::sleep((sf::milliseconds(2)));
        if (Keyboard::isKeyPressed(_key[0])) {
            // cout << "UP" << endl;
            if (p->getY() > 95) {
                p->Move(0, -1);
            }
            // p->display();
        }
        else if (Keyboard::isKeyPressed(_key[1])) {
            // cout << "LEFT" << endl;
            if (p->getX() > 125) {
                p->Move(-1, 0);
            }
            // p->display();
        }
        else if (Keyboard::isKeyPressed(_key[2])) {
            // cout << "DOWN" << endl;
            if (p->getY() < 835) {
                p->Move(0, 1);
            }
            // p->display();
        }
        else if (Keyboard::isKeyPressed(_key[3])) {
            // cout << "RIGHT" << endl;
            if (p->getX() < 875) {
                p->Move(1, 0);
            }  
            // p->display();
        }
    }
}

void drawGrid(RenderWindow& win) {
    VertexArray linex(Lines, 2), liney(Lines, 2);
    for (float i = 1; i <= 1001;) {
        linex[0].position = Vector2f(i, 0);
        linex[1].position = Vector2f(i, 1000);
        i += 50;
        win.draw(linex);
    }

    for (float i = 1; i <= 1001;) {
        liney[0].position = Vector2f(0, i);
        liney[1].position = Vector2f(1000, i);
        i += 50;
        win.draw(liney);
    }
}



int main() {
    XInitThreads();
    bool grid = true;
    srand(time(0));
    RenderWindow window(VideoMode(1001, 1001), "Coin Purge", Style::Close);
    window.setFramerateLimit(60);
    pthread_t pid1, pid2;
    pthread_attr_t p_att1, p_att2;
    pthread_attr_init(&p_att1);
    pthread_attr_init(&p_att2);
    pthread_attr_setdetachstate(&p_att1, PTHREAD_CREATE_DETACHED);
    pthread_attr_setdetachstate(&p_att2, PTHREAD_CREATE_DETACHED);

    const int ncoin = COINCOUNT;
    Entity Player[2], coin[ncoin];
    Player[0].entityTexture("p1.png");
    Player[0].Scale(0.2); Player[0].Origin(); Player[0].Position(124, 98);
    Player[1].entityTexture("p2.png");
    Player[1].Scale(0.2); Player[1].Origin(); Player[1].Position(874, 834);

    Keyboard _key;
    
    queue<playerThread*> PlayerQueue;
    playerThread* Player1 = new playerThread, *Player2 = new playerThread;
    Player1->number = 0;
    Player2->number = 1;
    Player1->P = &Player[0];
    Player2->P = &Player[1];
    Keyboard::Key* k = Player1->keys;
    k[0] = _key.Up; k[1] = _key.Left; k[2] = _key.Down; k[3] = _key.Right;
    k = Player2->keys;
    k[0] = _key.W; k[1] = _key.A; k[2] = _key.S; k[3] = _key.D;

    //Player 1 and 2 threads
    pthread_create(&pid1, &p_att1, playerMovement, (void*)Player1);
    pthread_create(&pid2, &p_att2, playerMovement, (void*)Player2);
    


    pthread_t cid; pthread_attr_t c_attr;
    pthread_attr_init(&c_attr);
    pthread_attr_setdetachstate(&c_attr, PTHREAD_CREATE_DETACHED);
    coinThread* _Coins = new coinThread;
    _Coins->C = coin;
    _Coins->count = COINCOUNT;

    pthread_create(&cid, &c_attr, displayCoin, (void*)_Coins);


    int coinspicked = 0, pscore[2]{};

    pthread_t tid; pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    tileThread* _Tile = new tileThread;


    cout << coin[0].getX() << " " << coin[0].getY() << endl;


    pthread_t scoreID;
    scoreThread* scorethread = new scoreThread;
    scorethread->C = coin;
    scorethread->P = Player;
    scorethread->end = false;
    pthread_create(&scoreID, NULL, scoreCalculate, (void*)scorethread);


    Text* text = new Text;
    bool stop = false;
    
    Struct* TIMER = new timerStruct;
    TIMER->text = text;
    TIMER->flag = &stop;
    pthread_t timerID; pthread_attr_t timer_attr;
    pthread_attr_init(&timer_attr);
    pthread_attr_setdetachstate(&timer_attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&timerID, &timer_attr, Timer, (void*)TIMER);


    sf::Font f;
    f.loadFromFile("QuixoticSans-Bold.otf");
    string texttime = "TIMER: ";
    Text txtime;
    txtime.setCharacterSize(50);
    txtime.setFont(f);
    txtime.setStyle(Text::Bold);
    txtime.setFillColor(Color::Black);
    txtime.setPosition(330, 20);
    txtime.setString(texttime);
//GAME LOOP
    while (window.isOpen() && !(*(TIMER->flag))) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape))
                window.close();
            if (event.type == Event::MouseButtonPressed)
                cout << Mouse::getPosition().x << " " << Mouse::getPosition().y << endl;
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Tab)
                grid = !grid;
            // if (Keyboard::isKeyPressed(Keyboard::Space))
            //     cout << Player[0].getX() << " " << Player[0].getY() << endl;
        } 


        window.clear();
        if (grid)
            drawGrid(window);
        

//DRAW STUFF HERE
        int coincount = COINCOUNT;
        for (int i = 0; i < coincount; i++)
            coin[i].Draw(window);

        for (int i = 1; i >= 0; i--)
            Player[i].Draw(window);

        window.draw(*text);
        window.draw(txtime);
        window.display();   
    }


    scorethread->end = true;
    void* status;
    pthread_join(scoreID, &status);
    queue<int>* scores = ((queue<int>*)status);
    
    while (!scores->empty()) {
        coinspicked++;
        pscore[scores->front()]++;
        scores->pop();
    }

    cout << "Total Coins Picked: " << coinspicked << endl;
    cout << "Player1 score: " << pscore[0] << endl;
    cout << "Player2 score: " << pscore[1] << endl;
}
