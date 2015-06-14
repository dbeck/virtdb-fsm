#include <fsm/timer.hh>
#include <fsm/exception.hh>

namespace virtdb { namespace fsm {
  
  
  timer::timer(actor fun,
               const std::string & description)
  : fun_{fun},
    description_{description}
  {
  }
  
  bool
  timer::execute(uint16_t seqno,
                 transition & trans,
                 state_machine & sm,
                 const clock_type::time_point & started_at)
  {
    if( fun_ )
    {
      return fun_(seqno,
                  trans,
                  sm,
                  started_at);
    }
    else
    {
      THROW_(std::string{"invalid actor function at: "}+std::to_string(seqno));      
    }
  }
  
  const std::string &
  timer::description() const
  {
    return description_;
  }
  
  timer::~timer() {}
  
}}
