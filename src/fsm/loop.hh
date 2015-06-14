#pragma once

#include <string>
#include <memory>
#include <functional>

namespace virtdb { namespace fsm {
  
  class transition;
  class state_machine;
  
  class loop
  {
  public:
    typedef std::function<bool(uint16_t seqno,
                               transition & trans,
                               state_machine & sm,
                               uint64_t iteration)> actor;
  private:
    actor         fun_;
    std::string   description_;
    
    // disable default construction
    loop() = delete;
    
    // disable copying until properly implemented
    loop(const loop &) = delete;
    loop & operator=(const loop &) = delete;
    
  public:
    typedef std::shared_ptr<loop> sptr;
    
    loop(actor fun,
         const std::string & description);
    
    bool execute(uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 uint64_t iteration);
    
    const std::string & description() const;
    
    virtual ~loop();
  };
  
}}
