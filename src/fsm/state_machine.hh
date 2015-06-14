#pragma once

#include <fsm/transition.hh>
#include <memory>
#include <string>
#include <functional>

namespace virtdb { namespace fsm {
  
  class state_machine
  {
  public:
    typedef std::function<void(state_machine & sm,
                               transition & trans,
                               uint16_t seqno,
                               const std::string & msg)> report_error;
    
  private:
    std::string    description_;
    report_error   on_error_;
    
    // disable default construction
    state_machine() = delete;
    
    // disable copying until properly implemented
    state_machine(const state_machine &) = delete;
    state_machine & operator=(const state_machine &) = delete;
    
  public:
    typedef std::shared_ptr<state_machine> sptr;
    
    state_machine(const std::string & description,
                  report_error on_error);
    
    const std::string & description() const;
    
    void add_transition(transition::sptr & trans);
    void run();
    
    virtual ~state_machine();
  };
  
}}
