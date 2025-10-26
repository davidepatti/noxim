Noxim - the NoC Simulator
=========================

Welcome to Noxim, the Network-on-Chip Simulator developed at the University of Catania (Italy).
The Noxim simulator is developed using SystemC, a system description language based on C++, and
it can be downloaded under GPL license terms.

**If you use Noxim in your research, we would appreciate the following citation in any publications to which it has contributed:**

V. Catania, A. Mineo, S. Monteleone, M. Palesi and D. Patti, "Improving the energy efficiency of wireless Network on Chip architectures through online selective buffers and receivers shutdown," 2016 13th IEEE Annual Consumer Communications & Networking Conference (CCNC), Las Vegas, NV, 2016, pp. 668-673, doi: 10.1109/CCNC.2016.7444860.
[Scopus reference](https://www.scopus.com/record/display.uri?eid=2-s2.0-84966659566&origin=resultslist&sort=plf-f&src=s&sid=b531296d946a78b05f463c35c681a44c&sot=autdocs&sdt=autdocs&sl=18&s=AU-ID%2835610853000%29&relpos=14&citeCnt=6&searchTerm=)

V. Catania, A. Mineo, S. Monteleone, M. Palesi and D. Patti, "Energy efficient transceiver in wireless Network on Chip architectures," 2016 Design, Automation & Test in Europe Conference & Exhibition (DATE), Dresden, 2016, pp. 1321-1326.
[Scopus reference](https://www.scopus.com/record/display.uri?eid=2-s2.0-84973661681&origin=resultslist&sort=plf-f&src=s&sid=4bd3ffce04cc0093a84655249383aefa&sot=autdocs&sdt=autdocs&sl=18&s=AU-ID%2835610853000%29&relpos=11&citeCnt=11&searchTerm=)

Registration
------------
To receive information about new Noxim features, updates and events, please register here:
[Registration Form](https://docs.google.com/forms/d/e/1FAIpQLSfJnYQZwxC4gr4jUc-nuwuGp0MDBA-0N_TVf8hqV1DIa325Dg/viewform?c=0&w=1)

What's New ? 
------------

**COMING SOON**

A tool for visualizing what's happening in the NoC. Not just numbers, but a way to grasp visually what traffic flows are doing...

**[October 2025]**
 * Harcoded traffic for specifying what happens at each cycle
 * Fedora Linux script for installing
 * Many other stuff, sorry we don't update this readme often...
   
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

If you are on Fedora, use this command:

    bash <(wget -qO- --no-check-certificate https://raw.githubusercontent.com/davidepatti/noxim/master/other/setup/fedora.sh)

Similarly for macOS:

    /bin/zsh -c "$(curl -fsSL https://raw.githubusercontent.com/davidepatti/noxim/master/other/setup/macos.zsh)"

(when having issues with cmake, see https://github.com/davidepatti/noxim/issues/160)

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


