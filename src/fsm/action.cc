#include <fsm/action.hh>
#include <fsm/exception.hh>
#include <fsm/transition.hh>

namespace virtdb { namespace fsm {
  
  action::action(actor fun,
                 const std::string & description)
  : fun_{fun},
    description_{description}
  {
  }
  
  void
  action::execute(uint16_t seqno,
                  transition & trans,
                  state_machine & sm)
  {
    if( fun_ )
    {
      fun_(seqno, trans, sm);
    }
    else
    {
      THROW_(std::string{"invalid actor function at: "}+std::to_string(seqno));
    }
  }
  
  const std::string &
  action::description() const
  {
    return description_;
  }
  
  action::~action() {}
  
}}
