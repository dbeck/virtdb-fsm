#pragma once

#include <string>
#include <memory>
#include <functional>

namespace virtdb { namespace fsm {
  
  class transition;
  class state_machine;
  
  class action
  {
  public:
    typedef std::function<void(uint16_t seqno,
                               transition & trans,
                               state_machine & sm)> actor;
    
  private:
    actor         fun_;
    std::string   description_;
    
    // disable default construction
    action() = delete;
    
    // disable copying until properly implemented
    action(const action &) = delete;
    action & operator=(const action &) = delete;
    
  public:
    typedef std::shared_ptr<action> sptr;
    
    action(actor fun,
           const std::string & description);
    
    void execute(uint16_t seqno,
                 transition & trans,
                 state_machine & sm);
    
    const std::string & description() const;
    
    virtual ~action();
  };
  
}}
