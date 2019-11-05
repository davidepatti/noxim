Noxim - the NoC Simulator
=========================

Welcome to Noxim, the Network-on-Chip Simulator developed at the University of Catania (Italy).
The Noxim simulator is developed using SystemC, a system description language based on C++, and
it can be downloaded under GPL license terms.

**If you use Noxim in your research, we would appreciate the following citation in any publications to which it has contributed:**

Catania, V., Mineo, A., Monteleone, S., Palesi, M., Patti, D.
Improving energy efficiency in wireless network-on-chip architectures
(2017) ACM Journal on Emerging Technologies in Computing Systems, 14 (1)

[paper reference on Scopus](https://www.scopus.com/inward/record.uri?eid=2-s2.0-85040309279&doi=10.1145%2f3138807&partnerID=40&md5=1a9c807853329ad6985a1cdb1e68c223)

Registration
------------
To receive information about new Noxim features, updates and events, please register here:
[Registration Form](https://docs.google.com/forms/d/e/1FAIpQLSfJnYQZwxC4gr4jUc-nuwuGp0MDBA-0N_TVf8hqV1DIa325Dg/viewform?c=0&w=1)

What's New ? 
------------
**[March 2018]**

  * Support for virtual channels for improved traffic management
  * Support for multiple radio-frequency channels for each Radio-Hub
  * New yaml examples (please update yours, since format is slightly different)
  
**[April 2017]** [Noxim tutorial slides](doc/noxim_tutorial.pdf) from lecture given at "Advanced Computer Architectures" (ELEC3219 - University of Southampton)

**[June 2015]** Massively improved version of Noxim. Major changes include:

  * Wireless transmission support
  * Hub connections for eterogeneous topologies
  * YAML configuration file for all features/parameters
  * Totally rewritten power model to support fine-grained estimation
  * Modular plugin-like addition of Routing/Selection strategies
  * Optional accurate logs for deep debugging (see DEBUG in Makefile)

Installation & Quick Start
-----------

If you are working on Ubuntu, you can install noxim and all the dependencies with the following command:
(**BE sure of copying the entire line, i.e., ending with "ubuntu.sh**)

    bash <(wget -qO- --no-check-certificate https://raw.githubusercontent.com/davidepatti/noxim/master/other/setup/ubuntu.sh)

Or, to get just the latest master sources, you can run:

    git clone https://github.com/davidepatti/noxim.git

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


