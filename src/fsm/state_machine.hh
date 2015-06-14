#pragma once

#include <fsm/transition.hh>
#include <memory>

namespace virtdb { namespace fsm {
  
  class state_machine
  {
    // disable copying until properly implemented
    state_machine(const state_machine &) = delete;
    state_machine & operator=(const state_machine &) = delete;
    
  public:
    typedef std::shared_ptr<state_machine> sptr;
    
    state_machine();
    virtual ~state_machine();
  };
  
}}
