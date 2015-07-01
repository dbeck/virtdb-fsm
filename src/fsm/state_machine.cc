#include <fsm/state_machine.hh>
#include <fsm/exception.hh>
#include <sstream>

namespace virtdb { namespace fsm {
  
  state_machine::state_machine(const std::string & description,
                               trace_fun trace_cb)
  : description_{description},
    trace_{trace_cb}
  {
  }
  
  const std::string &
  state_machine::description() const
  {
    return description_;
  }
  
  state_machine::trace_fun
  state_machine::trace_cb()
  {
    return trace_;
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
  state_machine::queue_has(uint16_t event) const
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
  
  uint64_t
  state_machine::queue_size() const
  {
    uint64_t ret = 0;
    {
      lock lck(event_mtx_);
      ret = events_.size();
    }
    return ret;
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
      else
      {
        // no such transitions
        if( trace_ )
        {
          std::ostringstream os;
          os << "NO SUCH TRANSITION: [" << state_name(act_state) << " + " << event_name(act_event) << ']';
          transition tr{act_state, act_event, 0, os.str()};
          trace_(0,"<NO ACTION>",tr,*this);
        }
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
      return std::to_string(st);
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
      return std::to_string(ev);
  }
  
}}
