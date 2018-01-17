/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */
#ifndef __MM_H__
#define __MM_H__

#include <tlm>

class mm: public tlm::tlm_mm_interface
{
  typedef tlm::tlm_generic_payload gp_t;

public:
  mm() : free_list(0), empties(0) {}

  gp_t* allocate();
  void  free(gp_t* trans);

private:
  struct access
  {
    gp_t* trans;
    access* next;
    access* prev;
  };

  access* free_list;
  access* empties;

};

#endif
