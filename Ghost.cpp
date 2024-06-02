#include <SFML/Graphics.hpp>
#include <time.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>

using namespace std;
using namespace sf;
// using namespace Color;
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
int matrix[21][28] =

    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1}, // 5,12
        {1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

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

    while (!Key_info[index].is_out)
    {

        if (pthread_mutex_trylock(&key_mutex) == 0)
        {
            cout<<"\nwait:"<<index;
            continue;
        }
        else
        {
            cout<<"\nNO_wait:"<<index;
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
    // sleep(index * 2);
    Quarintine(index);
    Sprite sp;

    char move = 'L';
    int move_index = 0;
    int previous_index = 0;
    int move_count = 2;
    Clock ghost_delay;
    float move_pass = 0.5;
    ghost_delay.restart();
    gr[ghost_grid_pos[index].x][ghost_grid_pos[index].y] = sp;
    ghost_grid_pos[index].x = 5;
    ghost_grid_pos[index].y = 14;
    while (true)
    {

        if (var == 1) // temporary shutdown
        {
            break;
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
            if (ghost_delay.getElapsedTime().asSeconds() >= 0.1)
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
            if (ghost_delay.getElapsedTime().asSeconds() >= 0.1)
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
int main()
{
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

    srand(time(0));
    RenderWindow window(VideoMode(1280, 1024), title);
    // sleep(10);
    Sprite sp; // right+
    Texture tex;
    if (!tex.loadFromFile("Sprites/G_red/red_up.png"))
    {
        cout << "Error loading texture" << endl;
    }
    pthread_t G_thr[4];
    Ghost_escape.restart();
    // 6,9
    ghost_grid_pos[0] = {7, 10};
    ghost_grid_pos[1] = {7, 12};
    ghost_grid_pos[2] = {7, 14};
    ghost_grid_pos[3] = {7, 16};
    func_draw();

    for (int i = 0; i < ghost_count; i++)
    {
        pthread_create(&G_thr[i], NULL, Ghost_thread, (void *)(intptr_t)i);
        pthread_mutex_lock(&Ghost_mutex[i]);
        // usleep(5000);
    }

    Vector2f pos;
    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) // If cross/close is clicked/pressed
                window.close();          // close the game

            if (e.mouseButton.button == sf::Mouse::Right)
            {
                // Get the mouse position
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                std::cout << "Left mouse button pressed at: (" << mousePosition.x << ", " << mousePosition.y << ")" << std::endl;
            }
        }

        window.clear();

        for (int i = 0; i < 4; i++)
        {
            if (pthread_mutex_trylock(&Ghost_mutex[i]) != 0 && Key_info[i].is_out == true)
            {
                // continue;
            }
            
        }
        // sp.setScale(0.7, 0.7);
        func_draw();

        for (int i = 0; i < 21; i++)
        {
            for (int j = 0; j < 28; j++)
            {

                window.draw(gr[i][j]);
            }
        }

        window.display(); // Displying all the sprites
        for (int i = 0; i < 4; i++)
        {
            pthread_mutex_unlock(&main_mutex[i]);
        }
    }
    var = 1;
    return 0;
}

void func_draw()
{
    int x = 720 / 2;
    int y = 1000 / 2;
    // x -= (41 * 3);
    // y -= (41 * 3);
    x = 10;
    y = 10;

    if (!Gr_tile.loadFromFile("Sprites/tile.png"))
    {
        cout << "\nError";
    }
    for (int i = 0; i < 21; i++)
    {
        //(sf::Vector2f(41, 41));

        for (int j = 0; j < 28; j++)
        {
            if (matrix[i][j] == 1)
            {
                gr[i][j].setTexture(Gr_tile);
                gr[i][j].setPosition(x, y);
            }
            else if (matrix[i][j] == 0)
            {
                if (i == ghost_grid_pos[0].x && j == ghost_grid_pos[0].y)
                {
                    /* code */
                    gr[ghost_grid_pos[0].x][ghost_grid_pos[0].y].setTexture(G_texture[0]);
                    gr[ghost_grid_pos[0].x][ghost_grid_pos[0].y].setScale(0.7, 0.7);
                    gr[i][j].setPosition(x, y);
                }

                if (i == ghost_grid_pos[1].x && j == ghost_grid_pos[1].y)
                {
                    /* code */
                    gr[ghost_grid_pos[1].x][ghost_grid_pos[1].y].setTexture(G_texture[1]);
                    gr[ghost_grid_pos[1].x][ghost_grid_pos[1].y].setScale(0.7, 0.7);
                    gr[i][j].setPosition(x, y);
                }

                if (i == ghost_grid_pos[2].x && j == ghost_grid_pos[2].y)
                {
                    /* code */
                    gr[ghost_grid_pos[2].x][ghost_grid_pos[2].y].setTexture(G_texture[2]);
                    gr[ghost_grid_pos[2].x][ghost_grid_pos[2].y].setScale(0.7, 0.7);
                    gr[i][j].setPosition(x, y);
                }

                if (i == ghost_grid_pos[3].x && j == ghost_grid_pos[3].y)
                {
                    /* code */
                    gr[ghost_grid_pos[3].x][ghost_grid_pos[3].y].setTexture(G_texture[3]);
                    gr[ghost_grid_pos[3].x][ghost_grid_pos[3].y].setScale(0.7, 0.7);
                    gr[i][j].setPosition(x, y);
                }
            }

            x += 41; // 28.7;
        }
        y += 41; // 28.7;
        x = 10;
    }
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
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_down.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'L')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_up.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'U')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_left.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'D')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Pink/pink_right.png"))
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
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_down.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'L')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_up.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'U')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_left.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else if (move == 'D')
    {
        if (!G_texture[index].loadFromFile("Sprites/G_Inky/inky_right.png"))
        {
            cout << "Error loading texture" << endl;
        }
    }
    else
        cout << "Invalid move direction Inky";
    return;
}