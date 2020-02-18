#ifndef __NOXIMNOCVIEWER_H__
#define __NOXIMNOCVIEWER_H__

#include "Tile.h"
#include "GlobalParams.h"
#include "Buffer.h"
#include <SFML/Graphics.hpp>
#include <tuple>
#include <thread>
#include <mutex>

using namespace sf;

#define DEFAULT_ROUTER_SIDE 30.0
#define DEFAULT_LINK_LENGTH 40.0
#define DEFAULT_SLOT_SIDE 10.0
#define DEFAULT_LINK_THICKNESS 2.0
#define SHIFT_X 15.0
#define SHIFT_Y 15.0
#define SCALE_FACTOR 1.5

class NoCViewer {
    
    public:
        NoCViewer(Tile ***_t);
        void draw();
        
    private:
        Tile *** t;
        RenderWindow * window;
        vector<RectangleShape *> routers;
        vector<RectangleShape *> links;
        map<Buffer *, vector<RectangleShape *>> slots_map;
        float x_shift, y_shift, scale_factor;
        thread * eventHandler;
        mutable mutex mutex_object;
        void closeWindow();
        void shiftX(float amount);
        void shiftY(float amount);
        void scale(float factor);
};

#endif