Noxim - the NoC Simulator
=========================

Welcome to Noxim, the Network-on-Chip Simulator developed at the University of Catania (Italy).
The Noxim simulator is developed using SystemC, a system description language based on C++, and
it can be downloaded under GPL license terms.

**If you use Noxim in your research, we would appreciate the following citation in any publications to which it has contributed:**

Vincenzo Catania, Andrea Mineo, Salvatore Monteleone, Maurizio Palesi, and Davide Patti. 2016. Cycle-Accurate Network on Chip Simulation with Noxim. ACM Transactions on Modeling and Computer Simulation. Volume 27 Issue 1, November 2016. DOI: https://doi.org/10.1145/2953878



What's New ? 
------------
**[April 2017]** [Noxim tutorial slides](doc/noxim_tutorial.pdf) from lecture given at "Advanced Computer Architectures" (ELEC3219 - University of Southampton)

**[June 2015]** Massively improved version of Noxim. Major changes include:

  * Wireless transmission support
  * Hub connections for eterogeneous topologies
  * YAML configuration file for all features/parameters
  * Totally rewritten power model to support fine-grained estimation
  * Modular plugin-like addition of Routing/Selection strategies
  * Optional accurate logs for deep debugging (see DEBUG in Makefile)

Quick Start
-----------

If you are working on Ubuntu, you can install noxim and all the dependencies with the following command:
(be sure of copying the entire line, i.e., ending with "ubuntu.sh)

    bash <(wget -qO- --no-check-certificate https://raw.githubusercontent.com/davidepatti/noxim/master/other/setup/ubuntu.sh)

Or, to get just the latest master sources, you can run:

    git clone https://github.com/davidepatti/noxim.git

For further information please refer to the following files contained in the "doc" directory:

  * INSTALL.txt: instructions to install the tool
  * MANUAL.txt: explanation of the arguments that can be passed to the simulator
  * AUTHORS.txt: authors of the tool
  * LICENSE.txt: license under which you are allowed to use the source files

Noxim has a command line interface for defining several parameters of a NoC. In particular the
user can customize the network size, buffer size, packet size distribution, routing algorithm,
selection strategy, packet injection rate, traffic time distribution, traffic pattern, hot-spot
traffic distribution.

The simulator allows NoC evaluation in terms of throughput, delay and power consumption. This
information is delivered to the user both in terms of average and per-communication results.

In detail, the user is allowed to collect different evaluation metrics including the total number
of received packets/flits, global average throughput, max/min global delay, total energy consumption,
per-communications delay/throughput/energy etc.

The Noxim simulator is shipped along with Noxim Explorer, a tool useful during the design space
exploration phase. Infact, Noxim Explorer executes many simulations using Noxim in order to explore
the design space, and modifying the conﬁguration parameters for each simulation. Noxim Explorer will
create new configuration parameters for you, or complete the exploration according to the information
read from a script (known as exploration script or space file).


