#pragma once

#include <fsm/transition.hh>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <queue>
#include <mutex>

namespace virtdb { namespace fsm {
  
  class state_machine
  {
  public:
    typedef transition::trace_fun      trace_fun;
        
  private:
    typedef std::pair<uint16_t, uint16_t>            state_event;
    typedef std::map<state_event,transition::sptr>   trans_map;
    typedef std::unique_lock<std::mutex>             lock;
    
    std::string           description_;
    trace_fun             trace_;
    trans_map             transitions_;
    std::queue<uint16_t>  events_;
    std::mutex            event_mtx_;
    
    // disable default construction
    state_machine() = delete;
    
    // disable copying until properly implemented
    state_machine(const state_machine &) = delete;
    state_machine & operator=(const state_machine &) = delete;
    
  public:
    typedef std::shared_ptr<state_machine> sptr;
    
    state_machine(const std::string & description,
                  trace_fun trace=[](uint16_t seqno,
                                     const std::string & desc,
                                     const transition & trans,
                                     const state_machine & sm){});
    
    const std::string & description() const;
    
    void add_transition(transition::sptr trans);
    void enqueue(uint16_t event);
    uint16_t run(uint16_t initial_state=0);
    
    virtual ~state_machine();
  };
  
}}
