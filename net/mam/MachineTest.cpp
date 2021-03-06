

#include "Machine.h"
#include "MachineStates.h"
#include <iostream>
#include <stdexcept>

int main()
{
  Machine m(10), m2(0);

  m.sell(10);
  std::cout << "m: "
            << "Sold 10 items" << std::endl;

  try
  {
    m.sell(1);
  }
  catch (std::exception &e)
  {
    std::cerr << "m: " << e.what() << std::endl;
  }

  m.refill(20);
  std::cout << "m: "
            << "Refilled 20 items" << std::endl;

  m.sell(10);
  std::cout << "m: "
            << "Sold 10 items" << std::endl;
  std::cout << "m: "
            << "Remaining " << m.getCurrentStock() << " items" << std::endl;

  m.sell(5);
  std::cout << "m: "
            << "Sold 5 items" << std::endl;
  std::cout << "m: "
            << "Remaining " << m.getCurrentStock() << " items" << std::endl;

  try
  {
    m.sell(10);
  }
  catch (std::exception &e)
  {
    std::cerr << "m: " << e.what() << std::endl;
  }

  try
  {
    m2.sell(1);
  }
  catch (std::exception &e)
  {
    std::cerr << "m2: " << e.what() << std::endl;
  }

  return 0;
}
