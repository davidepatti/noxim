/*****************************************************************************

  TBlockOOO.cpp -- Blocking out of ordered packets (Murali et al. DAC'06)

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

#include "TBlockOOO.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TBlockOOO::TBlockOOO()
{
}

//---------------------------------------------------------------------------

void TBlockOOO::update(const int comm_id)
{
  map<int,int>::iterator i = expect_pkt_table.find(comm_id);

  assert(i != expect_pkt_table.end());

  expect_pkt_table[comm_id]++;
}

//---------------------------------------------------------------------------

int TBlockOOO::expectPacketSeqNo(const int comm_id)
{
  map<int,int>::iterator i = expect_pkt_table.find(comm_id);

  if (i == expect_pkt_table.end())
    expect_pkt_table[comm_id] = 1;

  return expect_pkt_table[comm_id];
}

//---------------------------------------------------------------------------
