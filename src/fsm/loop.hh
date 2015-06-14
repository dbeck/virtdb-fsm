#pragma once

#include <memory>

namespace virtdb { namespace fsm {
  
  class loop
  {
    // disable copying until properly implemented
    loop(const loop &) = delete;
    loop & operator=(const loop &) = delete;
    
  public:
    typedef std::shared_ptr<loop> sptr;
    
    loop();
    virtual ~loop();
  };
  
}}
