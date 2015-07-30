#include "cpg.h"


#ifdef  TEST_CPG
int main() {
  Cpg cpg;
  for (size_t i = 0; i < 100; ++i)
  {
    cpg.step();
    std::cout<<cpg.angles()[0]<<std::endl;
  }
}
#endif
