#include <gtest/gtest.h>
#include <fsm/fsm.hh>
#include <future>
#include <iostream>
#include <string.h>
#include <map>

using namespace virtdb::fsm;

namespace virtdb { namespace test {
  
  class FsmTest : public ::testing::Test { };
  
}}

using namespace virtdb::test;

TEST_F(FsmTest, Dummy)
{
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  return ret;
}

