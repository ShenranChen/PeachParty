#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "GameWorld.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
const int WAITING = 0;
const int WALKING = 1;
const int invalid = -1;

const int ONLYLANDSON = 1;
const int BOTHBUTSAME = 2;
const int BOTHBUTDIFFERENT = 3;

//-----------------------Actor, Base Class
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth, double size):GraphObject(imageID, startX, startY, startDirection, depth, size), m_world(world), m_alive(true), impacted(false){}
    virtual ~Actor(){}
    StudentWorld* getWorld() const;
    virtual void doSomething() = 0;
    
    virtual bool isImpactable() const {return false;}
    bool isImpacted() const{return impacted;}
    void setImpacted(bool impact) {impacted = impact;}
    
    bool isAlive() const {return m_alive;}
    void setDead() { m_alive = false;}
    
    virtual bool isMoving() const {return true;}
    
private:
    StudentWorld* m_world;
    bool impacted;
    bool m_alive;
    

};

class Vortex: public Actor
{
public:
    Vortex(StudentWorld* world, int fireDIR, double startX, double startY): Actor(world, IID_VORTEX, startX, startY, right, 0, 1), fireDir(fireDIR){}
    virtual ~Vortex(){}
    virtual void doSomething();
    
private:
    int fireDir;
    
};

//------------------------ Peach and Yoshi, maybe baddies
class MovingObject: public Actor
{
public:
    MovingObject(StudentWorld* world, int imageID, double startX, double startY) : Actor(world, imageID, startX, startY, right, 0, 1), m_state(WAITING), m_walkingDirection(right), m_ticks_to_move(0){}
    virtual ~MovingObject(){}
    
    virtual void doSomething() = 0;
    
    void setWalkingDirection(int dir) {m_walkingDirection = dir;}
    int getWalkingDirection() const { return m_walkingDirection;}
    
    void setState(int state) {m_state = state;}
    int getState() const { return m_state;}
    
    void setTicks(int ticks) { m_ticks_to_move = ticks;}
    int getTicks() const { return m_ticks_to_move;}
    
    bool isFork() const;
    
    void turningPoint();
    
    void randomDirection();
    
    void teleport();
    virtual void teleportSpecific() = 0;
    
private:
    int m_state;
    int m_ticks_to_move;
    int m_walkingDirection;
    
};

//------------------------ Baddies
class Baddies: public MovingObject
{
public:
    Baddies(StudentWorld* world, int imageID, double startX, double startY): MovingObject(world, imageID, startX, startY), pauseTicks(180), peachActivated(false), yoshiActivated(false){}
    virtual ~Baddies(){}
    
    void setPauseTicks(int ticks){pauseTicks = ticks;}
    int getPauseTicks() const { return pauseTicks;}
    
    virtual bool isImpactable() const {return true;}
    
    virtual void doSomething();
    virtual void doSpecific(int playerNum) = 0;
    virtual void teleportSpecific();
    
    virtual bool droppingSq() const {return true;}
    
    bool isPActivated() const{ return peachActivated;}
    void setPActive(bool stats){ peachActivated = stats;}
    bool isYActivated() const{ return yoshiActivated;}
    void setYActive(bool stats){ yoshiActivated = stats;}
    
private:
    int pauseTicks;
    bool peachActivated;
    bool yoshiActivated;
    
};

class Bowser: public Baddies
{
public:
    Bowser(StudentWorld* world, double startX, double startY)
        : Baddies(world, IID_BOWSER , startX, startY){}
    virtual ~Bowser(){}
    virtual void doSpecific(int playerNum);
};

class Boo: public Baddies
{
public:
    Boo(StudentWorld* world, double startX, double startY)
        : Baddies(world, IID_BOO , startX, startY){}
    virtual ~Boo(){}
    virtual bool droppingSq() const {return false;}
    virtual void doSpecific(int playerNum);
};

//------------------------ Player Avatar
class PlayerAvatar: public MovingObject
{
public:
    PlayerAvatar(StudentWorld* world, int playerNum, int imageID, double startX, double startY)
    :MovingObject(world, imageID, startX, startY), m_player(playerNum), m_coins(0), m_stars(0),
    hasVortex(false){}
    virtual ~PlayerAvatar(){}
    virtual void doSomething();
    
    int getPlayerNum(){ return m_player;}
    void setCoins(int num){m_coins = num;}
    int getCoins() const {return m_coins;}
    
    void setStars(int num){m_stars = num;}
    int getStars()const {return m_stars;}
    
    void setHasVortex(bool status){hasVortex = status;}
    bool getHasVortex() const{ return hasVortex;}
    
    void swap(PlayerAvatar* other);
    
    virtual void teleportSpecific();
    
private:
    int m_player;
    int m_coins;
    int m_stars;
    bool hasVortex;
};

//------------------------ Peach
class Peach: public PlayerAvatar
{
public:
    Peach(StudentWorld* world, double startX, double startY)
        :PlayerAvatar(world, 1, IID_PEACH, startX, startY){}
    virtual ~Peach(){}
};

//------------------------ Yoshi
class Yoshi: public PlayerAvatar
{
public:
    Yoshi(StudentWorld* world, double startX, double startY)
    :PlayerAvatar(world, 2, IID_YOSHI, startX, startY){};
    virtual ~Yoshi(){};
};


//-----------------------Squares

class Squares: public Actor
{
public:
    Squares(StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth, double size, int activiateOn): Actor(world, imageID, startX, startY, startDirection, depth, size), peachActivated(false), yoshiActivated(false), whenActivate(activiateOn){}
    
    virtual ~Squares(){}
    bool isPActivated() const{ return peachActivated;}
    void setPActive(bool stats){ peachActivated = stats;}
    bool isYActivated() const{ return yoshiActivated;}
    void setYActive(bool stats){ yoshiActivated = stats;}
    
    virtual bool isMoving() const {return false;}
    virtual void doSomething();
    virtual void doSpecific(PlayerAvatar* a) = 0;
    
    
private:
    bool peachActivated;
    bool yoshiActivated;
    int whenActivate;
    int getWhenActivate() const {return whenActivate;}
};

//-----------------------Coin Squares
class CoinSquare: public Squares
{
public:
    CoinSquare(StudentWorld* world, int imageID, bool type, double startX, double startY)
        :Squares(world, imageID, startX, startY, right, 1, 1, ONLYLANDSON), givesCoins(type){}
    virtual ~CoinSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
private:
    bool givesCoins;
    bool getGivesCoins() const { return givesCoins;}
};

//-----------------------Stars Squares
class StarSquare: public Squares
{
public:
    StarSquare(StudentWorld* world, double startX, double startY)
        :Squares(world, IID_STAR_SQUARE, startX, startY, right, 1, 1, BOTHBUTSAME){}
    virtual ~StarSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
};

//-----------------------Dir Squares
class DirectionalSquare: public Squares
{
public:
    DirectionalSquare(StudentWorld* world, double startX, double startY, int sprite)
        :Squares(world, IID_DIR_SQUARE, startX, startY, sprite, 1, 1, BOTHBUTSAME), forceDir(sprite){}
    virtual ~DirectionalSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
    
private:
    int forceDir;
    int getForceDir() const{ return forceDir;}
};

//-----------------------Bank Squares
class BankSquare: public Squares
{
public:
    BankSquare(StudentWorld* world, double startX, double startY)
        :Squares(world, IID_BANK_SQUARE, startX, startY, right, 1, 1, BOTHBUTDIFFERENT){}
    virtual ~BankSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
};

//-----------------------Event Squares
class EventSquare: public Squares
{
public:
    EventSquare(StudentWorld* world, double startX, double startY)
        :Squares(world, IID_EVENT_SQUARE, startX, startY, right, 1, 1, ONLYLANDSON){}
    virtual ~EventSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
};

//-----------------------Dropping Squares
class DroppingSquare: public Squares
{
public:
    DroppingSquare(StudentWorld* world, double startX, double startY)
        :Squares(world, IID_DROPPING_SQUARE, startX, startY, right, 1, 1, ONLYLANDSON){}
    virtual ~DroppingSquare(){}
    virtual void doSpecific(PlayerAvatar* a);
};

#endif // ACTOR_H_
