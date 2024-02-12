#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

bool MovingObject::isFork() const
{
    int curX = getX();
    int curY = getY();
    int count = 0;
   
    //check each direction to see how many possible dir there are
    if(curX % 16 == 0 && curY %16 == 0)
    {
        //the left
        getPositionInThisDirection(left, 16, curX, curY);
        if(getWalkingDirection()!= right && curX >=0 && getWorld()->isValidPos(curX/16, curY/16))
        {
//            std::cout<< "left" << std::endl;
            count++;
        }
        //the start is not a fork
        if(curX < 0 || (getWalkingDirection() == right &&!getWorld()->isValidPos(curX/16, curY/16)))
        {
//            std::cout<< "account for start" << std::endl;
            count--;
        }
        
        //the up
        getPositionInThisDirection(up, 16, curX, curY);
        if(getWalkingDirection()!= down && getWorld()->isValidPos(curX/16, curY/16))
        {
//            std::cout<< "up" << std::endl;
            count++;
        }
        if(getWalkingDirection() == down && !getWorld()->isValidPos(curX/16, curY/16))
        {
            count--;
        }
        
        //the down
        getPositionInThisDirection(down, 16, curX, curY);
        if(getWalkingDirection()!= up && curY >=0 && getWorld()->isValidPos(curX/16, curY/16))
        {
//            std::cout<< "down" << std::endl;
            count++;
        }
        if(curY < 0 || (getWalkingDirection() == up && !getWorld()->isValidPos(curX/16, curY/16)))
        {
            count--;
        }
        
        //the right
        getPositionInThisDirection(right, 16, curX, curY);
        if(getWalkingDirection()!= left && getWorld()->isValidPos(curX/16, curY/16))
        {
//            std::cout<< "right" << std::endl;
            count++;
        }
        if(getWalkingDirection() == left  && !getWorld()->isValidPos(curX/16, curY/16))
        {
            count--;
        }

        
    }
    //if there are more than two possible direction, it is a fork
    return count >= 2;
}

//change position if it is a corner, if so change walking position
void MovingObject::turningPoint()
{
    int nextX;
    int nextY;
    int curDir = getWalkingDirection();
    
    //get position of the next square
    if(curDir == right || curDir == up)
    {
        getPositionInThisDirection(curDir, 16, nextX, nextY);
    }
    else
    {
        //ugh the math...
        getPositionInThisDirection(curDir, 2, nextX, nextY);
    }
    //if next square is not a valid pos, then change walking direction
   if(nextX < 0 || nextY < 0 ||  !getWorld()->isValidPos(nextX/16, nextY/16))
   {
       for(int i = 0; i< 1; i++)
       {
           getPositionInThisDirection(up, 16, nextX, nextY);
           if(curDir != down && getWorld()->isValidPos(nextX/16, nextY/16))
           {
               setWalkingDirection(up);
               setDirection(0);
               break;
           }
           getPositionInThisDirection(down, 16, nextX, nextY);
           if(curDir != up && getWorld()->isValidPos(nextX/16, nextY/16))
           {
               setWalkingDirection(down);
               setDirection(0);
               break;
           }
           getPositionInThisDirection(right, 16, nextX, nextY);
           if(curDir != left && getWorld()->isValidPos(nextX/16, nextY/16))
           {
               setWalkingDirection(right);
               setDirection(0);
               break;
           }
           getPositionInThisDirection(left, 16, nextX, nextY);
           if(curDir != right && getWorld()->isValidPos(nextX/16, nextY/16))
           {
               setWalkingDirection(left);
               setDirection(180);
               break;
           }
       }
   }
}

//pick a random new direction
void MovingObject::randomDirection()
{
    int old = getWalkingDirection();

    //loops until we find a vlid direction
    while(getWalkingDirection() == invalid || getWalkingDirection() == old)
    {
        int dir = randInt(1, 4);
        int nextX = getX();
        int nextY = getY();
        switch (dir) {
            case 1:
            {
                getPositionInThisDirection(right, 16, nextX, nextY);
                if(getWorld()->isValidPos(nextX/16, nextY/16))
                {
                    setWalkingDirection(right);
                    setDirection(0);
                }
                break;
            }
            case 2:
            {
                getPositionInThisDirection(left, 16, nextX, nextY);
                if(getWorld()->isValidPos(nextX/16, nextY/16))
                {
                    setWalkingDirection(left);
                    setDirection(180);
                }
                break;
            }
            case 3:
            {
                getPositionInThisDirection(up, 16, nextX, nextY);
                if(getWorld()->isValidPos(nextX/16, nextY/16))
                {
                    setWalkingDirection(up);
                    setDirection(0);
                }
                break;
            }
            case 4:
            {
                getPositionInThisDirection(down, 16, nextX, nextY);
                if(getWorld()->isValidPos(nextX/16, nextY/16))
                {
                    setWalkingDirection(down);
                    setDirection(0);
                }
                break;
            }
            default:
                break;
        }
    }
}

void MovingObject::teleport()
{
    int newX = randInt(0, 15);
    int newY = randInt(0, 15);
    
    //if the position isn't part of the board or if it's the same as the old
    while(!getWorld()->isValidPos(newX, newY) || (newX == getX() && newY == getY()))
    {
        newX = randInt(0, 15);
        newY = randInt(0, 15);
    }
    
    moveTo(newX*16, newY*16);
    teleportSpecific();
    
}

void PlayerAvatar::doSomething()
{
    
   if(getState() == WAITING)
   {
       //just teleported, new walking direction
       if(getWalkingDirection() == invalid)
       {
           randomDirection();
       }
       //see if the user inputed action
       switch (getWorld()->getAction(getPlayerNum())) {
           case ACTION_NONE:
           {
               break;
           }
           case ACTION_ROLL:
           {
               int diceRoll = randInt(1, 10);
               setTicks(diceRoll * 8);
               setState(WALKING);
               break;
           }
           case ACTION_FIRE:
           {
               //add a vortex to board
               if(hasVortex)
               {
                   if(getWalkingDirection() == left)
                   {
                       getWorld()->addVortex(getWalkingDirection(), getX()-16, getY());
                   }
                   else if(getWalkingDirection() == right )
                   {
                       getWorld()->addVortex(getWalkingDirection(), getX()+16, getY());
                   }
                    else if(getWalkingDirection() == up)
                    {
                        getWorld()->addVortex(getWalkingDirection(), getX(), getY() + 16);
                    }
                    else if(getWalkingDirection() == down)
                    {
                        getWorld()->addVortex(getWalkingDirection(), getX(), getY() - 16);
                    }
                   getWorld()->playSound(SOUND_PLAYER_FIRE);
                   setHasVortex(false);
               }
               break;
           }
           default:
               return;
               
       }
   }
    if( getState() == WALKING)
    {
        int nextX;
        int nextY;
        int curDir = getWalkingDirection();
//        std::cout << getX() << " " << getY() << std::endl;
        //if it's fork and not direction square
        if( isFork() && !getWorld()->isDirectionSq(getX()/16, getY()/16))
        {
//            std::cout << "Fork" << std::endl;
            //wait for user to input a new direction
            switch (getWorld()->getAction(getPlayerNum())){
                case ACTION_UP:
                {
                    getPositionInThisDirection(up, 16, nextX, nextY);
                    if(curDir != down && getWorld()->isValidPos(nextX/16, nextY/16))
                    {
                        setWalkingDirection(up);
                        setDirection(0);
                    }
                    else
                        return;
                    break;
                }
                case ACTION_DOWN:
                {
                    getPositionInThisDirection(down, 16, nextX, nextY);
                    if(curDir != up && getWorld()->isValidPos(nextX/16, nextY/16))
                    {
                        setWalkingDirection(down);
                        setDirection(0);
                    }
                    else
                        return;
                    break;
                }
                case ACTION_LEFT:
                {
                    getPositionInThisDirection(left, 16, nextX, nextY);
                    if(curDir != right && getWorld()->isValidPos(nextX/16, nextY/16))
                    {
                        setWalkingDirection(left);
                        setDirection(180);
                    }
                    else
                        return;
                    break;
                }
                case ACTION_RIGHT:
                {
                    getPositionInThisDirection(right, 16, nextX, nextY);
                    if(curDir != left && getWorld()->isValidPos(nextX/16, nextY/16))
                    {
                        setWalkingDirection(right);
                        setDirection(0);
                    }
                    else
                        return;
                    break;
                }
                default:
                    return;
            }
        }
        //changes direction if the next square of current direction is invalid
        turningPoint();
        moveAtAngle(getWalkingDirection(), 2);
        setTicks(getTicks()-1);
        if(getTicks() == 0)
        {
            setState(WAITING);
        }
    }
}

void PlayerAvatar::teleportSpecific()
{
    setWalkingDirection(invalid);
}


void PlayerAvatar::swap(PlayerAvatar* other)
{
    //changes the following:
//    x, y coordinates
//    ii. the number of ticks left that the player has to move before
//    completing their roll
//    iii. the player's walk direction
//    iv. the player's sprite direction
//    v. the player's roll/walk state
    int tempX = getX();
    int tempY = getY();
    int tempTicks = getTicks();
    int tempWalkDir = getWalkingDirection();
    int tempSprite = getDirection();
    int tempState = getState();
    
    moveTo(other->getX(), other->getY());
    other->moveTo(tempX, tempY);
    
    setTicks(other->getTicks());
    other->setTicks(tempTicks);
    
    setWalkingDirection(other->getWalkingDirection());
    other->setWalkingDirection(tempWalkDir);
    
    setDirection(other->getDirection());
    other->setDirection(tempSprite);
    
    setState(other->getState());
    other->setState(tempState);
}

void Vortex::doSomething()
{
    //check if alive
   if(!isAlive())
   {
       return;
   }
    //move
    moveAtAngle(fireDir, 2);
    //out of bounds
    if(getX() < 0 || getX() >=VIEW_WIDTH || getY() < 0 || getY() >=VIEW_HEIGHT)
    {
        setDead();
    }
    
    if(isAlive())
    {
        //find impactable actor
        Actor* a = getWorld()->getImpactableActor(this, getX(), getY());
        if(a != nullptr)
        {
            a->setImpacted(true);
            setDead();
            getWorld()->playSound(SOUND_HIT_BY_VORTEX);
        }
        else
        {
            std::cerr << "no impacted actor" << std::endl;
        }
        //loop through to find any impactable actor and see if location overlaps-Violate spec??
//        int size = getWorld()->getVectorSize();
//        for(int i = 0; i < size; i++)
//        {
//            Actor* a = getWorld()->getActor(i);
//            if(a->isImpactable())
//            {
//                if(getX() >= a->getX() && getX() < a->getX()+16 && getY() >= a->getY() && getY() < a->getY()+16)
//                {
//                    a->setImpacted(true);
//                    setDead();
//                    getWorld()->playSound(SOUND_HIT_BY_VORTEX);
//                    break;
//                }
//            }
//        }
    }
}

void Squares::doSomething()
{
    Peach* p = getWorld()->getPeach();
    Yoshi* y = getWorld()->getYoshi();
    
    //square that activates when landed on
    if(getWhenActivate() == ONLYLANDSON)
    {
        //for peach
        if(!isPActivated() && p->getState() == WAITING && (p->getX() == getX() && p->getY() == getY()))
        {
            setPActive(true);
            doSpecific(p);

        }
         else if(p->getX() != getX() && p->getY() != getY())
         {
             setPActive(false);
         }
        //for yoshi
        if(!isYActivated() && y->getState() == WAITING && (y->getX() == getX() && y->getY() == getY()))
        {
            setYActive(true);
            doSpecific(y);
            
        }
        else if(y->getX() != getX() && y->getY() != getY())
        {
            setYActive(false);
        }
    }
    //squares that activate when landed on and move onto, same action
   else if(getWhenActivate() == BOTHBUTSAME)
   {
       if(!isPActivated() && p->getX() == getX() && p->getY() == getY())
       {
           setPActive(true);
           doSpecific(p);
       }
       else if(p->getX() != getX() && p->getY() != getY())
       {
           setPActive(false);
       }
       
       if(!isYActivated() && y->getX() == getX() && y->getY() == getY())
       {
           setYActive(true);
           doSpecific(y);
       }
       else if(y->getX() != getX() && y->getY() != getY())
       {
           setYActive(false);
       }
   }
    //ssquare that activates when landed on and moves onto, different action
    else if(getWhenActivate() == BOTHBUTDIFFERENT)
    {
        if(!isPActivated() && (p->getX() == getX() && p->getY() == getY()))
        {
           setPActive(true);
            doSpecific(p);
        }
        else if(p->getX() != getX() && p->getY() != getY())
        {
            setPActive(false);
        }
//        else if(p->getState() == WALKING && (p->getX() == getX() && p->getY() == getY()))
//        {
//            doSpecific(p);
//        }
        
        if(!isYActivated()  && (y->getX() == getX() && y->getY() == getY()))
        {
            setYActive(true);
            doSpecific(y);
        }
         else if(y->getX() != getX() && y->getY() != getY())
         {
             setYActive(false);
         }
//         else if(y->getState() == WALKING && (y->getX() == getX() && y->getY() == getY()))
//         {
//             doSpecific(y);
//         }

    }
    
}

//gives 3 coins if blue, or deduct 3 if red
void CoinSquare::doSpecific(PlayerAvatar* a)
{
    if(givesCoins)
    {
        a->setCoins(a->getCoins()+3);
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    else
    {
        if(a->getCoins() < 3)
        {
            a->setCoins(0);
        }
        else
        {
            a->setCoins(a->getCoins()-3);
        }
        getWorld()->playSound(SOUND_TAKE_COIN);
    }
}

//gives star if player has at least 20 coins
void StarSquare::doSpecific(PlayerAvatar* a)
{
    if(a->getCoins() < 20)
    {
        return;
    }
    a->setCoins(a->getCoins()-20);
    a->setStars(a->getStars() + 1);
    getWorld()->playSound(SOUND_GIVE_STAR);
}

//changes the player walking direction
void DirectionalSquare::doSpecific(PlayerAvatar* a)
{
    a->setWalkingDirection(getForceDir());
    if(getForceDir() == left)
    {
        a->setDirection(180);
    }
    if(a->getWalkingDirection() != left && getForceDir() != left)
    {
        a->setDirection(0);
    }
}

//random 3 action
void EventSquare::doSpecific(PlayerAvatar* a)
{
    PlayerAvatar* b;
    if(a->getPlayerNum() == 1)
    {
        b = getWorld()->getYoshi();
    }
    else
    {
        b = getWorld()->getPeach();
    }

    int random = randInt(1, 3);
    switch (random) {
        case 1:
        {
            a->teleport();
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            break;
        }
        case 2:
        {
//            std::cout << "hey2" << std::endl;
            a->swap(b);
            //make sure it doesn't activate on that new player
            if(a->getPlayerNum() == 1)
            {
                setYActive(true);
            }
            else
            {
                setPActive(true);
            }
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            break;
        }
        case 3:
        {
            if(!a->getHasVortex())
            {
                a->setHasVortex(true);
                getWorld()->playSound(SOUND_GIVE_VORTEX);
            }
            break;
        }
        default:
        break;
    }
}

//gives coins if landed on else deduct 5
void BankSquare::doSpecific(PlayerAvatar* a)
{
   if(a->getState() == WAITING)
   {
       a->setCoins(a->getCoins() + getWorld()->getBank());
        getWorld()->setBank(0);
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
   }
    else if( a->getState() == WALKING)
    {
        if(a->getCoins() < 5)
        {
            getWorld()->setBank(getWorld()->getBank() + a->getCoins());
            a->setCoins(0);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
        }
        else
        {
            a->setCoins(a->getCoins() - 5);
            getWorld()->setBank(getWorld()->getBank() + 5);
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
        }
    }
}


void DroppingSquare::doSpecific(PlayerAvatar* a)
{
    int random = randInt(1, 2);
    switch (random) {
    case 1:
    {
        if(a->getCoins() < 10)
        {
            a->setCoins(0);
        }
        else
        {
            a->setCoins(a->getCoins() - 10);
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        break;
    }
    case 2:
    {
        if(a->getStars() >= 1)
        {
            a->setCoins(a->getStars() - 1);
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        break;
    }
    default:
        break;
    }
}

//changes stars or coins
void Boo::doSpecific(int playerNum)
{
    PlayerAvatar* p = getWorld()->getPeach();
    PlayerAvatar* y = getWorld()->getYoshi();

    int random = randInt(1, 2);
    
    if(random == 1)
    {
        int temp = p->getStars();
        p->setStars(y->getStars());
        y->setStars(temp);
        getWorld()->playSound(SOUND_BOO_ACTIVATE);
    }
    else
    {
        int temp = p->getCoins();
        p->setCoins(y->getCoins());
        y->setCoins(temp);
        getWorld()->playSound(SOUND_BOO_ACTIVATE);
    }
}

//get rid of coins and stars
void Bowser:: doSpecific(int playerNum)
{
    PlayerAvatar* a;
    if(playerNum == 1)
    {
        a = getWorld()->getPeach();
    }
    else
    {
        a = getWorld()->getYoshi();
    }
    int random = randInt(1, 2);
    switch (random) {
        case 1:
        {
            a->setCoins(0);
            a->setStars(0);
            getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            break;
        }
        default:
            break;
        }
}

void Baddies::doSomething()
{
    Peach* p = getWorld()->getPeach();
    Yoshi* y = getWorld()->getYoshi();
    if(getState() == WAITING)
    {
        if( !isPActivated() && (getX() == p->getX() && getY() == p->getY()) && p->getState() == WAITING )
        {
            setPActive(true);
            doSpecific(1);
        }
        if(getX() != p->getX() && getY() != p->getY())
        {
            setPActive(false);
        }
        
        
        if(!isYActivated() && (getX() == y->getX() && getY() == y->getY()) && y->getState() == WAITING)
        {
            setYActive(true);
            doSpecific(2);
        }
        if(getX() != y->getX() && getY() != y->getY())
        {
            setYActive(false);
        }
            
        
        setPauseTicks(getPauseTicks()-1);
        if(getPauseTicks() == 0)
        {
            int squares_to_move;
            //different squares to move
            if(droppingSq())
            {
                squares_to_move = randInt(1, 10);
            }
            else
            {
                squares_to_move = randInt(1, 3);
            }
            setTicks(squares_to_move *8);
            //random direction to move
            setWalkingDirection(invalid);
            randomDirection();
            setState(WALKING);
        }
    }
            
    if(getState() == WALKING)
    {
        if( isFork() && !getWorld()->isDirectionSq(getX()/16, getY()/16))
        {
            randomDirection();
        }
        //if it's corner, change dir
        turningPoint();
        
        moveAtAngle(getWalkingDirection(), 2);
        setTicks(getTicks()-1);
        if(getTicks() == 0)
        {
            setState(WAITING);
            setPauseTicks(180);
            if(droppingSq())
            {
                //making a dropping square
                int random = randInt(1, 4);
                if(random == 1)
                {
                    Actor* a = getWorld()->getActorWithSamePos(this, getX(), getY());
                    if(a != nullptr)
                    {
                        a->setDead();
                        getWorld()->addDroppingSquare(getX(), getY());
                        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
                    }
                    else
                    {
                        std::cerr << "omg" << std::endl;
                    }
                }
            }
        }
    }
    if(isImpacted())
    {
        teleport();
        setImpacted(false);
    }
}
void Baddies::teleportSpecific()
{
    setWalkingDirection(right);
    setDirection(0);
    setState(WAITING);
    setPauseTicks(180);
}
