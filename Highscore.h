#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#pragma once

#include <SFML/Graphics.hpp>
#include<string>
#include <fstream>
#include <cstring>
//#include <SFML/Graphics.hpp>
#include<iostream>

using namespace std;
using namespace sf;
class Highscore
{
public:
    string name;
    int score;
    int index;
    Window ref;
    Sprite sp;
    Texture bg;
    fstream file;


    Highscore();
    void displayhighscore();
    void addscore(int score, const std::string& name);
    ~Highscore();


};

#endif