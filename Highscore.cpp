#include "Highscore.h"

Highscore::Highscore()
{
    name = "";
    score = 0;
    index = 0;
    // sp;
    bg.loadFromFile("img/level3.png");
    sp.setTexture(bg);
    sp.setScale(1, 1); // 564*1002
}

void Highscore::addscore(int score, const std::string& name)
{
    ifstream read("highscore.txt");
    ofstream write("temp.txt");
    int count = 0;
    string str;
    bool inserted = false;

    if (!read)
    {
        cerr << "Failed to open input file." << endl;
        return;
    }

    if (!write)
    {
        cerr << "Failed to open output file." << endl;
        return;
    }

    while (getline(read, str))
    {
        string file_name;
        int file_score = 0;

        bool isScore = true;
        for (char c : str)
        {
            if (c == ' ')
            {
                isScore = false;
                continue;
            }

            if (isScore)
            {
                file_score = file_score * 10 + (c - '0');
            }
            else
            {
                file_name += c;
            }
        }

        if (file_score < score && count < 3 && !inserted)
        {
            write << score << " " << name << "\n";
            count++;
            inserted = true;
        }

        if (count < 3 && !(file_score == score && file_name == name))
        {
            write << file_score << " " << file_name << "\n";
            count++;
        }
    }

    if (count < 3 && !inserted)
    {
        write << score << " " << name << "\n";
    }

    read.close();
    write.close();

    if (remove("highscore.txt") != 0)
    {
        cerr << "Failed to remove the input file." << endl;
        return;
    }

    if (rename("temp.txt", "highscore.txt") != 0)
    {
        cerr << "Failed to rename the output file." << endl;
    }
}


// void Highscore::addscore(int score, string name)
// {
//     ifstream read("highscore.txt");
//     ofstream write("temp.txt");
//     int count = 0;

//     string str;

//     while (getline(read, str))
//     {
//         string file_name= str.substr(0, str.find("\t"));
//         int file_score= stoi(str.substr(str.find("\t") + 1));
//         cout << file_name << "\t" << file_score << endl;

//         if (file_score < score)
//         {
//             write << score << "\t" << name << "\n";
//             count++;
//         }

//         if (count < 3)
//         {
//             write << file_score << "\t" << file_name << "\n";
//             count++;
//         }
//     }
//     read.close();
//     write.close();

//     remove("highscore.txt");
//     rename("temp.txt", "highscore.txt");
// }

void Highscore::displayhighscore()
{
    fstream read("highscore.txt");
    string str;
    string highscores[3];
    int count = 0;

    while (getline(read, str) && count < 3)
    {
        cout << str << endl;
        highscores[count] = str;
        count++;
    }
    Text text1, text2, text3;
    Font font;
    font.loadFromFile("animal-silence/Animal Silence.otf");
    text1.setFont(font);
    text1.setFillColor(Color::Green);
    text1.setCharacterSize(50);
    text1.setString(highscores[0]);
    text1.setPosition(200, 250);
    text2.setFont(font);
    text2.setFillColor(Color::Blue);
    text2.setCharacterSize(50);
    text2.setString(highscores[1]);
    text2.setPosition(200, 380);
    text3.setFont(font);
    text3.setFillColor(Color::Magenta);
    text3.setCharacterSize(50);
    text3.setString(highscores[2]);
    text3.setPosition(200, 520);
    //Window window1;
    // RenderWindow.window1
    RenderWindow window1(VideoMode(564, 1002), "HIGH-SCORE");
    while (window1.isOpen())
    {
        Event e;
        while (window1.pollEvent(e))
        {
            if (e.type == Event::Closed) // If cross/close is clicked/pressed
                window1.close();         // close the game
        }
        window1.clear(Color::Black); // clears the screen
        window1.draw(sp);          // setting background
        window1.draw(text1);          // setting background
        window1.draw(text2);          // setting background
        window1.draw(text3);          // setting background
        window1.display();           // Displying all the sprites
    }
    read.close();
}
Highscore::~Highscore()
{
}