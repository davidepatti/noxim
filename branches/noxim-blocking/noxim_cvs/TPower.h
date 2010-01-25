/*****************************************************************************

  TPower.h -- Power model

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
#ifndef __TPOWER_H__
#define __TPOWER_H__

// ---------------------------------------------------------------------------
/*
The average energy dissipated by a flit for a hop switch was estimated
as being 0.151nJ, 0.178nJ, 0.182nJ and 0.189nJ for XY, Odd-Even, DyAD,
and NoP-OE respectively

We assumed the tile size to be 2mm x 2mm and that the tiles were
arranged in a regular fashion on the floorplan. The load wire
capacitance was set to 0.50fF per micron, so considering an average of
25% switching activity the amount of energy consumed by a flit for a
hop interconnect is 0.384nJ.


#define PWR_ROUTING_XY             0.151e-9
#define PWR_ROUTING_WEST_FIRST     0.155e-9
#define PWR_ROUTING_NORTH_LAST     0.155e-9
#define PWR_ROUTING_NEGATIVE_FIRST 0.155e-9
#define PWR_ROUTING_ODD_EVEN       0.178e-9
#define PWR_ROUTING_DYAD           0.182e-9
#define PWR_ROUTING_FULLY_ADAPTIVE 0.0
#define PWR_ROUTING_TABLE_BASED    0.185e-9

#define PWR_SEL_RANDOM             0.002e-9
#define PWR_SEL_BUFFER_LEVEL       0.006e-9
#define PWR_SEL_NOP                0.012e-9

#define PWR_FORWARD_FLIT           0.384e-9
#define PWR_INCOMING               0.002e-9
#define PWR_STANDBY                0.0001e-9/2.0

#define PWR_IO_CAM                 0.0
#define PWR_IO_BLOCKING            0.0
*/


#define F_CK        1000.0 // 1000 MHz = 1 GHz
#define uW2J        (1.0e-6/F_CK) * 1.0e-6

// Power values expressed in uW are converted in Joule
#define PWR_FIFO4   5576.0 * uW2J
#define PWR_ARBITER 629.0  * uW2J
#define PWR_XBAR    1231.0 * uW2J
#define PWR_WHRT    243.0  * uW2J

#define PWR_R_XY    10.0   * uW2J
#define PWR_R_OE    30.0   * uW2J
#define PWR_R_WF    -1     * uW2J
#define PWR_R_NL    -1     * uW2J
#define PWR_R_NF    -1     * uW2J
#define PWR_R_DY    -1     * uW2J
#define PWR_R_FA    -1     * uW2J
#define PWR_R_TB    -1     * uW2J

#define PWR_S_RND   161.0  * uW2J
#define PWR_S_BL    173.0  * uW2J
#define PWR_S_NOP   -1     * uW2J

#define PWR_CAM2    499.0  * uW2J * 0.2
#define PWR_CAM4    837.0  * uW2J * 0.2
#define PWR_CAM8    1597   * uW2J * 0.2
#define PWR_CAM16   2901   * uW2J * 0.2

#define PWR_MURALI  245    * uW2J

#define PWR_ROUTING_XY                PWR_R_XY
#define PWR_ROUTING_WEST_FIRST        PWR_R_WF
#define PWR_ROUTING_NORTH_LAST        PWR_R_NL
#define PWR_ROUTING_NEGATIVE_FIRST    PWR_R_NF
#define PWR_ROUTING_ODD_EVEN          PWR_R_OE
#define PWR_ROUTING_DYAD              PWR_R_DY
#define PWR_ROUTING_FULLY_ADAPTIVE    PWR_R_FA
#define PWR_ROUTING_TABLE_BASED       PWR_R_TB
#define PWR_LINK                      0.0

#define PWR_SEL_RANDOM                PWR_S_RND
#define PWR_SEL_BUFFER_LEVEL          PWR_S_BL
#define PWR_SEL_NOP                   PWR_S_NOP

#define PWR_IO_CAM                    PWR_CAM2
#define PWR_IO_BLOCKING               PWR_MURALI

#define PWR_FORWARD_FLIT              PWR_WHRT + PWR_ARBITER + PWR_XBAR + PWR_LINK
#define PWR_INCOMING                  PWR_FIFO4
#define PWR_STANDBY                   0.0


// ---------------------------------------------------------------------------

class TPower
{

 public:

  TPower();

  void Routing();
  void Selection();
  void Standby();
  void Forward();
  void Incoming();
  void InOrderStrategy();

  double getPower() { return pwr; }

  double getPwrRouting() { return pwr_routing; }
  double getPwrSelection() { return pwr_selection; }
  double getPwrForward() { return pwr_forward; }
  double getPwrStandBy() { return pwr_standby; }
  double getPwrIncoming() { return pwr_incoming; }

 private:

  double pwr_routing;
  double pwr_selection;
  double pwr_forward;
  double pwr_standby;
  double pwr_incoming;
  double pwr_io;

  double pwr;
};

// ---------------------------------------------------------------------------

#endif
