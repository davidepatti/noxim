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
  max_occupancy = 0;
  hold_time = 0.0;
  last_event = 0.0;
  hold_time_sum = 0.0;
  previous_occupancy = 0;
  mean_occupancy = 0.0;
  true_buffer = true;
}

void NoximBuffer::Disable()
{
  true_buffer = false;
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
  SaveOccupancyAndTime();

  if (IsFull())
    Drop(flit);
  else
    buffer.push(flit);
  
  UpdateMeanOccupancy();

  if (max_occupancy < buffer.size())
    max_occupancy = buffer.size();
}

NoximFlit NoximBuffer::Pop()
{
  NoximFlit f;

  SaveOccupancyAndTime();

  if (IsEmpty())
    Empty();
  else {
    f = buffer.front();
    buffer.pop();
  }

  UpdateMeanOccupancy();

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

void NoximBuffer::SaveOccupancyAndTime()
{
  previous_occupancy = buffer.size();
  hold_time = (sc_time_stamp().to_double() / 1000) - last_event;
  last_event = sc_time_stamp().to_double() / 1000;
}

void NoximBuffer::UpdateMeanOccupancy()
{
  double current_time = sc_time_stamp().to_double() / 1000;
  if (current_time - DEFAULT_RESET_TIME < NoximGlobalParams::stats_warm_up_time)
    return;

  mean_occupancy = mean_occupancy * (hold_time_sum/(hold_time_sum+hold_time)) +
    (1.0/(hold_time_sum+hold_time)) * hold_time * buffer.size();

  hold_time_sum += hold_time;
}

void NoximBuffer::ShowStats(std::ostream & out)
{
  if (true_buffer)
    out << "\t" << mean_occupancy << "\t" << max_occupancy;
  else
    out << "\t\t";
}
