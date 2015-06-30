#pragma once

#include <fsm/transition.hh>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <list>
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
    typedef std::map<uint16_t, std::string>          name_map;
    
    std::string           description_;
    trace_fun             trace_;
    trans_map             transitions_;
    std::list<uint16_t>   events_;
    mutable std::mutex    event_mtx_;
    name_map              state_names_;
    mutable std::mutex    state_name_mtx_;
    name_map              event_names_;
    mutable std::mutex    event_name_mtx_;
    
    // disable default construction
    state_machine() = delete;
    
    // disable copying until properly implemented
    state_machine(const state_machine &) = delete;
    state_machine & operator=(const state_machine &) = delete;
    
  public:
    typedef std::shared_ptr<state_machine> sptr;
    
    state_machine(const std::string & description,
                  trace_fun trace_cb=[](uint16_t seqno,
                                        const std::string & desc,
                                        const transition & trans,
                                        const state_machine & sm){});
    
    const std::string & description() const;
    trace_fun trace_cb();
    
    void add_transition(transition::sptr trans);
    void enqueue(uint16_t event);
    void enqueue_unique(uint16_t event);
    void enqueue_if_empty(uint16_t event);
    uint16_t run(uint16_t initial_state=0);
    bool queue_has(uint16_t event);
    
    void state_name(uint16_t st, const std::string & name);
    std::string state_name(uint16_t st) const;

    void event_name(uint16_t ev, const std::string & name);
    std::string event_name(uint16_t ev) const;
    
    virtual ~state_machine();
  };
  
}}
