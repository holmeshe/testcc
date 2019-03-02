//
//  GameScene.hpp
//  Assessment-mobile
//
//  Created by Chris on 28/02/19.
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include "cocos2d.h"
#include <vector>

USING_NS_CC;

using namespace ui;
using namespace std;

class Barrier;

enum class PhysicsCategory {
    None = 0,
    Bird = (1 << 0),    // 1
    Bar = (1 << 1), // 2
    All = PhysicsCategory::Bird | PhysicsCategory::Bar // 3
};

class Game : public cocos2d::Scene
{
private:
    Sprite* _bird;
    float _curFallSpeed = 0;
    
    vector<DrawNode*> _bars;
    
    Vector<SpriteFrame*> _jumpFrames;
    
    Action* _jumpAct;
    
    bool _done;
    
    int _score;
protected:
    bool onContactBegin(PhysicsContact &contact);

    bool onTouchBegin(Touch* touch, Event* event);

    void dummy(Touch* touch, Event* event);
    
    void fallingBird(float delta);
    void updateBarriers(float delta);
    void addBarrier();
    DrawNode* createNodeWithBarrier(Barrier *br, float indecatedX);
    
    void over();

public:
    static cocos2d::Scene* createScene();
    
    virtual bool init() override;
    
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
    
    void update(float) override;
};

#endif /* GameScene_hpp */
