/*****************************************************************************

  TBuffer.cpp -- Buffer implementation

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
#include "TBuffer.h"

//---------------------------------------------------------------------------

TBuffer::TBuffer()
{
  SetMaxBufferSize(DEFAULT_BUFFER_DEPTH);
}

//---------------------------------------------------------------------------

void TBuffer::SetMaxBufferSize(const unsigned int bms)
{
  assert(bms > 0);

  max_buffer_size = bms;
}

//---------------------------------------------------------------------------

unsigned int TBuffer::GetMaxBufferSize() const
{
  return max_buffer_size;
}

//---------------------------------------------------------------------------

bool TBuffer::IsFull() const
{
  return buffer.size() == max_buffer_size;
}

//---------------------------------------------------------------------------

bool TBuffer::IsEmpty() const
{
  return buffer.size() == 0;
}

//---------------------------------------------------------------------------

void TBuffer::Drop(const TFlit& flit) const
{
  assert(false);
}

//---------------------------------------------------------------------------

void TBuffer::Empty() const
{
  assert(false);
}

//---------------------------------------------------------------------------

void TBuffer::Push(const TFlit& flit)
{
  if (IsFull())
    Drop(flit);
  else
    buffer.push(flit);
}

//---------------------------------------------------------------------------

TFlit TBuffer::Pop()
{
  TFlit f;

  if (IsEmpty())
    Empty();
  else
    {
      f = buffer.front();
      buffer.pop();
    }

  return f;
}

//---------------------------------------------------------------------------

TFlit TBuffer::Front() const
{
  TFlit f;

  if (IsEmpty())
    Empty();
  else
    f = buffer.front();

  return f;
}

//---------------------------------------------------------------------------

unsigned int TBuffer::Size() const
{
  return buffer.size();
}

//---------------------------------------------------------------------------
unsigned int TBuffer::getCurrentFreeSlots() const
{
    return (GetMaxBufferSize()-Size());
}
//---------------------------------------------------------------------------
