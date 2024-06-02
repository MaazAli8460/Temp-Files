#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;
using namespace sf;

pthread_mutex_t gameMutex = PTHREAD_MUTEX_INITIALIZER;
bool gameStarted = false;

int lives = 3;
sem_t palletSemaphore;
bool powerPalletEaten = false;

void *powerPalletTimer(void *arg)
{
    sleep(15); // Sleep for 15 seconds
    powerPalletEaten = false;
    sem_post(&palletSemaphore); // Release the semaphore after 15 seconds
    pthread_exit(NULL);
}

int matrix[21][28] =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1}, // 5,12
        {1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

class Tile
{
public:
    int sizeX;
    int sizeY;
    int posX;
    int posY;
    sf::Sprite sprite;
    char type;

    Tile(sf::Texture &, sf::Sprite, int, int, int xOfRect = 264, int sX = 32, int sY = 32);

    void draw(sf::RenderWindow &window);
};
class Map
{
public:
    std::vector<Tile> tilemap;

    void generate(sf::RenderWindow &window, sf::Texture &tileTexture, int xOfRect);
};
struct Key_Permit
{
    bool has_key = false;
    bool is_out = false;
    bool has_permit = false;
};
struct cord_grid
{
    int x = 0, y = 0;
};
Clock Ghost_escape;
int permit_count = 2;
int key_count = 2;
int ghost_count = 4;
Vector2f Ghost_screen_pos[4];
Key_Permit Key_info[4];
cord_grid ghost_grid_pos[4];
string ghost_char[4] = {"simple", "fast", "fast", "ai"};
string ghost_color[4] = {"Red", "Blue", "Pink", "Inky"};
cord_grid Default_Ghost[4] = {{7, 10}, {7, 12}, {7, 14}, {7, 16}};
Sprite Ghosts[4];
Texture G_texture[4];

pthread_mutex_t key_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t key2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t permit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t permit2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t main_mutex[4]; //= PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t Ghost_mutex[4];
char moves[49] = {'L', 'U', 'L', 'U', 'L', 'D', 'L', 'D', 'R', 'D', 'L', 'U', 'R', 'D', 'L', 'U', 'R', 'U', 'L', 'U', 'L', 'D', 'L', 'D', 'R', 'D', 'R', 'D', 'R', 'D', 'L', 'U', 'R', 'U', 'L', 'U', 'L', 'D', 'L', 'U', 'R', 'D', 'L', 'D', 'R', 'D', 'R', 'U', 'R'};
int walk[49] = {2, 2, 5, 2, 3, 2, 1, 3, 1, 3, 3, 8, 25, 8, 3, 3, 1, 3, 1, 2, 3, 2, 5, 2, 5, 5, 3, 1, 3, 7, 3, 2, 1, 3, 1, 3, 3, 8, 19, 7, 3, 2, 1, 3, 1, 2, 3, 13, 7};
int moves_max = 49;
const char title[] = "OS-Project-PAC_MAN-Spring-2024";

Sprite gr[21][28];
Texture Gr_tile;
int var = 0;

void func_draw();
string calc_move_AI()
{
    return "right";
}
void set_text_ghost_Red(char move, int index);
void set_text_ghost_Blue(char move, int index);
void set_text_ghost_Pink(char move, int index);
void set_text_ghost_Inky(char move, int index);

void Quarintine(int index)
{
    Clock stop;

    while (!Key_info[index].is_out)
    {

        if (pthread_mutex_trylock(&key_mutex) != 0)
        {
            // cout << "\nwait:" << index;
            // continue;
            // stop.restart();
            // while (stop.getElapsedTime().asSeconds()<=5)
            // {
            //     //wait

            // }
        }
        pthread_mutex_trylock(&key_mutex);
        if ((Key_info[(index + 1) % 4].has_permit == false) && (Key_info[(index + 3) % 4].has_key == false))
        {

            Key_info[index].has_key = true;
            Key_info[index].has_permit = true;
            Key_info[index].is_out = true;
        }
        pthread_mutex_unlock(&key_mutex);
    }
    Key_info[index].has_key = false;
    Key_info[index].has_permit = false;
}
bool Is_Collided(int index)
{
    int collision = false;

    Vector2f ver = Ghosts[index].getPosition();
    int xbound = ver.x + 41;
    int ybound = ver.y + 41;
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            // Vector2f  SH=gr[i][j].getPosition();
            if (gr[i][j].getGlobalBounds().intersects(Ghosts[index].getGlobalBounds()))
            {
                return true;
            }
        }
    }
    return false;
}

bool Is_collided(int index, char move)
{
    if (move == 'L')
    {
        if (matrix[ghost_grid_pos[index].x - 1][ghost_grid_pos[index].y] != 1 && ghost_grid_pos[index].x - 1 > 0)
        {
            return false;
        }
        return true;
    }
    else if (move == 'R')
    {
        if (matrix[ghost_grid_pos[index].x + 1][ghost_grid_pos[index].y] != 1 && ghost_grid_pos[index].x + 1 < 21)
        {
            return false;
        }
        return true;
    }
    else if (move == 'U')
    {
        if (matrix[ghost_grid_pos[index].x][ghost_grid_pos[index].y - 1] != 1 && ghost_grid_pos[index].y - 1 > 0)
        {
            return false;
        }
        return true;
    }
    else if (move == 'D')
    {
        if (matrix[ghost_grid_pos[index].x][ghost_grid_pos[index].y + 1] != 1 && ghost_grid_pos[index].y + 1 < 28)
        {
            return false;
        }
        return true;
    }
}
char calc_rand_move(int index, char priv_move)
{
    char move;
    Vector2f ver = Ghosts[index].getPosition();
    srand((unsigned)(time(NULL)));
    Sprite sp;
    gr[ghost_grid_pos[index].x][ghost_grid_pos[index].y] = sp;
    if (priv_move == 'R')
    {
        // Ghosts[index].setPosition(ver.x + 1, ver.y);
        if (Is_collided(index, 'R') == false)
        {
            move = 'R';
            ghost_grid_pos[index].x++;
        }
        else
        {
            bool flag = false;
            while (flag == false)
            {
                int rand1 = rand() % 3;
                if (rand1 == 0 && Is_collided(index, 'U') == false)
                {
                    move = 'U';
                    ghost_grid_pos[index].y--;
                    break;
                }
                else if (rand1 == 1 && Is_collided(index, 'D') == false)
                {
                    move = 'D';
                    ghost_grid_pos[index].y++;
                    break;
                }
                else if (rand1 == 2 && Is_collided(index, 'L') == false)
                {
                    move = 'L';
                    ghost_grid_pos[index].x--;
                    break;
                }
            }
        }
    }
    else if (priv_move == 'L')
    {
        if (Is_collided(index, 'L') == false)
        {
            move = 'L';
            ghost_grid_pos[index].x--;
        }
        else
        {
            bool flag = false;
            while (flag == false)
            {
                int rand1 = rand() % 3;
                if (rand1 == 0 && Is_collided(index, 'U') == false)
                {
                    move = 'U';
                    ghost_grid_pos[index].y--;
                    break;
                }
                else if (rand1 == 1 && Is_collided(index, 'D') == false)
                {
                    move = 'D';
                    ghost_grid_pos[index].y++;
                    break;
                }
                else if (rand1 == 2 && Is_collided(index, 'R') == false)
                {
                    move = 'R';
                    ghost_grid_pos[index].x++;
                    break;
                }
            }
        }
    }
    else if (priv_move == 'U')
    {
        if (Is_collided(index, 'U') == false)
        {
            move = 'U';
            ghost_grid_pos[index].y--;
        }
        else
        {
            bool flag = false;
            while (flag == false)
            {
                int rand1 = rand() % 3;
                if (rand1 == 0 && Is_collided(index, 'R') == false)
                {
                    move = 'R';
                    ghost_grid_pos[index].x++;
                    break;
                }
                else if (rand1 == 1 && Is_collided(index, 'D') == false)
                {
                    move = 'D';
                    ghost_grid_pos[index].y++;
                    break;
                }
                else if (rand1 == 2 && Is_collided(index, 'L') == false)
                {
                    move = 'L';
                    ghost_grid_pos[index].x--;
                    break;
                }
            }
        }
    }
    else if (priv_move == 'D')
    {

        if (Is_collided(index, 'D') == false)
        {
            move = 'D';
            ghost_grid_pos[index].y++;
        }
        else
        {
            bool flag = false;
            while (flag == false)
            {
                int rand1 = rand() % 3;
                if (rand1 == 0 && Is_collided(index, 'U') == false)
                {
                    move = 'U';
                    ghost_grid_pos[index].y--;
                    break;
                }
                else if (rand1 == 1 && Is_collided(index, 'D') == false)
                {
                    move = 'D';
                    ghost_grid_pos[index].y++;
                    break;
                }
                else if (rand1 == 2 && Is_collided(index, 'L') == false)
                {
                    move = 'L';
                    ghost_grid_pos[index].x--;
                    break;
                }
            }
        }
    }
    // Ghosts[index].setPosition(ver.x,ver.y);
    if (ghost_color[index] == "Red")
    {
        set_text_ghost_Red(move, index);
    }
    else if (ghost_color[index] == "Blue")
    {
        set_text_ghost_Blue(move, index);
    }
    else if (ghost_color[index] == "Pink")
    {
        set_text_ghost_Pink(move, index);
    }
    else if (ghost_color[index] == "Inky")
    {
        set_text_ghost_Inky(move, index);
    }
    return move;
}

void Follow_Path(int index, int &move_index, int &move_count, int &previous_index)
{
    char move;
    Sprite sp;
    gr[ghost_grid_pos[index].x][ghost_grid_pos[index].y] = sp;
    if (move_count > 0) //&&  Is_collided(index,moves[move_index])==true )
    {
        // move_index++;
        if (moves[move_index] == 'U')
        {
            /* code */
            ghost_grid_pos[index].x--;
        }
        else if (moves[move_index] == 'D')
        {
            /* code */
            ghost_grid_pos[index].x++;
        }
        else if (moves[move_index] == 'R')
        {
            /* code */
            ghost_grid_pos[index].y++;
        }
        else if (moves[move_index] == 'L')
        {
            ghost_grid_pos[index].y--;
            /* code */
        }
        move = moves[move_index];
        move_count--;
        cout << "\n"
             << move << ":" << walk[move_index];
    }
    else if (move_count == 0)
    {
        move_index++;
        return;
    }
    if (move_index == moves_max)
    {
        move_index = 0;
        previous_index = -1;
        Key_info[index].has_key = false;
        Key_info[index].has_permit = false;
        /* code */
    }

    if (ghost_color[index] == "Red")
    {
        set_text_ghost_Red(move, index);
    }
    else if (ghost_color[index] == "Blue")
    {
        set_text_ghost_Blue(move, index);
    }
    else if (ghost_color[index] == "Pink")
    {
        set_text_ghost_Pink(move, index);
    }
    else if (ghost_color[index] == "Inky")
    {
        set_text_ghost_Inky(move, index);
    }
}
void *Ghost_thread(void *args)
{
    int index = (intptr_t)args;

    Texture temp;

    Sprite sp;

    char move = 'L';
    int move_index = 0;
    int previous_index = 0;
    int move_count = 2;
    Clock ghost_delay;
    float move_pass = 0.5;
    sleep(index * 5);
    Quarintine(index);
    gr[ghost_grid_pos[index].x][ghost_grid_pos[index].y] = sp;
    ghost_grid_pos[index].x = 5;
    ghost_grid_pos[index].y = 14;
    ghost_delay.restart();

    while (true)
    {

        if (var == 1) // temporary shutdown
        {
            break;
        }
        if (Key_info[index].is_out == false)
        {
            Quarintine(index);
            gr[ghost_grid_pos[index].x][ghost_grid_pos[index].y] = sp;
            ghost_grid_pos[index].x = 5;
            ghost_grid_pos[index].y = 14;
            move_count = 2;
            previous_index = 0;
            move_index = 0;
            move = 'L';
            move_pass = 0.5;
            ghost_delay.restart();
        }

        // critical section start
        while (pthread_mutex_trylock(&main_mutex[index]) != 0)
        {
        }

        pthread_mutex_trylock(&Ghost_mutex[index]);

        if (ghost_color[index] == "Red")
        {
            // set_text_ghost_Red(move, index);
            if (ghost_delay.getElapsedTime().asSeconds() >= move_pass)
            {
                ghost_delay.restart();
                move = calc_rand_move(index, move);
            }
        }
        else if (ghost_color[index] == "Blue")
        {
            // set_text_ghost_Blue(move,index);
            if (ghost_delay.getElapsedTime().asSeconds() >= move_pass)
            {
                ghost_delay.restart();
                move = calc_rand_move(index, move);
            }
        }
        else if (ghost_color[index] == "Pink")
        {
            // set_text_ghost_Pink(move, index);
            if (ghost_delay.getElapsedTime().asSeconds() >= 0.2)
            {
                ghost_delay.restart();
                if (previous_index < move_index)
                {
                    previous_index = move_index;
                    move_count = walk[move_index];
                }

                Follow_Path(index, move_index, move_count, previous_index);
            }
        }
        else if (ghost_color[index] == "Inky")
        {
            if (ghost_delay.getElapsedTime().asSeconds() >= 0.2)
            {
                ghost_delay.restart();
                if (previous_index < move_index)
                {
                    previous_index = move_index;
                    move_count = walk[move_index];
                }

                Follow_Path(index, move_index, move_count, previous_index);
            }
        }
        Ghosts[index].setTexture(G_texture[index]); // G_texture[index]);
        Ghosts[index].setScale(0.7, 0.7);

        pthread_mutex_unlock(&Ghost_mutex[index]);

        // critical section end
        pthread_mutex_trylock(&main_mutex[index]); // dont calculate untill main executes privious commands
    }
    return NULL;
}

void Map::generate(sf::RenderWindow &window, sf::Texture &tileTexture, int xOfRect)
{
    sf::Sprite tile;
    vector<Tile> map;

    sf::Font font;
    if (!font.loadFromFile("GrinchedRegular.otf"))
    {
        // Handle error loading font
        cerr << "Error loading font" << std::endl;
        return;
    }

    int mapHeight = 21; // Define the height of the map
    int mapWidth = 28;  // Define the width of the map
    int x = 720 / 2;
    int y = 1000 / 2;
    // x -= (41 * 3);
    // y -= (41 * 3);
    x = 0;
    y = 0;
    // Loop through the matrix and create tiles based on the values
    for (int lineCount = 0; lineCount < mapHeight; lineCount++)
    {
        for (int column = 0; column < mapWidth; column++)
        {
            if (matrix[lineCount][column] == 1)
            {
                // Create wall tile
                Tile block(tileTexture, tile, column * 32, lineCount * 32, xOfRect);

                map.push_back(block);
            }
            else if (matrix[lineCount][column] == 0)
            {
                // Draw a dot for the coin
                sf::Text dot(".", font, 20);
                dot.setFillColor(sf::Color::Green);
                dot.setPosition(column * 32, lineCount * 32);
                window.draw(dot);
            }
            else if (matrix[lineCount][column] == 2)
            {
                // Draw a dot for the power pallet
                sf::Text dot("0", font, 20);
                dot.setFillColor(sf::Color::Yellow);
                dot.setPosition(column * 32, lineCount * 32);
                window.draw(dot);
            }
            if (lineCount == ghost_grid_pos[0].x && column == ghost_grid_pos[0].y)
            {
                /* code */
                gr[ghost_grid_pos[0].x][ghost_grid_pos[0].y].setTexture(G_texture[0]);
                gr[ghost_grid_pos[0].x][ghost_grid_pos[0].y].setScale(0.7, 0.7);
                gr[lineCount][column].setPosition(x, y);
                Ghost_screen_pos[0].x = x;
                Ghost_screen_pos[0].y = y;

                // window.draw(gr[lineCount][column]);
            }

            if (lineCount == ghost_grid_pos[1].x && column == ghost_grid_pos[1].y)
            {
                /* code */
                gr[ghost_grid_pos[1].x][ghost_grid_pos[1].y].setTexture(G_texture[1]);
                gr[ghost_grid_pos[1].x][ghost_grid_pos[1].y].setScale(0.7, 0.7);
                gr[lineCount][column].setPosition(x, y);
                Ghost_screen_pos[1].x = x;
                Ghost_screen_pos[1].y = y;
                // window.draw(gr[lineCount][column]);
            }

            if (lineCount == ghost_grid_pos[2].x && column == ghost_grid_pos[2].y)
            {
                /* code */
                gr[ghost_grid_pos[2].x][ghost_grid_pos[2].y].setTexture(G_texture[2]);
                gr[ghost_grid_pos[2].x][ghost_grid_pos[2].y].setScale(0.7, 0.7);
                gr[lineCount][column].setPosition(x, y);
                Ghost_screen_pos[2].x = x;
                Ghost_screen_pos[2].y = y;
                // window.draw(gr[lineCount][column]);
            }

            if (lineCount == ghost_grid_pos[3].x && column == ghost_grid_pos[3].y)
            {
                /* code */
                gr[ghost_grid_pos[3].x][ghost_grid_pos[3].y].setTexture(G_texture[3]);
                gr[ghost_grid_pos[3].x][ghost_grid_pos[3].y].setScale(0.7, 0.7);
                gr[lineCount][column].setPosition(x, y);
                Ghost_screen_pos[3].x = x;
                Ghost_screen_pos[3].y = y;
                // window.draw(gr[lineCount][column]);
            }
            x += 32;
        }
        y += 32; // 28.7;
        x = 0;
    }

    tilemap = map;
    for (int i = 0; i < map.size(); i++)
    {
        map[i].draw(window);
    }
    // for (int i = 0; i < 21; i++)
    // {
    //     for (int j = 0; j < 28; j++)
    //     {

    //         window.draw(gr[i][j]);
    //     }
    // }
}

Tile::Tile(sf::Texture &t, sf::Sprite s, int x, int y, int xOfRect, int sX, int sY)
{
    sizeX = sX;
    sizeY = sY;
    posX = x;
    posY = y;
    s.setTexture(t);
    s.setTextureRect(sf::IntRect(xOfRect, 0, 32, 32));
    s.setPosition(x, y);
    sprite = s;
}
void Tile::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

static int score = 0;

class Player
{
public:
    sf::Sprite upSprite, downSprite, leftSprite, rightSprite;
    sf::Sprite *currentSprite;    // Pointer to current sprite
    std::string direction;        // Current direction
    std::string directionToCheck; // Direction to check for next move
    int posX, posY;               // Position of the player
    bool hasPowerPellet;          // Flag to indicate if Pacman has eaten a power pellet
    sf::Clock powerPelletClock;   // Clock to track the duration of the power pellet effect
    sf::SoundBuffer coinBuffer;   // Declare sound buffer
    sf::Sound coinSound;          // Declare sound object
    sf::SoundBuffer powerPelletBuffer;
    sf::Sound powerPelletSound;
    Player(sf::Texture &upTexture, sf::Texture &downTexture, sf::Texture &leftTexture, sf::Texture &rightTexture, int x, int y)
    {
        directionToCheck = "none";

        upSprite.setTexture(upTexture);
        upSprite.setPosition(x, y);
        upSprite.setScale(2.0f, 2.0f); // Stretch the texture by 2 in both x and y directions

        downSprite.setTexture(downTexture);
        downSprite.setPosition(x, y);
        downSprite.setScale(2.0f, 2.0f); // Stretch the texture by 2 in both x and y directions

        leftSprite.setTexture(leftTexture);
        leftSprite.setPosition(x, y);
        leftSprite.setScale(2.0f, 2.0f); // Stretch the texture by 2 in both x and y directions

        rightSprite.setTexture(rightTexture);
        rightSprite.setPosition(x, y);
        rightSprite.setScale(2.0f, 2.0f); // Stretch the texture by 2 in both x and y directions

        direction = "none";
        posX = x;
        posY = y;
        currentSprite = &rightSprite; // Set the default sprite to the right direction
        hasPowerPellet = false;
        if (!coinBuffer.loadFromFile("audio/munch_1.wav"))
        {
            // Failed to load the sound
            // Handle the error
        }

        // Set the sound buffer
        coinSound.setBuffer(coinBuffer);
        if (!powerPelletBuffer.loadFromFile("audio/siren_3.wav"))
        {
            // Handle error
        }
        powerPelletSound.setBuffer(powerPelletBuffer);
    }

    int getPositionX() const { return posX; }
    int getPositionY() const { return posY; }

    // Draw the player on the window
    void draw(sf::RenderWindow &window)
    {
        window.draw(*currentSprite);
    }
    void checkCollisionWithGhostwithPallets(sf::Sprite &ghostSprite, int index)
    {
        if (!hasPowerPellet)
            return; // Pacman cannot eat ghosts without a power pellet

        // Get the global bounds of Pacman and the ghost sprites

        // Check if the global bounds intersect
        sf::FloatRect pacmanBounds = currentSprite->getGlobalBounds();
        sf::FloatRect ghostBounds = ghostSprite.getGlobalBounds();
        int tileX = currentSprite->getPosition().x; // Convert player position to tile coordinates
        int tileY = currentSprite->getPosition().x;
        int x2 = Ghost_screen_pos[index].x;
        int y2 = Ghost_screen_pos[index].y;
        pacmanBounds.left += currentSprite->getPosition().x;
        pacmanBounds.top += currentSprite->getPosition().x;
        ghostBounds.left += Ghost_screen_pos[index].x;
        ghostBounds.top += Ghost_screen_pos[index].y;

        // Check if the global bounds intersect
        cout << "\nghost:" << Ghost_screen_pos[index].x << ":" << Ghost_screen_pos[index].y;
        cout << "\nPacman:" << ":" << posX << ":" << posY;
        cout << "\nghost pos:" << index << ":" << ghostBounds.height << ":" << ghostBounds.left << ":" << ghostBounds.top << ":" << ghostBounds.width;
        cout << "\nPacman pos:" << ":" << pacmanBounds.height << ":" << pacmanBounds.left << ":" << pacmanBounds.top << ":" << pacmanBounds.width;
        cout << "\nIntersection:" << pacmanBounds.intersects(ghostBounds);
        cout << "\nGHOST GRID POS:" << ghost_grid_pos[index].x << ":" << ghost_grid_pos[index].y;
        cout << "\nPacman GRID POS:" << tileX << ":" << tileY;
        if (pacmanBounds.intersects(ghostBounds))
        {
            // Remove the ghost from the window
            ghost_grid_pos[index].x = Default_Ghost[index].x;
            ghost_grid_pos[index].y = Default_Ghost[index].y;
            Key_info[index].is_out = false;
            score += 100;
            // Reset the power pellet effect after 10 seconds
            if (powerPelletClock.getElapsedTime().asSeconds() > 10)
            {
                hasPowerPellet = false;

            }
        }
    }

    void checkCollisionWithGhost(sf::Sprite &ghostSprite, int index)
    {
        // Get the global bounds of Pacman and the ghost sprite
        sf::FloatRect pacmanBounds = currentSprite->getGlobalBounds();
        sf::FloatRect ghostBounds = ghostSprite.getGlobalBounds();
        int tileX = currentSprite->getPosition().x; // Convert player position to tile coordinates
        int tileY = currentSprite->getPosition().x;
        int x2 = Ghost_screen_pos[index].x;
        int y2 = Ghost_screen_pos[index].y;
        pacmanBounds.left += currentSprite->getPosition().x;
        pacmanBounds.top += currentSprite->getPosition().x;
        ghostBounds.left += Ghost_screen_pos[index].x;
        ghostBounds.top += Ghost_screen_pos[index].y;

        // Check if the global bounds intersect
        cout << "\nghost:" << Ghost_screen_pos[index].x << ":" << Ghost_screen_pos[index].y;
        cout << "\nPacman:" << ":" << posX << ":" << posY;
        cout << "\nghost pos:" << index << ":" << ghostBounds.height << ":" << ghostBounds.left << ":" << ghostBounds.top << ":" << ghostBounds.width;
        cout << "\nPacman pos:" << ":" << pacmanBounds.height << ":" << pacmanBounds.left << ":" << pacmanBounds.top << ":" << pacmanBounds.width;
        cout << "\nIntersection:" << pacmanBounds.intersects(ghostBounds);
        cout << "\nGHOST GRID POS:" << ghost_grid_pos[index].x << ":" << ghost_grid_pos[index].y;
        cout << "\nPacman GRID POS:" << tileX << ":" << tileY;

        if (pacmanBounds.intersects(ghostBounds))
        {
            // Collision detected, reduce a life
            // You can also reset Pacman's position or do any other necessary actions here
            posX = 32;
            posY = 32;
            lives--;
        }
    }
    // tileX==ghost_grid_pos[index].x && tileY==ghost_grid_pos[index].y
    void checkAndCollectCoins(int matrix[21][28], int posX, int posY)
    {

        int tileX = posX / 32; // Convert player position to tile coordinates
        int tileY = posY / 32;

        // Check if the player's position corresponds to a coin or power pallet
        if (matrix[tileY][tileX] == 0 || matrix[tileY][tileX] == 2)
        {
            // Update score
            if (matrix[tileY][tileX] == 0)
                score += 10;
            else if (matrix[tileY][tileX] == 2)
            {
                score += 50;
                powerPelletSound.play();    // Play the power pellet sound
                hasPowerPellet = true;      // Set the flag when Pacman eats a power pellet
                powerPelletClock.restart(); // Restart the clock to track the duration of the effect
                matrix[tileY][tileX] = -1;
            }
            // Remove coin or power pallet from the matrix
            matrix[tileY][tileX] = -1;
            coinSound.play(); // Play the coin sound
        }
    }
    bool checkCollision(int matrix[21][28], int pix, std::string dir)
    {
        int nextTileX = posX / 32; // Convert player position to tile coordinates
        int nextTileY = posY / 32;

        // Calculate the next tile coordinates based on the direction
        if (dir == "right")
        {
            nextTileX = (posX + pix + 31) / 32;
        }
        else if (dir == "left")
        {
            nextTileX = (posX - pix) / 32;
        }
        else if (dir == "up")
        {
            nextTileY = (posY - pix) / 32;
        }
        else if (dir == "down")
        {
            nextTileY = (posY + pix + 31) / 32;
        }

        // Check if the next position corresponds to a wall tile
        if (matrix[nextTileY][nextTileX] == 1)
        {
            return true; // Collision detected
        }
        return false; // No collision
    }

    void checkColAndMove(int matrix[21][28], int pix, int windowWidth, int windowHeight)
    {
        checkAndCollectCoins(matrix, posX, posY); // Check and collect coins with the player's position

        if (directionToCheck != "none")
        {
            string dir = getDirection();
            setDirection(directionToCheck);

            // Check collision for the next step
            if (!checkCollision(matrix, pix, directionToCheck))
            {
                move(pix, windowWidth, windowHeight); // Move the player
                directionToCheck = "none";            // Reset direction to check
            }
            else
            {
                directionToCheck = "none"; // Reset direction to check if there's a collision
            }
        }
        else if (checkCollision(matrix, pix, direction))
        {
            // If there's a collision in the current direction, reset direction to none
            setDirection("none");
        }
    }

    void setDirection(std::string dir)
    {
        direction = dir;
        if (dir == "up")
            currentSprite = &upSprite;
        else if (dir == "down")
            currentSprite = &downSprite;
        else if (dir == "left")
            currentSprite = &leftSprite;
        else if (dir == "right")
            currentSprite = &rightSprite;
    }

    string getDirection()
    {
        return direction;
    }

    void chooseDirection(std::string dir)
    {
        if (getDirection() == "none") // pacman not moving
        {
            setDirection(dir);
        }
        else // pacman is moving
        {
            directionToCheck = dir;
        }
    }

    void move(int pix, int windowWidth, int windowHeight)
    {
        int nextPosX = posX;
        int nextPosY = posY;

        if (direction == "right")
        {
            nextPosX += pix;
            if (nextPosX >= windowWidth) // Wrap-around from right to left
                nextPosX = 0;
            else if (nextPosX >= windowWidth - 32) // Prevent going into the wall
                return;
        }
        else if (direction == "left")
        {
            nextPosX -= pix;
            if (nextPosX < 0) // Wrap-around from left to right
                nextPosX = windowWidth - 32;
            else if (nextPosX < 0) // Prevent going into the wall
                return;
        }
        else if (direction == "up")
        {
            nextPosY -= pix;
            if (nextPosY < 0) // Wrap-around from top to bottom
                nextPosY = windowHeight - 32;
            else if (nextPosY < 0) // Prevent going into the wall
                return;
        }
        else if (direction == "down")
        {
            nextPosY += pix;
            if (nextPosY >= windowHeight) // Wrap-around from bottom to top
                nextPosY = 0;
            else if (nextPosY >= windowHeight - 32) // Prevent going into the wall
                return;
        }

        // Update the position
        posX = nextPosX;
        posY = nextPosY;
        currentSprite->setPosition(posX, posY);
    }
};

void *start_game_thread(void *)
{
    // Load textures
    sf::Texture pacUpTexture, pacDownTexture, pacLeftTexture, pacRightTexture;
    pacUpTexture.loadFromFile("img/pacUp.png");
    pacDownTexture.loadFromFile("img/pacDown.png");
    pacLeftTexture.loadFromFile("img/pacLeft.png");
    pacRightTexture.loadFromFile("img/pacRight.png");
    srand(time(0));

    sf::Texture tileTexture;
    tileTexture.loadFromFile("Sprites/tile.png"); // Load the tiles texture

    sf::Texture powerPalletTexture;
    if (!powerPalletTexture.loadFromFile("img/pallet.png"))
    {
        cerr << "Error loading power pallet texture" << endl;
        // return ;
    }

    sf::Font font;
    if (!font.loadFromFile("GrinchedRegular.otf"))
    {
        // Handle font loading error
        std::cerr << "Error loading font" << std::endl;
        // return 1;
    }

    sf::Texture lifeTexture;
    if (!lifeTexture.loadFromFile("img/pacRight.png"))
    {
        // Handle texture loading error
        std::cerr << "Error loading life texture" << std::endl;
        // return 1;
    }

    // Create player and map objects
    Player pacman(pacUpTexture, pacDownTexture, pacLeftTexture, pacRightTexture, 32, 32);
    Map map;

    // Create window
    sf::RenderWindow appWindow(sf::VideoMode(900, 700), "Pacman");
    appWindow.setFramerateLimit(60);

    // Signal that the game has started
    pthread_mutex_lock(&gameMutex);
    gameStarted = true;
    pthread_mutex_unlock(&gameMutex);
    for (int i = 0; i < ghost_count; i++)
    {
        pthread_mutex_init(&Ghost_mutex[i], NULL); // initalize the ghost mutexes
        pthread_mutex_init(&main_mutex[i], NULL);  // initalize the ghost mutexes
        if (Key_info[i].is_out == false)
        {
            if (ghost_color[i] == "Red")
            {
                set_text_ghost_Red('R', i);
            }
            else if (ghost_color[i] == "Blue")
            {
                set_text_ghost_Blue('R', i);
            }
            else if (ghost_color[i] == "Pink")
            {
                set_text_ghost_Pink('R', i);
            }
            else if (ghost_color[i] == "Inky")
            {
                set_text_ghost_Inky('R', i);
            }
            Ghosts[i].setTexture(G_texture[i]);
        }
    }
    // int highScore = 1000; //
    ghost_grid_pos[0] = {7, 10};
    ghost_grid_pos[1] = {7, 12};
    ghost_grid_pos[2] = {7, 14};
    ghost_grid_pos[3] = {7, 16};
    pthread_t G_thr[4];
    // 6,9

    for (int i = 0; i < ghost_count; i++)
    {
        pthread_create(&G_thr[i], NULL, Ghost_thread, (void *)(intptr_t)i);
        pthread_mutex_lock(&Ghost_mutex[i]);
        // usleep(5000);
    }

    Vector2f pos;
    Ghost_escape.restart();

    while (appWindow.isOpen())
    {
        sf::Event event;
        while (appWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                appWindow.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Right)
                    pacman.chooseDirection("right");
                else if (event.key.code == sf::Keyboard::Left)
                    pacman.chooseDirection("left");
                else if (event.key.code == sf::Keyboard::Up)
                    pacman.chooseDirection("up");
                else if (event.key.code == sf::Keyboard::Down)
                    pacman.chooseDirection("down");
            }
        }

        // Move pacman and check collision with walls
        pacman.move(4, appWindow.getSize().x, appWindow.getSize().y);
        pacman.checkColAndMove(matrix, 4, appWindow.getSize().x, appWindow.getSize().y);

        if (lives <= 0)
        {
            appWindow.close(); // Exit the game
        }

        // Clear the window

        appWindow.clear(sf::Color::Black);

        // Draw the map and pacman
        map.generate(appWindow, tileTexture, 0);
        // for collision of ghosts
        pacman.checkCollisionWithGhost(gr[ghost_grid_pos[0].x][ghost_grid_pos[0].x], 0);
        pacman.checkCollisionWithGhost(gr[ghost_grid_pos[1].x][ghost_grid_pos[1].x], 1);
        pacman.checkCollisionWithGhost(gr[ghost_grid_pos[2].x][ghost_grid_pos[2].x], 2);
        pacman.checkCollisionWithGhost(gr[ghost_grid_pos[3].x][ghost_grid_pos[3].x], 3);

        // after eating pallets
        pacman.checkCollisionWithGhostwithPallets(gr[ghost_grid_pos[0].x][ghost_grid_pos[0].x], 0);
        pacman.checkCollisionWithGhostwithPallets(gr[ghost_grid_pos[1].x][ghost_grid_pos[1].x], 1);
        pacman.checkCollisionWithGhostwithPallets(gr[ghost_grid_pos[2].x][ghost_grid_pos[2].x], 2);
        pacman.checkCollisionWithGhostwithPallets(gr[ghost_grid_pos[3].x][ghost_grid_pos[3].x], 3);
        for (int i = 0; i < 4; i++)
        {
            if (pthread_mutex_trylock(&Ghost_mutex[i]) != 0 && Key_info[i].is_out == true)
            {
                // continue;
            }
            // if (Key_info[i].is_out == false)
            // {
            //     Vector2f pos=gr[ghost_grid_pos[i].x][ghost_grid_pos[i].x].getPosition();
            //     gr[ghost_grid_pos[i].x][ghost_grid_pos[i].x].setTexture(G_texture[i]);
            //     gr[ghost_grid_pos[i].x][ghost_grid_pos[i].x].setPosition(pos.x,pos.y);
            //     appWindow.draw(gr[ghost_grid_pos[i].x][ghost_grid_pos[i].x]);
            // }
        }
        pacman.draw(appWindow);
        // func_draw();
        for (int i = 0; i < 21; i++)
        {
            for (int j = 0; j < 28; j++)
            {
                if (pacman.hasPowerPellet)
                {
                    Texture tex1;
                    tex1.loadFromFile("Sprites/G_DEAD_Look/dead_1.png");
                    Sprite special;
                    special.setTexture(tex1);
                    special.setPosition(32 * j, 32 * i);
                    special.setScale(0.7, 0.7);
                    if (i == ghost_grid_pos[0].x && j == ghost_grid_pos[0].y)
                    {
                        /* code */
                        appWindow.draw(special);
                    }

                    if (i == ghost_grid_pos[1].x && j == ghost_grid_pos[1].y)
                    {
                        /* code */
                        appWindow.draw(special);
                    }

                    if (i == ghost_grid_pos[2].x && j == ghost_grid_pos[2].y)
                    {
                        /* code */
                        appWindow.draw(special);
                    }

                    if (i == ghost_grid_pos[3].x && j == ghost_grid_pos[3].y)
                    {
                        /* code */
                        appWindow.draw(special);
                    }
                }
                else
                {
                    appWindow.draw(gr[i][j]);
                }
            }
        }
        sf::Text highScoreText("Score: " + std::to_string(score), font, 24);
        highScoreText.setFillColor(sf::Color::Red);
        highScoreText.setPosition(appWindow.getSize().x - 200, appWindow.getSize().y - 30);
        appWindow.draw(highScoreText);

        for (int i = 0; i < lives; ++i)
        {
            sf::Sprite lifeSprite(lifeTexture);
            lifeSprite.setScale(2.0f, 2.0f);                            // Set the size of the texture
            lifeSprite.setPosition(i * 64, appWindow.getSize().y - 50); // Adjust position
            appWindow.draw(lifeSprite);
        }

        // Display
        appWindow.display();
        for (int i = 0; i < 4; i++)
        {
            pthread_mutex_unlock(&main_mutex[i]);
        }
    }
    
    return NULL;
}

void start_game()
{
    pthread_t gameThread;
    pthread_create(&gameThread, NULL, start_game_thread, NULL);

    // Wait for the game to start
    pthread_mutex_lock(&gameMutex);
    while (!gameStarted)
    {
        pthread_mutex_unlock(&gameMutex);
        pthread_mutex_lock(&gameMutex);
    }
    pthread_mutex_unlock(&gameMutex);

    pthread_join(gameThread, NULL);
}
void set_text_ghost_Red(char move, int index)
{
    string filename;
    if (move == 'R')
        filename = "Sprites/G_red/red_down.png";
    else if (move == 'L')
        filename = "Sprites/G_red/red_up.png";
    else if (move == 'U')
        filename = "Sprites/G_red/red_left.png";
    else if (move == 'D')
        filename = "Sprites/G_red/red_right.png";
    else
        cout << "Invalid move direction red";

    if (!G_texture[index].loadFromFile(filename))
    {
        cout << "Error loading texture" << endl;
    }
    return;
}

void set_text_ghost_Blue(char move, int index)
{
    if (move == 'R')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Blue/blue_down.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'L')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Blue/blue_up.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'U')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Blue/blue_left.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'D')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Blue/blue_right.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else
        cout << "Invalid move direction blue";
    return;
}

void set_text_ghost_Pink(char move, int index)
{
    if (move == 'R')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_right.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'L')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_left.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'U')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_up.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'D')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_down.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else
        cout << "Invalid move direction pink";
    return;
}

void set_text_ghost_Inky(char move, int index)
{
    if (move == 'R')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_right.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'L')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_left.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'U')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_up.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'D')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_down.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else
        cout << "Invalid move direction Inky";
    return;
}