/*****************************************************************************

  TBuffer.h -- Buffer definition

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
#ifndef __TBUFFER_H__
#define __TBUFFER_H__

//---------------------------------------------------------------------------

#include <cassert>
#include <queue>
#include "NoximDefs.h"

using namespace std;

//---------------------------------------------------------------------------

class TBuffer
{
 public:

  TBuffer();

  virtual ~TBuffer() {}
  
  void SetMaxBufferSize(const unsigned int bms); // Set buffer max
						 // size (in flits)

  unsigned int GetMaxBufferSize() const; // Get max buffer size
  unsigned int getCurrentFreeSlots() const; // free buffer slots

  bool IsFull() const; // Returns true if buffer is full
  bool IsEmpty() const; // Returns true if buffer is empty

  virtual void Drop(const TFlit& flit) const; // Called by Push() when
					// buffer is full

  virtual void Empty() const; // Called by Pop() when buffer is empty

  void Push(const TFlit& flit); // Push a flit. Calls Drop method if
				// buffer is full.

  TFlit Pop(); // Pop a flit.

  TFlit Front() const; // Return a copy of the first flit in the buffer.

  unsigned int Size() const;

private:
  
  unsigned int max_buffer_size;
  queue<TFlit> buffer;
};

#endif
