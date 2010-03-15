/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2010 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include "NoximBuffer.h"

NoximBuffer::NoximBuffer()
{
    SetMaxBufferSize(DEFAULT_BUFFER_DEPTH);
}

void NoximBuffer::SetMaxBufferSize(const unsigned int bms)
{
    assert(bms > 0);

    max_buffer_size = bms;
}

unsigned int NoximBuffer::GetMaxBufferSize() const
{
    return max_buffer_size;
}

bool NoximBuffer::IsFull() const
{
    return buffer.size() == max_buffer_size;
}

bool NoximBuffer::IsEmpty() const
{
    return buffer.size() == 0;
}

void NoximBuffer::Drop(const NoximFlit & flit) const
{
    assert(false);
}

void NoximBuffer::Empty() const
{
    assert(false);
}

void NoximBuffer::Push(const NoximFlit & flit)
{
    if (IsFull())
	Drop(flit);
    else
	buffer.push(flit);
}

NoximFlit NoximBuffer::Pop()
{
    NoximFlit f;

    if (IsEmpty())
	Empty();
    else {
	f = buffer.front();
	buffer.pop();
    }

    return f;
}

NoximFlit NoximBuffer::Front() const
{
    NoximFlit f;

    if (IsEmpty())
	Empty();
    else
	f = buffer.front();

    return f;
}

unsigned int NoximBuffer::Size() const
{
    return buffer.size();
}

unsigned int NoximBuffer::getCurrentFreeSlots() const
{
    return (GetMaxBufferSize() - Size());
}
