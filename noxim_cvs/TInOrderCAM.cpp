/*****************************************************************************

  TInOrderCAM.cpp -- InOrder table implementation

 *****************************************************************************/
/* Copyright 2005-2007  
    Fabrizio Fazzino <fabrizio.fazzino@diit.unict.it>
    Maurizio Palesi <mpalesi@diit.unict.it>
    Davide Patti <dpatti@diit.unict.it>

 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "TInOrderCAM.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TInOrderCAM::TInOrderCAM()
{
  setCAMCapacity(0);
}

//---------------------------------------------------------------------------

void TInOrderCAM::setCAMCapacity(const unsigned int s)
{
  capacity = s;
}

//---------------------------------------------------------------------------

unsigned int TInOrderCAM::getCAMCapacity()
{
  return capacity;
}

//---------------------------------------------------------------------------

bool TInOrderCAM::isFull()
{
  return cam.size() >= capacity;
}

//---------------------------------------------------------------------------

bool TInOrderCAM::isCacheable(const int comm_id)
{
  if (cam.size() < capacity)
    return true;

  map<int,int>::iterator i = cam.find(comm_id);
  return (i != cam.end());
}

//---------------------------------------------------------------------------

void TInOrderCAM::insertEntry(const int comm_id, const int output)
{
  //  assert(isFull() == false);

  cam[comm_id] = output;
}

//---------------------------------------------------------------------------

int TInOrderCAM::getOutputPort(const int comm_id)
{
  map<int,int>::iterator i = cam.find(comm_id);

  return (i != cam.end())? i->second : -1;
}

//---------------------------------------------------------------------------

void TInOrderCAM::removeEntry(const int comm_id)
{
  map<int,int>::iterator i = cam.find(comm_id);

  if (i != cam.end()) 
    cam.erase(i);
}

//---------------------------------------------------------------------------

int TInOrderCAM::getCAMCurrentSize()
{
  return cam.size();
}

//---------------------------------------------------------------------------

int TInOrderCAM::showCAM()
{
  cout << "[";
  for (map<int,int>::iterator i=cam.begin(); i!=cam.end(); i++)
  {
    int comm_id = i->first;
    cout << (comm_id >> 24) << "->" << ((comm_id >> 16) & 0xff)
	 << " (" << (comm_id & 0xffff) << "), ";
  }
  cout << "]" << endl;
}

//---------------------------------------------------------------------------
