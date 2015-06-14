#pragma once

#include <memory>

namespace virtdb { namespace fsm {
  
  class action
  {
    // disable copying until properly implemented
    action(const action &) = delete;
    action & operator=(const action &) = delete;
    
  public:
    typedef std::shared_ptr<action> sptr;
    
    action();
    virtual ~action();
  };
  
}}
