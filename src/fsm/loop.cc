#include <fsm/loop.hh>
#include <fsm/exception.hh>

namespace virtdb { namespace fsm {
  
  loop::loop(actor fun,
             const std::string & description)
  : fun_{fun},
    description_{description}
  {
  }
  
  bool
  loop::execute(uint16_t seqno,
                transition & trans,
                state_machine & sm,
                uint64_t iteration)
  {
    if( fun_ )
    {
      return fun_(seqno, trans, sm, iteration);
    }
    else
    {
      THROW_(std::string{"invalid actor function at: "}+std::to_string(seqno));
    }
  }
  
  const std::string &
  loop::description() const
  {
    return description_;
  }
  
  loop::~loop() {}
  
}}
