#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Peach;
class Yoshi;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
    ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();
    
    bool isValidPos(int x, int y);
    bool isDirectionSq(int x, int y);
    
    Peach* getPeach() const {return peach;}
    Yoshi* getYoshi() const {return yoshi;}
   
    Actor* getActorWithSamePos( Actor* a, int x, int y);
    Actor* getImpactableActor(Actor* a, int x, int y);
    
    int getBank() const {return bankMoney;}
    void setBank(int amt){ bankMoney = amt;}
    
    void addVortex(int forceDir, int startX, int startY);
    void addDroppingSquare(int x, int y);
    

private:
    Peach* peach;
    Yoshi* yoshi;
    int bankMoney;
    std::vector<Actor*> things;
    Board board;
    int winner() const;
};

#endif // STUDENTWORLD_H_
