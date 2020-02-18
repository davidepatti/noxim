#include "NoCViewer.h"
using namespace sf;
using namespace std;

NoCViewer::NoCViewer(Tile ***_t) : t(_t), x_shift(0.f), y_shift(0.f), scale_factor(1.f){   
    
    window = new RenderWindow(VideoMode(VideoMode::getDesktopMode().width,VideoMode::getDesktopMode().height), "noxim");

    RectangleShape * shape;

    for (int i = 0; i < GlobalParams::mesh_dim_x; i++)
        for(int j = 0; j < GlobalParams::mesh_dim_y; j++){
            shape = new RectangleShape(Vector2f(DEFAULT_ROUTER_SIDE,DEFAULT_ROUTER_SIDE));
            shape->setOutlineColor(Color::Black);
            shape->setFillColor(Color(192,192,192));
            shape->setPosition(Vector2f(
                i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE,
                j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE
            ));
            routers.push_back(shape);
            
            
            //WEST BUFFER

            slots_map[t[i][j]->r->buffer[DIRECTION_WEST]] = vector<RectangleShape *>();

            for(int z=0; z < GlobalParams::buffer_depth; z++){
                shape = new RectangleShape(Vector2f(DEFAULT_SLOT_SIDE,DEFAULT_SLOT_SIDE));
                shape->setOutlineColor(Color::Black);
                shape->setOutlineThickness(2);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE - (z+1)*DEFAULT_SLOT_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE
                ));
                slots_map[t[i][j]->r->buffer[DIRECTION_WEST]].push_back(shape);
            }

            //WEST LINE
            if(i > 0){
                shape = new RectangleShape(sf::Vector2f(DEFAULT_LINK_LENGTH, DEFAULT_LINK_THICKNESS));
                shape->setFillColor(Color::Black);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE - DEFAULT_LINK_LENGTH,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE - DEFAULT_LINK_THICKNESS
                ));
                links.push_back(shape);
            }

            //EAST BUFFER

            slots_map[t[i][j]->r->buffer[DIRECTION_EAST]] = vector<RectangleShape *>();

            for(int z=0; z < GlobalParams::buffer_depth; z++){
                shape = new RectangleShape(Vector2f(DEFAULT_SLOT_SIDE,DEFAULT_SLOT_SIDE));
                shape->setOutlineColor(Color::Black);
                shape->setOutlineThickness(2);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + z*DEFAULT_SLOT_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE - DEFAULT_SLOT_SIDE
                ));
                slots_map[t[i][j]->r->buffer[DIRECTION_EAST]].push_back(shape);
            }

            //EAST LINE

            if(i < GlobalParams::mesh_dim_x-1){
                shape = new RectangleShape(sf::Vector2f(DEFAULT_LINK_LENGTH, DEFAULT_LINK_THICKNESS));
                shape->setFillColor(Color::Black);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE
                ));
                links.push_back(shape);
            }

            //SOUTH BUFFER

            slots_map[t[i][j]->r->buffer[DIRECTION_SOUTH]] = vector<RectangleShape *>();

            for(int z=0; z < GlobalParams::buffer_depth; z++){
                shape = new RectangleShape(Vector2f(DEFAULT_SLOT_SIDE,DEFAULT_SLOT_SIDE));
                shape->setOutlineColor(Color::Black);
                shape->setOutlineThickness(2);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + z*DEFAULT_SLOT_SIDE
                ));
                slots_map[t[i][j]->r->buffer[DIRECTION_SOUTH]].push_back(shape);
            }

            //SOUTH LINE
            if(j < GlobalParams::mesh_dim_y-1){
                shape = new RectangleShape(sf::Vector2f(DEFAULT_LINK_THICKNESS,DEFAULT_LINK_LENGTH));
                shape->setFillColor(Color::Black);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE - DEFAULT_LINK_THICKNESS,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE  
                ));
                links.push_back(shape);
            }

            //NORTH BUFFER

            slots_map[t[i][j]->r->buffer[DIRECTION_NORTH]] = vector<RectangleShape *>();

            for(int z=0; z < GlobalParams::buffer_depth; z++){
                shape = new RectangleShape(Vector2f(DEFAULT_SLOT_SIDE,DEFAULT_SLOT_SIDE));
                shape->setOutlineColor(Color::Black);
                shape->setOutlineThickness(2);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE - DEFAULT_SLOT_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE - (z + 1)*DEFAULT_SLOT_SIDE
                ));

                slots_map[t[i][j]->r->buffer[DIRECTION_NORTH]].push_back(shape);
                
            }

            //NORTH LINE
            if(j > 0){
                shape = new RectangleShape(sf::Vector2f(DEFAULT_LINK_THICKNESS,DEFAULT_LINK_LENGTH));
                shape->setFillColor(Color::Black);
                shape->setPosition(Vector2f(
                    i*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE,
                    j*(GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE + DEFAULT_ROUTER_SIDE + DEFAULT_LINK_LENGTH) + GlobalParams::buffer_depth*DEFAULT_SLOT_SIDE - DEFAULT_LINK_LENGTH  
                ));
                links.push_back(shape);
            }
        }
    
    draw();

    eventHandler = new thread([this]() {
        Event event;
        while (this->window->waitEvent(event)){
            switch (event.type){
                case Event::Closed:
                    this->closeWindow();
                    break;

                case Event::KeyPressed:
                    switch(event.key.code){
                        case Keyboard::Up:
                            this->shiftY(SHIFT_Y);
                            this->draw();
                            break;
                        case Keyboard::Down:
                            this->shiftY(-SHIFT_Y);
                            this->draw();
                            break;
                        case Keyboard::Left:
                            this->shiftX(SHIFT_X);
                            this->draw();
                            break;
                        case Keyboard::Right:
                            this->shiftX(-SHIFT_X);
                            this->draw();
                            break;
                    }
                    break;
                
                case Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.wheel==Mouse::Wheel::VerticalWheel){
                        this->shiftX(-this->x_shift);
                        this->shiftY(-this->y_shift);
                        this->scale(
                            event.mouseWheelScroll.delta > 0 ? SCALE_FACTOR : 1/SCALE_FACTOR
                        );
                        this->draw();
                    }
                    break;
            }
        }
    });

}

void NoCViewer::draw() 
{   
    lock_guard<mutex> lock(mutex_object);

    if(!window->isOpen())
        return;
    
    window->clear(Color::White);

    for(auto iterator : routers)
        window->draw(*iterator);


    for( auto const& [buffer, slots] : slots_map ){
        queue<Flit> flits_queue = buffer->GetFlitsQueue();
        for(auto iterator : slots){
            if(!flits_queue.empty()){
                switch(flits_queue.front().flit_type){
                    case FLIT_TYPE_HEAD:
                        iterator->setFillColor(Color::Green);
                        break;
                    case FLIT_TYPE_BODY:
                        iterator->setFillColor(Color::Red);
                        break;
                    case FLIT_TYPE_TAIL:
                        iterator->setFillColor(Color::Blue);
                        break;
                }
                flits_queue.pop();
            }
            else
                iterator->setFillColor(Color::White);
            window->draw(*iterator);
        }
    }

    for(auto iterator : links)
        window->draw(*iterator);
    
    window->display();
}

void NoCViewer::closeWindow() 
{   
    lock_guard<mutex> lock(mutex_object);

    if(window->isOpen())
        window->close();
}

void NoCViewer::shiftX(float amount){

    lock_guard<mutex> lock(mutex_object);

    for(auto iterator : routers)
        iterator->move(amount,0.f);

    for( auto const& [buffer, slots] : slots_map )
        for(auto iterator : slots)
            iterator->move(amount,0.f);

    for(auto iterator : links)
        iterator->move(amount,0.f);

    x_shift += amount;
}

void NoCViewer::shiftY(float amount){

    lock_guard<mutex> lock(mutex_object);

    for(auto iterator : routers)
        iterator->move(0.f,amount);

    for( auto const& [buffer, slots] : slots_map )
        for(auto iterator : slots)
            iterator->move(0.f,amount);

    for(auto iterator : links)
        iterator->move(0.f,amount);

    y_shift += amount;

}

void NoCViewer::scale(float factor){

    lock_guard<mutex> lock(mutex_object);

    for(auto iterator : routers){
        iterator->setSize(Vector2f(iterator->getSize().x*factor,iterator->getSize().y*factor));
        iterator->setPosition(Vector2f(iterator->getPosition().x*factor,iterator->getPosition().y*factor));
    }

    for( auto const& [buffer, slots] : slots_map )
        for(auto iterator : slots){
            iterator->setSize(Vector2f(iterator->getSize().x*factor,iterator->getSize().y*factor));
            iterator->setPosition(Vector2f(iterator->getPosition().x*factor,iterator->getPosition().y*factor));
        }

    for(auto iterator : links){
        iterator->setSize(Vector2f(iterator->getSize().x*factor,iterator->getSize().y*factor));
        iterator->setPosition(Vector2f(iterator->getPosition().x*factor,iterator->getPosition().y*factor));
    }

    scale_factor *= factor;
}