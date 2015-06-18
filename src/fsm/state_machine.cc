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
    events_.push_back(event);
  }
  
  void
  state_machine::enqueue_if_empty(uint16_t event)
  {
    lock lck(event_mtx_);
    if( events_.empty() )
      events_.push_back(event);
  }
  
  void
  state_machine::enqueue_unique(uint16_t event)
  {
    lock lck(event_mtx_);
    for( auto const & e : events_ )
    {
      if( e == event )
      {
        return;
      }
    }
    
    events_.push_back(event);
  }
  
  bool
  state_machine::queue_has(uint16_t event)
  {
    lock lck(event_mtx_);
    for( auto const & e : events_ )
    {
      if( e == event )
      {
        return true;
      }
    }
    return false;
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
        events_.pop_front();
      }
      
      state_event se{act_state, act_event};
      
      auto it = transitions_.find(se);
      if( it != transitions_.end() )
      {
        act_state = (it->second)->execute(*this, trace_);
      }
    }
    return act_state;
  }

  state_machine::~state_machine() {}
  
  void
  state_machine::state_name(uint16_t st,
                            const std::string & name)
  {
    lock lck(state_name_mtx_);
    state_names_[st] = name;
  }
  
  std::string
  state_machine::state_name(uint16_t st) const
  {
    lock lck(state_name_mtx_);
    auto it = state_names_.find(st);
    if( it != state_names_.end() )
      return it->second;
    else
      return std::string();
  }

  void
  state_machine::event_name(uint16_t ev,
                            const std::string & name)
  {
    lock lck(event_name_mtx_);
    event_names_[ev] = name;
  }
  
  std::string
  state_machine::event_name(uint16_t ev) const
  {
    lock lck(event_name_mtx_);
    auto it = event_names_.find(ev);
    if( it != event_names_.end() )
      return it->second;
    else
      return std::string();
  }
  
}}
