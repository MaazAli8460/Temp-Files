#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
using namespace std;
using namespace sf;


// Mutex to synchronize access to menuFinished flag
extern pthread_mutex_t menuMutex;
extern bool menuFinished;
void display_menu()

{


        // Create the menu window
        sf::RenderWindow window(sf::VideoMode(800, 800), "Menu");


		  sf::Texture background_texture;
        if (!background_texture.loadFromFile("img/background.jpeg")) {
            // Handle error loading texture
            cout<<"Error menu "<<endl<<endl;
        }

        // Create the background sprite and set its texture
sf::Sprite background_sprite(background_texture);

// Set the scale of the background sprite to match the window size
background_sprite.setScale(window.getSize().x / background_sprite.getLocalBounds().width, 
                            window.getSize().y / background_sprite.getLocalBounds().height);

background_sprite.setPosition(0, 0);

        
        // Define the menu options
        sf::Font font;
        font.loadFromFile("GrinchedRegular.otf"); 
	
		// makes the button of start_select game
        sf::Text start_opt;
        start_opt.setFont(font);
        start_opt.setString("Start Game");
        start_opt.setCharacterSize(100);
        start_opt.setFillColor(sf::Color::White);
        start_opt.setPosition(80, 150);

		sf::Text inst_opt;
        inst_opt.setFont(font);
        inst_opt.setString("Instructions");
        inst_opt.setCharacterSize(100);
        inst_opt.setFillColor(sf::Color::White);
        inst_opt.setPosition(80, 300);

		//makes the button of quit
        sf::Text quit_opt;
        quit_opt.setFont(font);
        quit_opt.setString("Quit");
        quit_opt.setCharacterSize(100);
        quit_opt.setFillColor(sf::Color::White);
        quit_opt.setPosition(80, 450);

   
        bool start_select = true; // Indicates whether "Start Game" is currently selected
  		bool inst_select=false;
  		// bool high_select=false;
  		bool quit_select=false;
  // Loop until the window is closed
while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                if (start_select) {
                    quit_select = true;
                    start_select = false;
                } else if (inst_select) {
                    start_select = true;
                    inst_select = false;
                }else{
                	inst_select=true;
                	quit_select=false;
                }
             
            } else if (event.key.code == sf::Keyboard::Down) {
                if (start_select) {
                    inst_select = true;
                    start_select = false;
                } else if (inst_select) {
                    quit_select = true;
                    inst_select = false;
                } 
                else{
                	start_select=true;
                	quit_select=false;
                }
            }
           else if (event.key.code == sf::Keyboard::Enter) {
                if (start_select) {
                       pthread_mutex_lock(&menuMutex);
            menuFinished = true;
            pthread_mutex_unlock(&menuMutex);
            return;
                 
                }
        else if (inst_select) {
            // Open the instruction window
            sf::RenderWindow instructionwindow(sf::VideoMode(800, 600), "Instructions");
            bool instwindowopen = true;
            sf::Texture backgroundTexture1;
            if (!backgroundTexture1.loadFromFile("img/back2.png"))
            {
            cout << "Error background" << endl<<endl;
            // return ; 
            }
            
            sf::Sprite backgroundSprite(backgroundTexture1);
            backgroundSprite.setScale(window.getSize().x / backgroundSprite.getLocalBounds().width, 
                            window.getSize().y / backgroundSprite.getLocalBounds().height);

backgroundSprite.setPosition(0, 0);
            sf::Text instruction;
            instruction.setFont(font);
            instruction.setCharacterSize(24);
            instruction.setString("\n     Game Instructions:\n\nArrow keys: Use the arrow keys (up, down, left, right) to control \nPacman's movement.\nEat all the dots or pellets scattered throughout the maze.\nEach dot or pellet eaten increases your score.\nGhosts roam the maze, and if they touch Pacman, you lose a life.\nYou have a limited number of lives.\nLosing all lives ends the game.");

            instruction.setPosition(100, 100);

            while (instwindowopen) {
                sf::Event instructionEvent;
                while (instructionwindow.pollEvent(instructionEvent)) {
                    if (instructionEvent.type == sf::Event::Closed) {
                        instructionwindow.close();
                        instwindowopen = false;
                    }
                    else if (instructionEvent.type == sf::Event::KeyPressed) {
                        if (instructionEvent.key.code == sf::Keyboard::Escape) {
                            instructionwindow.close();
                            instwindowopen = false;
                        }
                    }
                }

                instructionwindow.clear();
                instructionwindow.draw(backgroundSprite);
                instructionwindow.draw(instruction);
                instructionwindow.display();
            }
        }
                
                else {
                    // window.close();
                    exit(0);
                }
            }
        }
    }// window poll event ends

    // Update the appearance of the options based on the currently selected option
    if (start_select) {
        start_opt.setFillColor(sf::Color::Red);
        inst_opt.setFillColor(sf::Color::White);
        quit_opt.setFillColor(sf::Color::White);
    }
     else if (inst_select) {
        start_opt.setFillColor(sf::Color::White);
        inst_opt.setFillColor(sf::Color::Red);
        quit_opt.setFillColor(sf::Color::White);
    }

    else if (quit_select) {
        start_opt.setFillColor(sf::Color::White);
        inst_opt.setFillColor(sf::Color::White);
        quit_opt.setFillColor(sf::Color::Red);
    }

    // Clear the window and draw the options
    window.clear();
    window.draw(background_sprite);
    window.draw(start_opt);
    window.draw(inst_opt);
    window.draw(quit_opt);
    // window.draw(high_opt);
    window.display();
}//while window is open ends
    
}// display menu ends

