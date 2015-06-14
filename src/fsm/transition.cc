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
    for( auto s : starts_ )
    {
      auto a = all_actions_.find(s.first);
      if( a != all_actions_.end() )
      {
        auto res = ((a->second)(seqno, *this, sm));
        if( res == timeout )
          return true;
      }
    }
    return false;
  }
  
  void
  transition::set_action(uint16_t seqno,
                         action::sptr a)
  {
    auto f = [a](uint16_t seqno,
                 transition & trans,
                 state_machine & sm)
    {
      a->execute(seqno, trans, sm);
      return ok;
    };
    
    all_actions_[seqno] = f;
  }
  
  void
  transition::set_loop(uint16_t seqno,
                       loop::sptr l)
  {
    auto f = [l,this](uint16_t seqno,
                      transition & trans,
                      state_machine & sm)
    {
      bool result = true;
      uint64_t iteration = 0;
      while( result )
      {
        if( timed_out(seqno, sm) )
        {
          return timeout;
        }
        else
        {
          result = l->execute(seqno,
                              trans,
                              sm,
                              iteration);
        }
        ++iteration;
      }
      return (result ? ok : failed);
    };
    
    all_actions_[seqno] = f;
  }
  
  void
  transition::set_timer(uint16_t seqno,
                        timer::sptr t)
  {
    auto f = [t,this](uint16_t seqno,
                      transition & trans,
                      state_machine & sm)
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
  }
  
  void
  transition::clear_timer(uint16_t seqno,
                          uint16_t timer_at_seqno)
  {
    auto f = [timer_at_seqno, this](uint16_t seqno,
                                    transition & trans,
                                    state_machine & sm)
    {
      starts_.erase(seqno);
      return ok;
    };
    
    all_actions_[seqno] = f;
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
                      report_error on_error)
  {
    bool tmout    = false;
    bool stopped  = false;
    bool thrown   = false;
    uint16_t last_seqno = 0;
    
    try
    {
      for( auto a : all_actions_ )
      {
        last_seqno = a.first;
        if( timed_out(last_seqno, sm) )
        {
          tmout = true;
          break;
        }
        else
        {
          auto result = (a.second)(last_seqno, *this, sm);
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
      // TODO log error
      thrown = true;
    }
    catch (...)
    {
      // TODO log error
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
  
  transition::~transition() {}
  
}}
