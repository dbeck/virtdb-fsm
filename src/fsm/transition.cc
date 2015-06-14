#include <fsm/transition.hh>

namespace virtdb { namespace fsm {
  
  transition::transition(uint16_t state,
                         uint16_t event,
                         uint16_t next_state,
                         const std::string & description)
  : state_{state},
    event_{event},
    next_state_{next_state},
    description_{description}
  {
  }
  
  
  /*
   typedef std::function<bool(uint16_t seqno,
   transition & trans,
   state_machine & sm,
   uint64_t iteration,
   const timer::clock_type::time_point & started_at,
   uint16_t timer_at_seqno)> action_fun;
   */
  
  void
  transition::set_action(uint16_t seqno,
                         action::sptr a)
  {
    auto f = [a](uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 uint64_t iteration,
                 const timer::clock_type::time_point & started_at,
                 uint16_t timer_at_seqno)
    {
      a->execute(seqno, trans, sm);
      return true;
    };
    all_actions_[seqno] = f;
  }
  
  void
  transition::set_loop(uint16_t seqno,
                       loop::sptr l)
  {
    auto f = [l](uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 uint64_t iteration,
                 const timer::clock_type::time_point & started_at,
                 uint16_t timer_at_seqno)
    {
      // TODO
      l->execute(seqno, trans, sm, iteration);
      return true;
    };
    all_actions_[seqno] = f;
  }
  
  void
  transition::set_timer(uint16_t seqno,
                        timer::sptr t)
  {
    // TODO
  }
  
  void
  transition::clear_timer(uint16_t seqno,
                          uint16_t timer_at_seqno)
  {
    // TODO
  }
   
  void
  transition::on_timeout_state(uint16_t nst)
  {
    // TODO
  }
  
  void
  transition::on_error_state(uint16_t nst)
  {
    // TODO
  }
  
  void
  transition::default_state(uint16_t nst)
  {
    // TODO
  }
   
  uint16_t
  transition::execute(state_machine & sm,
                      report_error on_error)
  {
    // TODO
    return 0;
  }
  
  const std::string &
  transition::description() const
  {
    return description_;
  }
  
  transition::~transition() {}
  
}}
