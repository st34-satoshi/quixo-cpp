#include <iostream>

#include "global.h"
#include "state.h"
#include "encstate.h"
#include "tools.h"
#include "compute.h"



int main() {
  std::cout << std::setprecision(3) << std::fixed; // to display time
  
  initEncState();
  initOutcomeSize();
  computeOutcome();

  return 0;
}
  
