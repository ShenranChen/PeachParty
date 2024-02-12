#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), bankMoney(0), peach(nullptr), yoshi(nullptr)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

//add vortex to vortex
void StudentWorld::addVortex(int forceDir, int startX, int startY)
{
    things.push_back(new Vortex(this, forceDir, startX, startY));
}

//add dropping square to vector
void StudentWorld::addDroppingSquare(int x, int y)
{
    things.push_back(new DroppingSquare(this, x, y));
}

Actor* StudentWorld::getActorWithSamePos(Actor* a, int x, int y)
{
    
    for(int i = 0; i < things.size(); i++)
    {
       if(things[i] != a && !things[i]->isMoving() && a->getX() == things[i]->getX() && a->getY() == things[i]->getY())
        {
            return things[i];
            break;
        }
    }
    return nullptr;
}

Actor* StudentWorld::getImpactableActor(Actor* a, int x, int y)
{
    for(int i = 0; i < things.size(); i++)
    {
        if(things[i]->isImpactable())
        {
            if(a->getX() >= things[i]->getX() && a->getX() < things[i]->getX()+16 && a->getY() >= things[i]->getY() && a->getY() < things[i]->getY()+16)
            {
                return things[i];
            }
        }
    }
    return nullptr;
}

int StudentWorld::init()
{
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = board.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
    {
        return GWSTATUS_BOARD_ERROR;
//        cerr << "Could not find board01.txt data file\n";
    }
    else if (result == Board::load_fail_bad_format)
    {
        return GWSTATUS_BOARD_ERROR;
//        cerr << "Your board was improperly formatted\n";
    }
    else if (result == Board::load_success)
    {
        cerr << "Successfully loaded board\n";
        
        for(int y = 0; y < 16 ; y++)
        {
            for(int x = 0; x < 16; x++)
            {
                Board::GridEntry ge = board.getContentsOf(x, y);
                switch (ge) {
                    case Board::player:
                        peach = new Peach(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y);
                        yoshi = new Yoshi(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y);
                        things.push_back(new CoinSquare(this,IID_BLUE_COIN_SQUARE, true, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::blue_coin_square:
                        things.push_back(new CoinSquare(this,IID_BLUE_COIN_SQUARE, true, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::red_coin_square:
                        things.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, false, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::star_square:
                        things.push_back(new StarSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::bank_square:
                        things.push_back(new BankSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::bowser:
                        things.push_back(new Bowser(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        things.push_back(new CoinSquare(this,IID_BLUE_COIN_SQUARE, true, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::boo:
                        things.push_back(new Boo(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        things.push_back(new CoinSquare(this,IID_BLUE_COIN_SQUARE, true, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    case Board::up_dir_square:
                        things.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 90));
                        break;
                    case Board::down_dir_square:
                        things.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 270));
                        break;
                    case Board::left_dir_square:
                        things.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 180));
                        break;
                    case Board::right_dir_square:
                        things.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 0));
                        break;
                    case Board::event_square:
                        things.push_back(new EventSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y));
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //call each item in the vector
    for(int i= 0; i< things.size(); i++)
    {
        if(!things[i]->isAlive())
        {
            delete things[i];
            things.erase(things.begin()+i);
            i--;
        }
        else
        {
            things[i]->doSomething();
        }
    }
    peach->doSomething();
    yoshi->doSomething();
    
    //making the scoreboard
    string text;
    text += "P1 Roll: " + to_string(peach->getTicks()/8) + " Stars: " + to_string(peach->getStars()) +" $$: " + to_string(peach->getCoins());
    if(peach->getHasVortex())
    {
        text += " VOR";
    }
    text += " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBank()) + " | P2 Roll: " + to_string(yoshi->getTicks()/8) + " Stars: " + to_string(yoshi->getStars()) +" $$: " + to_string(yoshi->getCoins());
    if(yoshi->getHasVortex())
    {
        text += " VOR";
    }
    
    setGameStatText(text);
    
    //when game ends
    if (timeRemaining() <= 0)
    {
        playSound(SOUND_GAME_FINISHED);
       if(winner() == 1)
       {
           setFinalScore(peach->getStars(), peach->getCoins());
           return GWSTATUS_PEACH_WON;
       }
       else
       {
           setFinalScore(yoshi->getStars(), yoshi->getCoins());
           return GWSTATUS_YOSHI_WON;
       }
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete peach;
    peach = nullptr;
    delete yoshi;
    yoshi = nullptr;
    vector<Actor*>::iterator it;
    it = things.begin();
    while(it != things.end())
    {
        delete *it;
        it = things.erase(it);
    }
}

//given coordinates, is it a valid position
bool StudentWorld::isValidPos(int x, int y) 
{
    //check to see if it in bounds
   if( x < 0 || x >= 16 || y < 0 || y >= 16)
   {
       return false;
   }
//    cout << x << " " << y << endl;
    if(board.getContentsOf(x, y) == Board::empty)
    {
        return false;
    }
//    if( x == 10 & y == 5)
//    {
//        cout << "hey" << endl;
//        return false;
//    }
    return true;
}

//check if the the coordinate is a direction square
bool StudentWorld::isDirectionSq(int x, int y)
{
    if(board.getContentsOf(x, y) == Board::down_dir_square || board.getContentsOf(x, y) == Board::up_dir_square || board.getContentsOf(x, y) == Board::left_dir_square || board.getContentsOf(x, y) == Board::right_dir_square)
    {
        return true;
    }
    return false;
}

//check to see who won
int StudentWorld::winner() const
{
    int peachPoints = peach->getStars();
    int yoshiPoints = yoshi->getStars();
    
    if(yoshiPoints > peachPoints)
    {
        return 2;
    }
    else if(peachPoints > yoshiPoints)
    {
        return 1;
    }
    else
    {
        peachPoints += peach->getCoins();
        yoshiPoints += yoshi->getCoins();
        if(peachPoints > yoshiPoints)
        {
            return 1;
        }
        else if (yoshiPoints > peachPoints)
        {
            return 2;
        }
        else
        {
            return randInt(1, 2);
        }
    }
}
