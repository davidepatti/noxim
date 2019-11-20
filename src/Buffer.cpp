/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the buffer
 */

#include "Buffer.h"
#include "Utils.h"

Buffer::Buffer()
{
  SetMaxBufferSize(GlobalParams::buffer_depth);
  max_occupancy = 0;
  hold_time = 0.0;
  last_event = 0.0;
  hold_time_sum = 0.0;
  previous_occupancy = 0;
  mean_occupancy = 0.0;
  true_buffer = true;
  full_cycles_counter = 0;
  last_front_flit_seq = NOT_VALID;
  deadlock_detected = false;
}


void Buffer::setLabel(string l)
{
    //cout << "\n BUFFER LABEL: " << l << endl;
    label = l;
}

string Buffer::getLabel() const
{
    return label;
}

void Buffer::Print()
{
    queue<Flit> m = buffer;

    string bstr = "";
   

    char  t[] = "HBT";

    cout << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << "\t";
    cout << label << " QUEUE *[";
    while (!(m.empty()))
    {
	Flit f = m.front();
	m.pop();
	cout << bstr << t[f.flit_type] << f.sequence_no <<  "(" << f.dst_id << ") | ";
    }
    cout << "]*" << endl;
    cout << endl;
}


void Buffer::deadlockCheck()
{
    // TOOD: add as parameter
    int check_threshold = 50000;

    if (IsEmpty()) return;

    Flit f = buffer.front();
    int seq = f.sequence_no;

    if (last_front_flit_seq==seq)
    {
	full_cycles_counter++;
    }
    else
    {
	if (deadlock_detected) 
	{
	    cout << " WRONG DEADLOCK detection, please increase the check_threshold " << endl;
	    assert(false);
	}
	last_front_flit_seq = seq;
	full_cycles_counter=0;
    }

    if (full_cycles_counter>check_threshold && !deadlock_detected) 
    {
	double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
	cout << "WARNING: DEADLOCK DETECTED at cycle " << current_time << " in buffer:  " << getLabel() << endl;
	deadlock_detected = true;
    }
}


bool Buffer::deadlockFree()
{
    if (IsEmpty()) return true;

    Flit f = buffer.front();
    
    int seq = f.sequence_no;


    if (last_front_flit_seq==seq)
    {
	full_cycles_counter++;
    }
    else
    {
	last_front_flit_seq = seq;
	full_cycles_counter=0;
    }

    if (full_cycles_counter>50000) 
    {
	return false;
    }

    return true;

}

void Buffer::Disable()
{
  true_buffer = false;
}

void Buffer::SetMaxBufferSize(const unsigned int bms)
{
  assert(bms > 0);

  max_buffer_size = bms;
}

unsigned int Buffer::GetMaxBufferSize() const
{
  return max_buffer_size;
}

bool Buffer::IsFull() const
{
  return buffer.size() == max_buffer_size;
}

bool Buffer::IsEmpty() const
{
  return buffer.size() == 0;
}

void Buffer::Drop(const Flit & flit) const
{
  assert(false);
}

void Buffer::Empty() const
{
  assert(false);
}

void Buffer::Push(const Flit & flit)
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

Flit Buffer::Pop()
{
  Flit f;

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

Flit Buffer::Front() const
{
  Flit f;

  if (IsEmpty())
    Empty();
  else
    f = buffer.front();

  return f;
}

unsigned int Buffer::Size() const
{
  return buffer.size();
}

unsigned int Buffer::getCurrentFreeSlots() const
{
  return (GetMaxBufferSize() - Size());
}

void Buffer::SaveOccupancyAndTime()
{
  previous_occupancy = buffer.size();
  hold_time = (sc_time_stamp().to_double() / GlobalParams::clock_period_ps) - last_event;
  last_event = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
}

void Buffer::UpdateMeanOccupancy()
{
  double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
  if (current_time - GlobalParams::reset_time < GlobalParams::stats_warm_up_time)
    return;

  mean_occupancy = mean_occupancy * (hold_time_sum/(hold_time_sum+hold_time)) +
    (1.0/(hold_time_sum+hold_time)) * hold_time * buffer.size();

  hold_time_sum += hold_time;
}

void Buffer::ShowStats(std::ostream & out)
{
  if (true_buffer)
    out << "\t" << mean_occupancy << "\t" << max_occupancy;
  else
    out << "\t\t";
}
