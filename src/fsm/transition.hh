#pragma once

#include <memory>

namespace virtdb { namespace fsm {
  
  class transition
  {
    // disable copying until properly implemented
    transition(const transition &) = delete;
    transition & operator=(const transition &) = delete;
    
  public:
    typedef std::shared_ptr<transition> sptr;
    
    transition();
    virtual ~transition();
  };
  
}}
