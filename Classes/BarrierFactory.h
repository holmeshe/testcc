//
//  BarrierFactory.hpp
//  Assessment-mobile
//
//  Created by Chris on 1/03/19.
//

#ifndef BarrierFactory_hpp
#define BarrierFactory_hpp

#include <vector>

using namespace std;

typedef struct Barrier {
    float width;
    float height;
    float distance; // the first barrier is 0
    bool isUp;
} Barrier;


class BarrierFactory {
private:
    vector<Barrier> _store;
    size_t iter;
    
    BarrierFactory();
    BarrierFactory(BarrierFactory const&);
    void operator=(BarrierFactory const&);

public:
    static BarrierFactory& getInstance() {
        static BarrierFactory instance;
        return instance;
    }
    
    void init();
    Barrier* pop();
    Barrier* next();
};


#endif /* BarrierFactory_hpp */
