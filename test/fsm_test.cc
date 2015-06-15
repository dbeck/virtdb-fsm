#include <gtest/gtest.h>
#include <fsm/state_machine.hh>
#include <fsm/exception.hh>
#include <future>
#include <iostream>
#include <string.h>
#include <map>

using namespace virtdb::fsm;

namespace virtdb { namespace test {
  
  class FsmTest : public ::testing::Test { };
  
  auto trace = [](uint16_t seqno,
                  const std::string & desc,
                  const transition & trans,
                  const state_machine & sm)
  {
    std::cout << sm.description() << '[' << trans.state() << "]:"
              << trans.description() << " ev/" << trans.event() << "/ : "
              << "(" << seqno << ") "
              << desc << "\n";
  };

}}

using namespace virtdb::test;

TEST_F(FsmTest, EmptyFsm)
{
  state_machine sm("TEST");
  sm.run();
}

TEST_F(FsmTest, OneTransitionNoActions)
{
  state_machine sm("TEST");
  
  uint16_t state = 0;
  uint16_t event = 99;
  uint16_t non_existent_state = 100;
  transition::sptr tr1{new transition{state,event,non_existent_state,"TR1"}};
  sm.add_transition(tr1);
  
  // run will do nothing, because no events yet
  sm.run();
  
  // run will execute one transition
  sm.enqueue(99);
  uint16_t terminal_state = sm.run();
  
  EXPECT_EQ(terminal_state, non_existent_state);
}

TEST_F(FsmTest, OneTransitionOneAction)
{
  state_machine sm("TEST",trace);
  
  transition::sptr tr1{new transition{0,0,0,"TR1"}};
  sm.add_transition(tr1);
  
  action::sptr act1{new action{[](uint16_t seqno,
                                  transition & trans,
                                  state_machine & sm){
    sm.enqueue(1);
  },"ACT1"}};
  
  // add the action to the transition
  tr1->set_action(1, act1);
  
  // run will do nothing, because no events yet
  sm.run();
  
  // run will execute one transition
  sm.enqueue(0);
  uint16_t terminal_state = sm.run();
  
  // stay the same state, but no transition for 0+1
  EXPECT_EQ(terminal_state, 0);
}

TEST_F(FsmTest, TwoTransitionsOneLoop)
{
  state_machine sm("TEST",trace);
  
  transition::sptr tr1{new transition{0,66,0,"TR1"}};
  transition::sptr tr2{new transition{1,1,99,"TR2"}};
  
  action::sptr act1{new action{[](uint16_t seqno,
                                  transition & trans,
                                  state_machine & sm){
    sm.enqueue(1);
    trans.default_state(1);
  },"ACT1"}};
  
  tr1->set_action(1, act1);
  
  loop::sptr l1{new loop{[](uint16_t seqno,
                            transition & trans,
                            state_machine & sm,
                            uint64_t iteration) {
    if( iteration == 30 ) {
      sm.enqueue(10); // unknown transiiton
      trans.default_state(88);
      return false;
    }
    return true;
  }, "LOOP1"}};
  
  tr2->set_loop(33, l1);

  sm.add_transition(tr1);
  sm.add_transition(tr2);
  
  // run will execute one transition
  sm.enqueue(66);
  uint16_t terminal_state = sm.run(0);
  
  // stay the same state, but no transition for 0+1
  EXPECT_EQ(terminal_state, 88);
}

TEST_F(FsmTest, OneTransitionTimedLoop)
{
  state_machine sm("TEST",trace);
  
  transition::sptr tr1{new transition{0,1,0,"TR1"}};
  
  tr1->default_state(10);
  tr1->on_error_state(11);
  tr1->on_timeout_state(12);
  
  loop::sptr l1{new loop{[](uint16_t seqno,
                            transition & trans,
                            state_machine & sm,
                            uint64_t iteration) {
    // loop forever, timeout supposed to catch this issue
    return true;
  }, "LOOP1"}};
  
  timer::sptr t1{new timer{[](uint16_t seqno,
                              transition & trans,
                              state_machine & sm,
                              const timer::clock_type::time_point & started_at) {
    auto now = timer::clock_type::now();
    return now < started_at+std::chrono::milliseconds(100);
  }, "TIMER1"}};
  
  tr1->set_timer(11, t1);
  tr1->set_loop(33, l1);
  
  sm.add_transition(tr1);
  sm.enqueue(1);
  
  uint16_t terminal_state = sm.run(0);
  EXPECT_EQ(terminal_state, 12);
}

TEST_F(FsmTest, OneTransitionClearedTimerLoop)
{
  state_machine sm("TEST",trace);
  
  transition::sptr tr1{new transition{0,1,0,"TR1"}};
  
  tr1->default_state(10);
  tr1->on_error_state(11);
  tr1->on_timeout_state(12);
  
  loop::sptr l1{new loop{[](uint16_t seqno,
                            transition & trans,
                            state_machine & sm,
                            uint64_t iteration) {
    // loop for 100 which supposed to be smaller than the timeout below
    if( iteration < 100 )
      return true;
    else
      return false;
  }, "LOOP1"}};
  
  timer::sptr t1{new timer{[](uint16_t seqno,
                              transition & trans,
                              state_machine & sm,
                              const timer::clock_type::time_point & started_at) {
    auto now = timer::clock_type::now();
    return now < started_at+std::chrono::milliseconds(100);
  }, "TIMER1"}};
  
  tr1->set_timer(11, t1);
  tr1->set_loop(33, l1);
  tr1->clear_timer(55, 11);
  
  sm.add_transition(tr1);
  sm.enqueue(1);
  
  uint16_t terminal_state = sm.run(0);
  EXPECT_EQ(terminal_state, 10);
}

TEST_F(FsmTest, OneTransitionException)
{
  state_machine sm("TEST",trace);
  
  transition::sptr tr1{new transition{0,1,0,"TR1"}};
  
  tr1->default_state(10);
  tr1->on_error_state(11);
  tr1->on_timeout_state(12);
  
  loop::sptr l1{new loop{[](uint16_t seqno,
                            transition & trans,
                            state_machine & sm,
                            uint64_t iteration) {
    // loop for 100 which supposed to be smaller than the timeout below
    if( iteration < 100 )
      return true;
    else
    {
      THROW_("failed");
    }
  }, "LOOP1"}};
  
  timer::sptr t1{new timer{[](uint16_t seqno,
                              transition & trans,
                              state_machine & sm,
                              const timer::clock_type::time_point & started_at) {
    auto now = timer::clock_type::now();
    return now < started_at+std::chrono::milliseconds(100);
  }, "TIMER1"}};
  
  tr1->set_timer(11, t1);
  tr1->set_loop(33, l1);
  
  sm.add_transition(tr1);
  sm.enqueue(1);
  
  uint16_t terminal_state = sm.run(0);
  EXPECT_EQ(terminal_state, 11);
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  return ret;
}

