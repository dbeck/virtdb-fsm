#pragma once

#include <memory>

namespace virtdb { namespace fsm {
  
  class timer
  {
    // disable copying until properly implemented
    timer(const timer &) = delete;
    timer & operator=(const timer &) = delete;
    
  public:
    typedef std::shared_ptr<timer> sptr;
    
    timer();
    virtual ~timer();
  };
  
}}
