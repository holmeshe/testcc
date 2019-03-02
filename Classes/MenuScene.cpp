/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"

int highestScore = 0; /* Global variable will not exist in prod */

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainMenu::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    string txt = "Hello There";
    if (highestScore > 0) {
        char tmp[255];
        snprintf(tmp, 255, "Highest Score: %d", highestScore);
        txt = tmp;
    }

    _score = Label::createWithTTF(txt, "fonts/Marker Felt.ttf", 24);
    if (_score == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
        return false;
    }
    
    _score->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - _score->getContentSize().height));

    this->addChild(_score, 1);

    // 1. ref: https://docs.cocos2d-x.org/cocos2d-x/en/ui_components/buttons.html
    auto button = Button::create("Button_Normal.png", "Button_Press.png", "Button_Disable.png");
    if (button == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
        return false;
    }
    
    button->setScale(3);
    button->setTitleText("Start Game");
    button->setTitleFontSize(6);
    button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type){
        Scene *gameScene;
        switch (type)
        {
            case ui::Widget::TouchEventType::BEGAN:
                break;
            case ui::Widget::TouchEventType::ENDED:
                gameScene = Game::create();
                gameScene->getPhysicsWorld()->setGravity(Vec2(0,0));
                Director::getInstance()->replaceScene(gameScene);

                break;
            default:
                break;
        }
    });
    
    button->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    this->addChild(button, 0);
    
    return true;
}

