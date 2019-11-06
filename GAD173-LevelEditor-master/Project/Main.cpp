#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <list>
#include <functional>
#include "arial.h"

using namespace std;

//Setup window size
const int windowWidth = 1025;
const int windowHeight = 650;

//Grid amounts
const int x = 30;
const int y = 20;

//Mouse Pos
sf::Vector2f worldPos;

//Track if painting with actor or tile
bool actorNotTile = false;

//Actor class, it's drawable and has a transform
class Actor : public sf::Drawable, public sf::Transformable
{
private: 
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(sprite, states);
	}
public:
	sf::Sprite sprite;

	enum Type {None, Enemy, Coin, Player, Trap, Exit};
	Type type = None;

	sf::Texture doorTexture;
	sf::Texture coinTexture;
	sf::Texture enemyAliveTexture;
	sf::Texture enemyDeadTexture;
	sf::Texture playerTexture;
	sf::Texture trapTexture;
	sf::Texture blankTexture;

	Actor()
	{

	}

	void loadTextures()
	{
		if (!blankTexture.loadFromFile("SpritesPlatformer/None.png"))
		{
			cout << "Failed to load None.png";
		}
		if (!coinTexture.loadFromFile("SpritesPlatformer/Coin.png"))
		{
			cout << "Failed to load Coin.png";
		}
		if (!doorTexture.loadFromFile("SpritesPlatformer/Door.png"))
		{
			cout << "Failed to load Door.png";
		}
		if (!enemyAliveTexture.loadFromFile("SpritesPlatformer/EnemyAlive.png"))
		{
			cout << "Failed to load EnemyAlive.png";
		}
		if (!enemyDeadTexture.loadFromFile("SpritesPlatformer/EnemyDead.png"))
		{
			cout << "Failed to load EnemyDead.png";
		}
		if (!playerTexture.loadFromFile("SpritesPlatformer/Player.png"))
		{
			cout << "Failed to load Player.png";
		}
		if (!trapTexture.loadFromFile("SpritesPlatformer/Trap.png"))
		{
			cout << "Failed to load Trap.png";
		}
	}

	void init(int x, int y)
	{
		loadTextures();
		sprite.setPosition(x, y);
		RefreshActor();
	}

	void RefreshActor()
	{
		switch (type)
		{
		case Player:
			sprite.setTexture(playerTexture);
			break;
		case Enemy:
			sprite.setTexture(enemyAliveTexture);
			break;
		case Coin:
			sprite.setTexture(coinTexture);
			break;
		case Exit:
			sprite.setTexture(doorTexture);
			break;
		case Trap:
			sprite.setTexture(trapTexture);
			break;
		case None:
			sprite.setTexture(blankTexture);
			break;
		default:
			sprite.setTexture(blankTexture);
			break;
		}
	}

	void ChangeActor(Type t)
	{
		type = t;
		RefreshActor();
	}
};

//Tile class, it's drawable and has a transform
class Tile : public sf::Drawable, public sf::Transformable
{
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(sprite, states);
		target.draw(actor, states);
	}
public:
	enum Type
	{
		Sky,
		Platform,
		Lava
	};

	Actor actor;

	//bool isGroundTile = false;

	//Setting tile to be Sky
	Type type = Tile::Sky;

	sf::Sprite sprite;
	sf::FloatRect rBounds;

	//Define Textures
	sf::Texture blockSkyTexture;
	sf::Texture blockPlatformTexture;
	sf::Texture blockLavaTexture;

	void init(int x, int y)
	{
		//Load textures
		loadTextures();

		//Set the position of this tiles sprite
		sprite.setPosition(x, y);
		actor.init(x, y);
		refreshTile();
	}

	void loadTextures()
	{
		if (!blockSkyTexture.loadFromFile("SpritesPlatformer/BlockSky.png"))
		{
			cout << "Failed to load BlockSky.png";
		}
		if (!blockPlatformTexture.loadFromFile("SpritesPlatformer/BlockPlatform.png"))
		{
			cout << "Failed to load BlockPlatform.png";
		}
		if (!blockLavaTexture.loadFromFile("SpritesPlatformer/BlockLava.png"))
		{
			cout << "Failed to load BlockLava.png";
		}
	}

	bool mouseOver(sf::Vector2f windPos)
	{
		rBounds = sprite.getGlobalBounds();
		if (rBounds.contains(windPos.x, windPos.y))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void refreshTile()
	{
		switch (type)
		{
		case Type::Sky:
			sprite.setTexture(blockSkyTexture);
			break;
		case Type::Platform:
			sprite.setTexture(blockPlatformTexture);
			break;
		case Type::Lava:
			sprite.setTexture(blockLavaTexture);
			break;
		}
		actor.RefreshActor();
	}

	//Direct call to change Actor - Saves time using tile.changeactor instead of tile.actor.changeactor
	void ChangeActor(Actor::Type a)
	{
		actor.ChangeActor(a);
	}
	void ChangeType(Type t)
	{
		type = t;
	}
};

//Button class, it's drawable and has a transform
class Button : public sf::Drawable, public sf::Transformable
{
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(rect, states);
		target.draw(text, states);
	}
public:
	int textSize = 16;
	float rectOutline = 1;
	sf::RectangleShape rect;
	sf::Font font;
	sf::Text text;
	sf::FloatRect rbounds;

	//Init
	Button()
	{
		font.loadFromMemory(&arial_ttf, arial_ttf_size);
		text.setFont(font);
		text.setCharacterSize(textSize);
		text.setFillColor(sf::Color::Black);
		text.setStyle(sf::Text::Regular);

		//Set up button visuals
		rect.setFillColor(sf::Color::White);
		rect.setOutlineColor(sf::Color::Black);
		rect.setOutlineThickness(rectOutline);
	}

	bool mouseOver(sf::Vector2f windPos)
	{
		rbounds = rect.getGlobalBounds();
		if (rbounds.contains(windPos.x, windPos.y))
		{
			rect.setOutlineThickness(2);
			return true;
		}
		else
		{
			rect.setOutlineThickness(rectOutline);
			return false;
		}
	}

	//Check to see if button has been clicked,
	//If so, run function passed as an argument
	void checkClick(function<void()>f, sf::Vector2f windPos)
	{
		if (mouseOver(windPos) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			f();
		}
	}
};

//ToolPanel class, it's drawable and has a transform
class ToolPanel : public sf::Drawable, public sf::Transformable
{
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(loadButton, states);
		target.draw(saveButton, states);
	}
public:
	Button loadButton;
	Button saveButton;

	//Init
	ToolPanel()
	{
		loadButton.text.setString("Load");
		loadButton.rect.setSize(sf::Vector2f(loadButton.text.getLocalBounds().width + 6,
			loadButton.text.getLocalBounds().height + 6));
		loadButton.rect.setPosition(2, 20);
		loadButton.text.setPosition(loadButton.rect.getPosition().x +
			(loadButton.rect.getSize().x / 2) - (loadButton.text.getLocalBounds().width / 2),
			loadButton.rect.getPosition().y);

		saveButton.text.setString("Save");
		saveButton.rect.setSize(sf::Vector2f(saveButton.text.getLocalBounds().width + 6,
			saveButton.text.getLocalBounds().height + 6));
		saveButton.rect.setPosition(2, 45);
		saveButton.text.setPosition(saveButton.rect.getPosition().x +
			(saveButton.rect.getSize().x / 2) - (saveButton.text.getLocalBounds().width / 2),
			saveButton.rect.getPosition().y);
	}
};

//Saving
void save(Tile tile[x][y])
{
	//Writing to a text file
	ofstream myfile("save.sav");
	list<sf::Vector2i> coinPos;
	list<sf::Vector2i> enemyPos;
	list<sf::Vector2i> trapPos;
	sf::Vector2i exitPos;
	sf::Vector2i playerPos;

	if (myfile.is_open())
	{
		cout << "Saving Tiles \n";
		for (int i = 0; i < x; i++)
		{
			for (int j = 0; j < y; j++)
			{
				switch (tile[i][j].type)
				{
				case Tile::Type::Sky:
					myfile << "0";
					break;
				case Tile::Type::Platform:
					myfile << "1";
					break;
				case Tile::Type::Lava:
					myfile << "2";
					break;
				}
				myfile << ",";

				switch (tile[i][j].actor.type)
				{
				case Actor::Type::Coin:
					coinPos.push_back(sf::Vector2i(i, j));
					break;
				case Actor::Type::Enemy:
					enemyPos.push_back(sf::Vector2i(i, j));
					break;
				case Actor::Type::Trap:
					trapPos.push_back(sf::Vector2i(i, j));
					break;
				case Actor::Type::Player:
					playerPos = sf::Vector2i(i, j);
					break;
				case Actor::Type::Exit:
					exitPos = sf::Vector2i(i, j);
					break;
				}
			}
			myfile << "\n";
		}

		//Loop through all the lists, add them to save file, then add player and exit pos
		//coins
		if (!coinPos.empty())
		{
			cout << "Saving Coins \n";
			myfile << "Coin";
			list<sf::Vector2i>::iterator cIt;
			for (cIt = coinPos.begin(); cIt != coinPos.end(); cIt++)
			{
				//Access through the iterator
				sf::Vector2i curCoinPos = sf::Vector2i(cIt->x, cIt->y);

				//Print contents
				cout << "Saving coin at: " << curCoinPos.x << ',' << curCoinPos.y << "\n";

				//Save to file
				myfile << '(' << curCoinPos.x << ',' << curCoinPos.y << ')';
			}
			myfile << "\n";
		}

		//Enemies
		if (!enemyPos.empty())
		{
			cout << "Saving Enemies \n";
			myfile << "Enemy";
			list<sf::Vector2i>::iterator eIt;
			for (eIt = enemyPos.begin(); eIt != enemyPos.end(); eIt++)
			{
				//Access through the iterator
				sf::Vector2i curEnemyPos = sf::Vector2i(eIt->x, eIt->y);

				//Print contents
				cout << "Saving enemy at: " << curEnemyPos.x << ',' << curEnemyPos.y << "\n";

				//Save to file
				myfile << '(' << curEnemyPos.x << ',' << curEnemyPos.y << ')';
			}
			myfile << "\n";
		}

		//Traps
		if (!trapPos.empty())
		{
			cout << "Saving Traps \n";
			myfile << "Trap";
			list<sf::Vector2i>::iterator tIt;
			for (tIt = trapPos.begin(); tIt != trapPos.end(); tIt++)
			{
				//Access through the iterator
				sf::Vector2i curTrapPos = sf::Vector2i(tIt->x, tIt->y);

				//Print contents
				cout << "Saving trap at: " << curTrapPos.x << ',' << curTrapPos.y << "\n";

				//Save to file
				myfile << '(' << curTrapPos.x << ',' << curTrapPos.y << ')';
			}
			myfile << "\n";
		}

		//Saving player
		cout << "Saving Player at: " << playerPos.x << ',' << playerPos.y << "\n";
		myfile << "Player" << '(' << playerPos.x << ',' << playerPos.y << ')' << "\n";

		//Saving Exit
		cout << "Saving Exit at: " << exitPos.x << ',' << exitPos.y << "\n";
		myfile << "Exit" << '(' << exitPos.x << ',' << exitPos.y << ')' << "\n";

		myfile.close();
		cout << "File Saved \n";
	}
	else
	{
	cout << "Can't open save file \n";
	}
}

//Loading
void load(Tile tile[x][y])
{
	string line;
	ifstream myfile("save.sav");
	if (myfile.is_open())
	{
		int a = 0;
		int b = 0;
		string saveHold;
		while (getline(myfile, line))
		{
			if (b == 0)
			{
				cout << "Loading Tiles \n";
			}
			if (b < x)
			{
				for (int i = 0; i < line.size(); i++)
				{
					switch (line[i])
					{
					case ',':
						a += 1;
						break;
					case '0':
						tile[b][a].ChangeType(tile[a][b].Type::Sky);
						break;
					case '1':
						tile[b][a].ChangeType(tile[a][b].Type::Platform);
						break;
					case '2':
						tile[b][a].ChangeType(tile[a][b].Type::Lava);
						break;
					}
					tile[b][a].refreshTile();
				}
			}
			else if (b >= x)
			{
				//Check the first two letter on lines after the tiles are loaded
				//Begin loading Actors
				string lineHold = line;
				int curStart;
				int curEnd = 0;
				string posString;

				if (line[0] == 'C' && line[1] == 'o')
				{
					cout << "Loading Coins \n";
					while (curEnd < lineHold.length() && lineHold.find('(') != string::npos)
					{
						curStart = lineHold.find('(');
						curEnd = lineHold.find(')');

						posString = lineHold.substr(curStart + 1, curEnd - (curStart + 1));
						cout << "Loaded Coin at: " << posString << "\n";
						string xStr = posString.substr(0, posString.find(','));
						string yStr = posString.substr(posString.find(',') + 1, posString.length() - 1);
						cout << "x = " << xStr << ", y = " << yStr << "\n";
						tile[stoi(xStr)][stoi(yStr)].actor.ChangeActor(Actor::Type::Coin);

						lineHold[curStart] = '<';
						lineHold[curEnd] = '>';
					}
				}
				else if (line[0] == 'E' && line[1] == 'n')
				{
					cout << "Loading Enemies \n";
					while (curEnd < lineHold.length() && lineHold.find('(') != string::npos)
					{
						curStart = lineHold.find('(');
						curEnd = lineHold.find(')');

						posString = lineHold.substr(curStart + 1, curEnd - (curStart + 1));
						cout << "Loaded Enemy at: " << posString << "\n";
						string xStr = posString.substr(0, posString.find(','));
						string yStr = posString.substr(posString.find(',') + 1, posString.length() - 1);
						cout << "x = " << xStr << ", y = " << yStr << "\n";
						tile[stoi(xStr)][stoi(yStr)].actor.ChangeActor(Actor::Type::Enemy);

						lineHold[curStart] = '<';
						lineHold[curEnd] = '>';
					}
				}
				else if (line[0] == 'T' && line[1] == 'r')
				{
					cout << "Loading Traps \n";
					while (curEnd < lineHold.length() && lineHold.find('(') != string::npos)
					{
						curStart = lineHold.find('(');
						curEnd = lineHold.find(')');

						posString = lineHold.substr(curStart + 1, curEnd - (curStart + 1));
						cout << "Loaded Trap at: " << posString << "\n";
						string xStr = posString.substr(0, posString.find(','));
						string yStr = posString.substr(posString.find(',') + 1, posString.length() - 1);
						cout << "x = " << xStr << ", y = " << yStr << "\n";
						tile[stoi(xStr)][stoi(yStr)].actor.ChangeActor(Actor::Type::Trap);

						lineHold[curStart] = '<';
						lineHold[curEnd] = '>';
					}
				}
				else if (line[0] == 'E' && line[1] == 'x')
				{
					cout << "Loading Exit \n";
					while (curEnd < lineHold.length() && lineHold.find('(') != string::npos)
					{
						curStart = lineHold.find('(');
						curEnd = lineHold.find(')');

						posString = lineHold.substr(curStart + 1, curEnd - (curStart + 1));
						cout << "Loaded Exit at: " << posString << "\n";
						string xStr = posString.substr(0, posString.find(','));
						string yStr = posString.substr(posString.find(',') + 1, posString.length() - 1);
						cout << "x = " << xStr << ", y = " << yStr << "\n";
						tile[stoi(xStr)][stoi(yStr)].actor.ChangeActor(Actor::Type::Exit);

						lineHold[curStart] = '<';
						lineHold[curEnd] = '>';
					}
				}
				else if (line[0] == 'P' && line[1] == '1')
				{
					cout << "Loading Player \n";
					while (curEnd < lineHold.length() && lineHold.find('(') != string::npos)
					{
						curStart = lineHold.find('(');
						curEnd = lineHold.find(')');

						posString = lineHold.substr(curStart + 1, curEnd - (curStart + 1));
						cout << "Loaded Player at: " << posString << "\n";
						string xStr = posString.substr(0, posString.find(','));
						string yStr = posString.substr(posString.find(',') + 1, posString.length() - 1);
						cout << "x = " << xStr << ", y = " << yStr << "\n";
						tile[stoi(xStr)][stoi(yStr)].actor.ChangeActor(Actor::Type::Player);

						lineHold[curStart] = '<';
						lineHold[curEnd] = '>';
					}
				}
			}
			b += 1;
			a = 0;
		}
		myfile.close();
		cout << "File Loaded \n";
	}
	else
	{
		cout << "Unable to open file \n";
	}
}

//Printing out tiles
void PrintToConsole(Tile tile[x][y])
{
	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			switch (tile[i][j].type)
			{
			case tile[j][i].Type::Sky:
				cout << "0";
				break;
			case tile[j][i].Type::Platform:
				cout << "1";
				break;
			case tile[j][i].Type::Lava:
				cout << "2";
				break;
			}
			cout << ",";
		}
		cout << "\n";
	}
	cout << "Success \n";
}

int main()
{
	//Setup space for toolbar
	ToolPanel tools;
	sf::View toolsView(sf::FloatRect(0, 0.f, windowWidth * 0.045f, windowHeight));
	toolsView.setViewport(sf::FloatRect(0, 0, 0.045f, 1));

	sf::View levelEditView(sf::FloatRect(0, 0.f, windowWidth, windowHeight));
	levelEditView.setViewport(sf::FloatRect(0.03f, 0, 1, 1));

	//Setup Window
	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Level Editor", sf::Style::Titlebar | sf::Style::Close);

	//Setup variable to track selected tile
	Tile::Type curTileType = Tile::Type::Platform;
	Actor::Type curActorType = Actor::Type::Coin;

	//Setup tiles to select and paint with
	Tile tileButton[9];
	for (int i = 0; i < 9; i++)
	{
		tileButton[i].init(10, (32 + 5) * i + 150);
	}
	tileButton[0].type = Tile::Type::Sky;
	tileButton[0].refreshTile();
	tileButton[1].type = Tile::Type::Platform;
	tileButton[1].refreshTile();
	tileButton[2].type = Tile::Type::Lava;
	tileButton[2].refreshTile();
	tileButton[3].actor.type = Actor::Type::Coin;
	tileButton[3].refreshTile();
	tileButton[4].actor.type = Actor::Type::Enemy;
	tileButton[4].refreshTile();
	tileButton[5].actor.type = Actor::Type::Exit;
	tileButton[5].refreshTile(); 
	tileButton[6].actor.type = Actor::Type::Player;
	tileButton[6].refreshTile();
	tileButton[7].actor.type = Actor::Type::Trap;
	tileButton[7].refreshTile();
	tileButton[8].actor.type = Actor::Type::None;
	tileButton[8].refreshTile();

	//Setup Tiles
	Tile tile[x][y];

	//Init Tiles
	for (int i = 0; i < x; i++) //Rows
	{
		for (int j = 0; j < y; j++) //Columns
		{
			tile[i][j].init(i * 32 + ((windowWidth / 2) - ((32 * x) / 2)), j * 32);
		}
	}

	//Game Loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			window.close();
			break;

			//Cycle for current tile
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
					//Pressed T
				case sf::Keyboard::T:
					switch (curTileType)
					{
					case Tile::Type::Sky:
						curTileType = Tile::Type::Platform;
						break;
					case Tile::Type::Platform:
						curTileType = Tile::Type::Lava;
						break;
					case Tile::Type::Lava:
						curTileType = Tile::Type::Sky;
						break;
					}
					break;
				}
			}
		}

		//Prepare the window for displaying stuff
		window.clear(sf::Color::White);
		window.setView(toolsView);

		//Track mouse position
		worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		//Save and Load functionality
		tools.saveButton.checkClick(std::bind(save, tile), worldPos);
		tools.loadButton.checkClick(std::bind(load, tile), worldPos);

		//Loo[ the toolbar tiles
		for (int i = 0; i < 9; i++)
		{
			if (tileButton[i].mouseOver(worldPos))
			{
				tileButton[i].sprite.setScale(sf::Vector2f(1.1f, 1.1f));
				tileButton[i].actor.sprite.setScale(sf::Vector2f(1.1f, 1.1f));
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (i < 3)
					{
						actorNotTile = false;
						curTileType = tileButton[i].type;
					}
					else
					{
						actorNotTile = true;
						curActorType = tileButton[i].actor.type;
					}
				}
			}
			else
			{
				tileButton[i].sprite.setScale(sf::Vector2f(1.0f, 1.0f));
				tileButton[i].actor.sprite.setScale(sf::Vector2f(1.0f, 1.0f));
			}
			//Draw the tiles in the same loop we check them
			window.draw(tileButton[i]);
		}
		window.draw(tools);
		window.setView(levelEditView);

		//Loop through all tiles to draw
		for (int i = 0; i < x; i++)
		{
			for (int j = 0; j < y; j++)
			{
				//check for click
				if (tile[i][j].mouseOver(worldPos) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					if (!actorNotTile)
					{
						//Return the oppostie of the current tipe
						tile[i][j].ChangeType(curTileType);
					}
					else //Painting with actors
					{
						tile[i][j].actor.ChangeActor(curActorType);
					}
				}
				//draw tiles
				tile[i][j].refreshTile();
				window.draw(tile[i][j]);
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			PrintToConsole(tile);
		}

		//Draw everything onto screen
		window.display();
	}
}