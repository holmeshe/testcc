//
//  GameScene.cpp
//  Assessment-mobile
//
//  Created by Chris on 28/02/19.
//

#include "GameScene.h"
#include "MenuScene.h"
#include "BarrierFactory.h"

const float BIRD_G = 8;
const float BIRD_STR = 85;
const float BAR_SPD = 28;

Scene* Game::createScene() {
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init() {
    // 5. ref: https://docs.cocos2d-x.org/cocos2d-x/en/physics/concepts.html
    if ( !Scene::initWithPhysics() ) {
        return false;
    }
    
    BarrierFactory& fac = BarrierFactory::getInstance();
    fac.init();
    
    _curFallSpeed = 0;
    _done = false;
    _jumpAct = nullptr;
    _score = 0;
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    _bird = Sprite::create("norm.jpg");
    _bird->setScale(3);
    if (_bird == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
        return false;
    }
    _bird->setPosition(Vec2(origin.x + _bird->getContentSize().width * 3,
                            origin.y + visibleSize.height / 2 + _bird->getContentSize().height / 2));
    
    // 5. ref: https://docs.cocos2d-x.org/cocos2d-x/en/physics/collisions.html
    auto physicsBody = PhysicsBody::createCircle(_bird->getContentSize().width/3.8 );
    physicsBody->setDynamic(true);
    physicsBody->setCategoryBitmask((int)PhysicsCategory::Bird);
    physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
    physicsBody->setContactTestBitmask((int)PhysicsCategory::Bar);
    _bird->addComponent(physicsBody);

    this->addChild(_bird, 1);

    // 2. ref: https://www.gamefromscratch.com/post/2014/10/03/Cocos2d-x-Tutorial-Series-Handling-Touch-and-Mouse-Input.aspx
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchEnded = CC_CALLBACK_2(Game::dummy, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Game::dummy, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2(Game::dummy, this);
    
    touchListener->onTouchBegan = CC_CALLBACK_2(Game::onTouchBegin, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    // 3. ref: https://www.gamefromscratch.com/post/2014/10/11/Cocos2d-x-Tutorial-Series-Game-loops-Updates-and-Action-Handling.aspx
    this->scheduleUpdate();
    
    // 5. ref: https://docs.cocos2d-x.org/cocos2d-x/en/physics/collisions.html
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Game::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    // 4. ref: https://docs.cocos2d-x.org/cocos2d-x/en/actions/sequences.html
    _jumpFrames.reserve(5);
    _jumpFrames.pushBack(SpriteFrame::create("jump1.jpg", Rect(0,
                                                              0,
                                                              _bird->getContentSize().width,
                                                              _bird->getContentSize().height)));
    _jumpFrames.pushBack(SpriteFrame::create("jump2.jpg", Rect(0,
                                                              0,
                                                              _bird->getContentSize().width,
                                                              _bird->getContentSize().height)));
    _jumpFrames.pushBack(SpriteFrame::create("jump3.jpg", Rect(0,
                                                              0,
                                                              _bird->getContentSize().width,
                                                              _bird->getContentSize().height)));
    _jumpFrames.pushBack(SpriteFrame::create("jump4.jpg", Rect(0,
                                                              0,
                                                              _bird->getContentSize().width,
                                                              _bird->getContentSize().height)));
    _jumpFrames.pushBack(SpriteFrame::create("norm.jpg", Rect(0,
                                                              0,
                                                              _bird->getContentSize().width,
                                                              _bird->getContentSize().height)));
    
    return true;
}

void Game::update(float delta) {
    if (_done) {
        return;
    }
    fallingBird(delta);
    updateBarriers(delta);
}

bool Game::onTouchBegin(Touch* touch, Event* event) {
    if (_done) { return false; }
    
    _curFallSpeed = 0;
    
    if (_jumpAct != nullptr) {
        _bird->stopAction(_jumpAct);
    }
    
    Animation* animation = Animation::createWithSpriteFrames(_jumpFrames, 0.08f);
    Animate* animate = Animate::create(animation);
    auto seq = Sequence::create(animate, nullptr);
    
    _bird->runAction(seq);
    
    MoveTo *moveTo = MoveTo::create(2, Vec2(_bird->getPositionX(), _bird->getPositionY() + BIRD_STR));
    _bird->runAction(moveTo);
    
    return true;
}

void Game::fallingBird(float delta) {
    auto position = _bird->getPosition();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    if (position.y > origin.y + visibleSize.height) {
        over();
    }
    
    _curFallSpeed += BIRD_G * delta;
    position.y -= _curFallSpeed;
    
    if (position.y < 0) {
        over();
    }
    
    _bird->setPosition(position);
}

void Game::updateBarriers(float delta) {
    for (vector<DrawNode*>::iterator it = _bars.begin(); it != _bars.end();) {
        DrawNode* bar = *it;
        assert(bar != nullptr);
        
        auto position = bar->getPosition();
        position.x -= delta * BAR_SPD;
        if (position.x < 0) {
            _score++;
            bar->removeFromParent();
            it = _bars.erase(it);
            continue;
        }

        ++it;
        bar->setPosition(position);
    }
    
    addBarrier();
    
    if (_bars.size() == 0) {
        over();
    }
}

void Game::addBarrier() {
    BarrierFactory& fac = BarrierFactory::getInstance();
    Barrier *nextBr = fac.next();
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    if (nextBr == nullptr) {
        return;
    }
    
    float tailX = origin.x + visibleSize.width;
    if (_bars.size() > 0) {
        DrawNode* lstBar = _bars[_bars.size() - 1];
        assert(lstBar != nullptr);
        auto position = lstBar->getPosition();
        tailX = lstBar->getPositionX() + lstBar->getContentSize().width / 2;
    }
    
    if (origin.x + visibleSize.width - tailX >= nextBr->distance) {
        fac.pop();

        auto bar = createNodeWithBarrier(nextBr, tailX);
        
        this->addChild(bar, 2);
        this->_bars.push_back(bar);
    }
}

DrawNode* Game::createNodeWithBarrier(Barrier *br, float indecatedX) {
    assert(br);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto rectNode = DrawNode::create();
    if (rectNode == nullptr) {
        assert(false);
    }
    
    Vec2 rectangle[4];
    rectangle[0] = Vec2(-br->width/2, -br->height/2);
    rectangle[1] = Vec2(br->width/2, -br->height/2);
    rectangle[2] = Vec2(br->width/2, br->height/2);
    rectangle[3] = Vec2(-br->width/2, br->height/2);
    
    Color4F white(1, 1, 1, 1);
    
    rectNode->drawPolygon(rectangle, 4, white, 1, white);
    rectNode->setPosition(Vec2(indecatedX + br->distance + br->width / 2,
                               br->isUp ? origin.y + br->height / 2 : origin.y + visibleSize.height - br->height / 2));

    auto physicsBody = PhysicsBody::createBox(Size(br->width , br->height),
                                              PhysicsMaterial(0.1f, 1.0f, 0.0f));
    physicsBody->setDynamic(true);
    physicsBody->setCategoryBitmask((int)PhysicsCategory::Bar);
    physicsBody->setCollisionBitmask((int)PhysicsCategory::None);
    physicsBody->setContactTestBitmask((int)PhysicsCategory::Bird);
    rectNode->addComponent(physicsBody);

    return rectNode;
}

bool Game::onContactBegin(PhysicsContact &contact) {
    over();
    
    return true;
}

void Game::over() {
    _done = true;
    Vector<SpriteFrame*> animFrames;

    animFrames.reserve(7);
    
    if (_bars.size() > 0) {
        animFrames.pushBack(SpriteFrame::create("fail1.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail2.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail3.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail4.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail5.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail6.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("fail7.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
    } else {
        animFrames.pushBack(SpriteFrame::create("succ1.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));
        animFrames.pushBack(SpriteFrame::create("succ2.jpg", Rect(0,
                                                                  0,
                                                                  _bird->getContentSize().width,
                                                                  _bird->getContentSize().height)));

    }

    Animation* animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
    Animate* animate = Animate::create(animation);
    _bird->stopAllActions();
    
    if (_score > highestScore /* global variable will not exist in prod */) {
        highestScore= _score;
    }
    auto callbackEnd = CallFunc::create([](){
        auto menuScene = MainMenu::createScene();
        Director::getInstance()->replaceScene(menuScene);
        

    });

    
    auto seq = Sequence::create(animate, callbackEnd, nullptr);

    _bird->runAction(seq);
}

void Game::dummy(Touch* touch, Event* event) {
    return;
}
