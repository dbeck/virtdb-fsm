#pragma once

#include <fsm/action.hh>
#include <fsm/loop.hh>
#include <fsm/timer.hh>
#include <memory>
#include <string>
#include <functional>
#include <map>

namespace virtdb { namespace fsm {
  
  class state_machine;
  
  class transition
  {
  public:
    typedef std::function<void(state_machine & sm,
                               transition & trans,
                               uint16_t seqno,
                               const std::string & msg)> report_error;
  private:
    typedef std::function<bool(uint16_t seqno,
                               transition & trans,
                               state_machine & sm,
                               uint64_t iteration,
                               const timer::clock_type::time_point & started_at,
                               uint16_t timer_at_seqno)> action_fun;
    
    typedef std::map<uint16_t, action_fun>                      action_map;
    typedef std::map<uint16_t, timer::clock_type::time_point>   start_map;
    
    uint16_t     state_;
    uint16_t     event_;
    uint16_t     next_state_;
    std::string  description_;
    action_map   all_actions_;
    action_map   timers_;
    start_map    starts_;
    
    // disable default construction
    transition() = delete;
    
    // disable copying until properly implemented
    transition(const transition &) = delete;
    transition & operator=(const transition &) = delete;
    
  public:
    typedef std::shared_ptr<transition> sptr;
    
    transition(uint16_t state,
               uint16_t event,
               uint16_t next_state,
               const std::string & description);
    
    const std::string & description() const;
    
    void set_action(uint16_t seqno, action::sptr a);
    void set_loop(uint16_t seqno, loop::sptr l);
    void set_timer(uint16_t seqno, timer::sptr t);
    void clear_timer(uint16_t seqno, uint16_t timer_at_seqno);
    
    // set next states
    void on_timeout_state(uint16_t nst);
    void on_error_state(uint16_t nst);
    void default_state(uint16_t nst);
    
    // do the transition and return next state
    uint16_t execute(state_machine & sm,
                     report_error on_error);
    
    virtual ~transition();
  };
  
}}
