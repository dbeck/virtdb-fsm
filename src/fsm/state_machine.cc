#include <fsm/state_machine.hh>

namespace virtdb { namespace fsm {
  
  state_machine::state_machine(const std::string & description,
                               report_error on_error)
  : description_{description},
    on_error_{on_error}
  {
  }
  
  const std::string &
  state_machine::description() const
  {
    return description_;
  }
  
  void
  state_machine::add_transition(transition::sptr & trans)
  {
    // TODO
  }
  
  void
  state_machine::run()
  {
    // TODO
  }

  state_machine::~state_machine() {}
  
}}
