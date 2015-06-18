#include <fsm/transition.hh>

namespace virtdb { namespace fsm {
  
  transition::transition(uint16_t state,
                         uint16_t event,
                         uint16_t next_state,
                         const std::string & description)
  : state_{state},
    event_{event},
    timeout_state_{next_state},
    error_state_{next_state},
    default_state_{next_state},
    description_{description}
  {
  }
  
  bool
  transition::timed_out(uint16_t seqno,
                        state_machine & sm)
  {
    auto dummy_trace = [](uint16_t seqno,
                          const std::string & desc,
                          const transition & trans,
                          const state_machine & sm) {};
    
    for( auto s : starts_ )
    {
      auto a = all_actions_.find(s.first);
      if( a != all_actions_.end() )
      {
        auto res = ((a->second)(s.first,
                                *this,
                                sm,
                                dummy_trace));
        if( res == timeout )
          return true;
      }
    }
    return false;
  }
  
  const std::string &
  transition::seqno_description(uint16_t seqno)
  {
    static const std::string empty;
    const std::string & ret = empty;
    
    auto it = seqno_descs_.find(seqno);
    if( it != seqno_descs_.end() )
    {
      const std::string & tmp = (it->second)();
      return tmp;
    }
    return ret;
  }
  
  void
  transition::set_action(uint16_t seqno,
                         action::sptr a)
  {
    auto f = [a](uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 trace_fun trace)
    {
      a->execute(seqno, trans, sm);
      return ok;
    };
    
    all_actions_[seqno] = f;
    seqno_descs_[seqno] = [a]() { return a->description(); };
  }
  
  void
  transition::set_loop(uint16_t seqno,
                       loop::sptr l)
  {
    auto f = [l,this](uint16_t seqno,
                      transition & trans,
                      state_machine & sm,
                      trace_fun trace)
    {
      action_result result = ok;
      uint64_t iteration = 0;
      while( result == ok)
      {
        if( timed_out(seqno, sm) )
        {
          result = timeout;
        }
        else
        {
          if( !l->execute(seqno,
                          trans,
                          sm,
                          iteration) )
          {
            break;
          }
        }
        ++iteration;
      }
      if( trace && iteration != 1 )
      {
        std::string trace_str = l->description() + "[" + std::to_string(iteration) +"]";
        trace( seqno, trace_str, trans, sm );
      }
      return result;
    };
    
    all_actions_[seqno] = f;
    seqno_descs_[seqno] = [l]() { return l->description(); };
  }
  
  void
  transition::set_timer(uint16_t seqno,
                        timer::sptr t)
  {
    auto f = [t,this](uint16_t seqno,
                      transition & trans,
                      state_machine & sm,
                      trace_fun trace)
    {
      {
        if( starts_.count(seqno) == 0 )
        {
          starts_[seqno] = timer::clock_type::now();
        }
      }
      bool result = t->execute(seqno, trans, sm, starts_[seqno]);
      return (result ? ok : timeout);
    };
    
    all_actions_[seqno] = f;
    seqno_descs_[seqno] = [t]() { return t->description(); };
  }
  
  void
  transition::clear_timer(uint16_t seqno,
                          uint16_t timer_at_seqno)
  {
    auto f = [timer_at_seqno, this](uint16_t seqno,
                                    transition & trans,
                                    state_machine & sm,
                                    trace_fun trace)
    {
      starts_.erase(seqno);
      return ok;
    };
    
    all_actions_[seqno] = f;
    
    std::string clear{"CLEAR["};
    clear += std::to_string(timer_at_seqno)+"]: ";
    clear += seqno_description(timer_at_seqno);
    
    seqno_descs_[seqno] = [clear](){ return clear; };
  }
   
  void
  transition::on_timeout_state(uint16_t nst)
  {
    timeout_state_ = nst;
  }
  
  void
  transition::on_error_state(uint16_t nst)
  {
    error_state_ = nst;
  }
  
  void
  transition::default_state(uint16_t nst)
  {
    default_state_ = nst;
  }
   
  uint16_t
  transition::execute(state_machine & sm,
                      trace_fun trace)
  {
    bool tmout    = false;
    bool stopped  = false;
    bool thrown   = false;
    uint16_t last_seqno = 0;

    // clear timers
    starts_.clear();
    
    try
    {
      for( auto a : all_actions_ )
      {
        last_seqno = a.first;
        if( trace )
        {
          std::string desc = seqno_description(last_seqno);
          trace(last_seqno,
                desc,
                *this,
                sm);
        }
        if( timed_out(last_seqno, sm) )
        {
          tmout = true;
          break;
        }
        else
        {
          auto result = (a.second)(last_seqno,
                                   *this,
                                   sm,
                                   trace);
          if( result == timeout )
          {
            tmout = true;
            break;
          }
          else if( result == failed )
          {
            stopped = true;
            break;
          }
        }
      }
    }
    catch (const std::exception & e)
    {
      std::string desc = seqno_description(last_seqno);
      std::string trace_str = desc + "[EXCEPTION]";
      trace( last_seqno, trace_str, *this, sm );
      thrown = true;
    }
    catch (...)
    {
      thrown = true;
      std::string desc = seqno_description(last_seqno);
      std::string trace_str = desc + "[EXCEPTION]";
      trace( last_seqno, trace_str, *this, sm );
      thrown = true;
    }

    // last check for timeout
    if( !tmout )
    {
      tmout = timed_out(last_seqno+1, sm);
    }
    
    if( thrown || stopped )
    {
      return error_state_;
    }
    else if( tmout )
    {
      return timeout_state_;
    }
    else
    {
      return default_state_;
    }
  }
  
  const std::string &
  transition::description() const
  {
    return description_;
  }
  
  uint16_t
  transition::state() const
  {
    return state_;
  }
  
  uint16_t
  transition::event() const
  {
    return event_;
  }
  
  transition::~transition() {}
  
}}
