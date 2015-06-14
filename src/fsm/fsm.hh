#pragma once

#include <memory>

namespace virtdb { namespace fsm {
  
  class fsm
  {
    // disable copying until properly implemented
    fsm(const fsm &) = delete;
    fsm & operator=(const fsm &) = delete;
    
  public:
    typedef std::shared_ptr<fsm> sptr;
    
    fsm();
    virtual ~fsm();
  };
  
}}
