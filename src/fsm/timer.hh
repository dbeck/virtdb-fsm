#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <functional>

namespace virtdb { namespace fsm {
  
  class transition;
  class state_machine;
  
  class timer
  {
  public:
    typedef std::chrono::steady_clock clock_type;
    
    typedef std::function<bool(uint16_t seqno,
                               transition & trans,
                               state_machine & sm,
                               const clock_type::time_point & started_at)> actor;
    
  private:
    actor         fun_;
    std::string   description_;
    
    // disable default construction
    timer() = delete;

    // disable copying until properly implemented
    timer(const timer &) = delete;
    timer & operator=(const timer &) = delete;
    
  public:
    typedef std::shared_ptr<timer> sptr;
    
    timer(actor fun,
          const std::string & description);
    
    bool execute(uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 const clock_type::time_point & started_at);
    
    const std::string & description() const;
    
    virtual ~timer();
  };
  
}}
