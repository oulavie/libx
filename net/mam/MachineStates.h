

#include "Machine.h"
#include <exception>
#include <stdexcept>

class Machine;

class AbstractState
{
public:
  virtual void sell(Machine &machine, unsigned int quantity) = 0;
  virtual void refill(Machine &machine, unsigned int quantity) = 0;
  virtual ~AbstractState();

protected:
  void setState(Machine &machine, AbstractState *st);
  void updateStock(Machine &machine, unsigned int quantity);
};

class Normal : public AbstractState
{
public:
  virtual void sell(Machine &machine, unsigned int quantity);
  virtual void refill(Machine &machine, unsigned int quantity);
  virtual ~Normal();
};

class SoldOut : public AbstractState
{
public:
  virtual void sell(Machine &machine, unsigned int quantity);
  virtual void refill(Machine &machine, unsigned int quantity);
  virtual ~SoldOut();
};

AbstractState::~AbstractState()
{
}

void AbstractState::setState(Machine &machine, AbstractState *state)
{
  AbstractState *aux = machine.mState;
  machine.mState = state;
  delete aux;
}

void AbstractState::updateStock(Machine &machine, unsigned int quantity)
{
  machine.mStockQuantity = quantity;
}

Normal::~Normal()
{
}

void Normal::sell(Machine &machine, unsigned int quantity)
{
  int currStock = machine.getCurrentStock();
  if (currStock < quantity)
  {
    throw std::runtime_error("Not enough stock");
  }

  updateStock(machine, currStock - quantity);

  if (machine.getCurrentStock() == 0)
  {
    setState(machine, new SoldOut());
  }
}

void Normal::refill(Machine &machine, unsigned int quantity)
{
  int currStock = machine.getCurrentStock();
  updateStock(machine, currStock + quantity);
}

SoldOut::~SoldOut()
{
}

void SoldOut::sell(Machine &machine, unsigned int quantity)
{
  throw std::runtime_error("Sold out!");
}

void SoldOut::refill(Machine &machine, unsigned int quantity)
{
  updateStock(machine, quantity);
  setState(machine, new Normal());
}
