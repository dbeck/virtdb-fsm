#include <fsm/state_machine.hh>
#include <fsm/exception.hh>

namespace virtdb { namespace fsm {
  
  state_machine::state_machine(const std::string & description,
                               trace_fun trace)
  : description_{description},
    trace_{trace}
  {
  }
  
  const std::string &
  state_machine::description() const
  {
    return description_;
  }
  
  void
  state_machine::add_transition(transition::sptr trans)
  {
    if( trans )
    {
      state_event se{trans->state(), trans->event()};
      transitions_[se] = trans;
    }
    else
    {
      THROW_("invalid transition received");
    }
  }
  
  void
  state_machine::enqueue(uint16_t event)
  {
    lock lck(event_mtx_);
    events_.push(event);
  }
  
  uint16_t
  state_machine::run(uint16_t initial_state)
  {
    uint16_t act_state = initial_state;
    while( true )
    {
      uint16_t act_event = 0;
      {
        lock lck(event_mtx_);
        if( events_.empty() )
          break;
        
        act_event = events_.front();
        events_.pop();
      }
      
      state_event se{act_state, act_event};
      
      auto it = transitions_.find(se);
      if( it == transitions_.end() )
        break;
      else
        act_state = (it->second)->execute(*this, trace_);
    }
    return act_state;
  }

  state_machine::~state_machine() {}
  
}}
