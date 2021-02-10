
#pragma once

#include "MachineStates.h"

class AbstractState;
class Machine
{
  friend class AbstractState;

public:
  Machine(unsigned int inStockQuantity);
  void sell(unsigned int quantity);
  void refill(unsigned int quantity);
  unsigned int getCurrentStock();
  ~Machine();

private:
  unsigned int mStockQuantity;
  AbstractState *mState;
};

Machine::Machine(unsigned int inStockQuantity)
    : mStockQuantity(inStockQuantity), mState(inStockQuantity > 0 ? new Normal() : new SoldOut())
{
}

Machine::~Machine()
{
  delete mState;
}

void Machine::sell(unsigned int quantity)
{
  mState->sell(*this, quantity);
}

void Machine::refill(unsigned int quantity)
{
  mState->refill(*this, quantity);
}

unsigned int Machine::getCurrentStock()
{
  return mStockQuantity;
}


