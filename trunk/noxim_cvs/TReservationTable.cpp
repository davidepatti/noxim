/*****************************************************************************

  TReservationTable.cpp -- TReservationTable implementation

 *****************************************************************************/

#include "NoximDefs.h"
#include "TReservationTable.h"

//---------------------------------------------------------------------------

TReservationTable::TReservationTable()
{
  clear();
}

//---------------------------------------------------------------------------

void TReservationTable::clear()
{
  rtable.resize(DIRECTIONS+1);
  
  for (int i=0; i<DIRECTIONS+1; i++)
    rtable[i] = NOT_RESERVED;
}

//---------------------------------------------------------------------------

bool TReservationTable::isReserved(const int port_out)
{
  assert(port_out >= 0 && port_out < DIRECTIONS+1);

  return (rtable[port_out] != NOT_RESERVED);
}

//---------------------------------------------------------------------------

void TReservationTable::reserve(const int port_in, const int port_out)
{

  int o = getOutputPort(port_in);

  if (o != NOT_RESERVED)
    release(o);

  rtable[port_out] = port_in;
}

//---------------------------------------------------------------------------

void TReservationTable::release(const int port_out)
{
  assert(isReserved(port_out));

  rtable[port_out] = NOT_RESERVED;
}

//---------------------------------------------------------------------------

int TReservationTable::getOutputPort(const int port_in)
{
  for (int i=0; i<DIRECTIONS+1; i++)
    if (rtable[i] == port_in)
      return i;

  return NOT_RESERVED;
}

//---------------------------------------------------------------------------




