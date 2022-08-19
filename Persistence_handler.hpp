#ifndef _PERSISTENCE_HANDLER_H_
#define _PERSISTENCE_HANDLER_H_

class PersistenceHandler
{
public:
    virtual void save_state() = 0;
    virtual void load_state() = 0;

};



#endif