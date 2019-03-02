//
//  BarrierFactory.cpp
//  Assessment-mobile
//
//  Created by Chris on 1/03/19.
//

#include "BarrierFactory.h"

BarrierFactory::BarrierFactory() {
}

void BarrierFactory::init() {
    _store.clear();
    
    iter = 0;
    
    Barrier b1 = {15, 30, 0, true};
    Barrier b2 = {15, 60, 25, false};
    Barrier b3 = {15, 20, 25, true};
    Barrier b4 = {15, 90, 25, false};
    Barrier b5 = {15, 120, 30, true};
    Barrier b6 = {15, 30, 20, false};
    
    _store.push_back(b1);
    _store.push_back(b2);
    _store.push_back(b3);
    _store.push_back(b4);
    _store.push_back(b5);
    _store.push_back(b6);
}

Barrier* BarrierFactory::pop() {
    assert(iter >= 0);
    
    if (iter >= _store.size()) {
        return nullptr;
    }
    
    Barrier* ret = &(_store[iter++]);
    return ret;
}

Barrier* BarrierFactory::next() {
    assert(iter >= 0);
    
    if (iter >= _store.size()) {
        return nullptr;
    }
    
    Barrier* ret = &(_store[iter]);
    return ret;
}
