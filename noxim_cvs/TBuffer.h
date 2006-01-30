/*****************************************************************************

  TBuffer.h -- Buffer definition

 *****************************************************************************/

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
