/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2015 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "ProcessingElement.h"
#include <string>

int ProcessingElement::randInt(int min, int max)
{
    return min +
	(int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}

void ProcessingElement::rxProcess()
{
    if (reset.read()) {
	ack_rx.write(0);
	current_level_rx = 0;
    } else {
	if (req_rx.read() == 1 - current_level_rx) {
	    Flit flit_tmp = flit_rx.read();
	    current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
	}
	ack_rx.write(current_level_rx);
    }
}

void ProcessingElement::txProcess()
{
    if (reset.read()) {
	req_tx.write(0);
	current_level_tx = 0;
        
	transmittedAtPreviousCycle = false;
        transmissionIsComplete = false;
        waitToSendPacket = false;      
        cycleNextPacket = 0;   
                        
        if(GlobalParams::traffic_distribution == TRAFFIC_APPLICATION){
            
            if(peDataFile.is_open()){
                if (GlobalParams::verbose_mode > VERBOSE_HIGH)
                    cout << "Instant Power file already opened" << endl;                      
            }else{
           
                if(GlobalParams::verbose_mode > VERBOSE_HIGH)
                    cout << "File opening of PE number : " << local_id << endl;

                std::string sId = int_to_string(local_id);
                std::string pathApp(GlobalParams::app_traffic_pathname); 
                fname = pathApp + "NoC_tracesIP" + sId;  //filename specific to one PE

                peDataFile.open(fname.c_str()); //The file is only opened once during simulation for a comptutation time optimisation purpose
                                                //The file will be automatically closed with ifstream destructor
                if(!peDataFile){
                    if(GlobalParams::verbose_mode > VERBOSE_HIGH)
                    cerr << "Error reading file : "<< fname << endl;
                    
                    if(GlobalParams::verbose_mode > VERBOSE_HIGH)
                        cout << "PE no " << local_id << " will not emit data during this simulation"<< endl;   
                    
                    never_transmit = true; //Same use as table traffic mode
                }else {
                    if(GlobalParams::verbose_mode > VERBOSE_HIGH)
                        cout << "File " << fname << " successfully openened" << endl;
                    never_transmit = false;
                }                 
            }
        }
        
    } else {
	Packet packet;
        bool shoot;
        double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
        
	if (shoot = canShot(packet)) {
 
            if(GlobalParams::traffic_distribution != TRAFFIC_APPLICATION){
                packet_queue.push(packet);
                transmittedAtPreviousCycle = true;
            } else {
                if(now >= cycleNextPacket){
                    packet_queue.push(packet);
                    transmittedAtPreviousCycle = true;
                } else {
                    transmittedAtPreviousCycle = false;
                }    
            }         
            
	} else 
	    transmittedAtPreviousCycle = false;
            
        

	if (ack_tx.read() == current_level_tx) {
	    if (!packet_queue.empty()) { 
		Flit flit = nextFlit();	// Generate a new flit 
		flit_tx->write(flit);	// Send the generated flit
		current_level_tx = 1 - current_level_tx; //Negate the old value for Alternating Bit Protocol (ABP)
		req_tx.write(current_level_tx);
                
               
	    } 
                
	}
        
       
    }
}

Flit ProcessingElement::nextFlit()
{
    Flit flit;
    Packet packet = packet_queue.front();

    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.sequence_length = packet.size;
    flit.hop_no = 0;

    if (packet.size == packet.flit_left)
	flit.flit_type = FLIT_TYPE_HEAD;
    else if (packet.flit_left == 1)
	flit.flit_type = FLIT_TYPE_TAIL;
    else
	flit.flit_type = FLIT_TYPE_BODY;
 
    
    //Erwan 22/06/15, payload to get a bit-accurate Noxim
    flit.payload.data = init_Payload(flit);
        
        
    packet_queue.front().flit_left--;
    if (packet_queue.front().flit_left == 0)
	packet_queue.pop();

    return flit;
}

/*  
 * EM Generate a flit's data in according to the flit's type
 */
sc_uint<MAX_FLIT_PAYLOAD> ProcessingElement::init_Payload(Flit flit)
{
    Payload flit_data;
    flit_data.data = 0x0000000000000000;
     
    if(GlobalParams::traffic_distribution == TRAFFIC_APPLICATION){
       
        if(GlobalParams::use_own_header){          
            flit_data.data = body_Payload(flit);           
        }else{
        
            if(flit.flit_type == FLIT_TYPE_HEAD){
                flit_data.data[0]=1; 
                flit_data.data[1]=0;

                //Get the binary dst and src of the packet
                string src = dec_to_bin(flit.src_id);
                string dst = dec_to_bin(flit.dst_id);           
                string pkt_size = dec_to_bin(flit.sequence_length);  
                assert(("Src can't possibly be higher than flits size", src.length()< GlobalParams::flit_size));
                assert(("Dst can't possibly be higher than flits size", dst.length()< GlobalParams::flit_size));
                assert(("Packet can't possibly be higher than flits size", pkt_size.length()< GlobalParams::flit_size));
                //And fill the head with
                unsigned int i;
                for(i=0; i < src.length();i++){
                    if(src[i] == '0')
                        flit_data.data[i+2] = 0;
                    else
                        flit_data.data[i+2] = 1;
                }         
                int start_dst = i+2;
                assert(("start_dst can't be negative", start_dst >=0));
                unsigned int j;        
                for(j=0; j < dst.length();j++){
                    if(dst[j] == '0')
                        flit_data.data[start_dst+j] = 0;
                    else
                        flit_data.data[start_dst+j] = 1;
                }
                //Add of the packet size
                int start_pkt_size = start_dst + j;
                assert(("start_pkt_size can't be negative", start_pkt_size >=0));
                for(unsigned int k=0; k < pkt_size.length();k++){
                    if(pkt_size[k] == '0')
                        flit_data.data[start_pkt_size+k] = 0;
                    else
                        flit_data.data[start_pkt_size+k] = 1;
                }

            } else{ 
                    if(flit.flit_type == FLIT_TYPE_TAIL){
                    flit_data.data[0]=0; 
                    flit_data.data[1]=1;
                    //The other bits stay at 0 for the tail     

                    } else{
                            flit_data.data = body_Payload(flit);                       
                    }
            }
            
        }
         
    }else{
        flit_data.data = body_Payload(flit);
    }    
        
   return flit_data.data;
}

/* 
 * EM Generate a flit's data in according to the payload_pattern
 */
sc_uint<MAX_FLIT_PAYLOAD> ProcessingElement::body_Payload(Flit flit)
{  
    
    if(GlobalParams::traffic_distribution == TRAFFIC_APPLICATION){
        
        if(GlobalParams::use_own_header){  
            sc_uint<MAX_FLIT_PAYLOAD> flit_data = packet_queue.front().packet_data[flit.sequence_no].data;
            flit.payload.data = flit_data;
        }else{
            sc_uint<MAX_FLIT_PAYLOAD> flit_data = packet_queue.front().packet_data[flit.sequence_no-1].data;
            flit.payload.data = flit_data;
        }
        
    } else{
    
        switch (GlobalParams::payload_pattern) {
                case PAYLOAD_0_BEST:
                    //Don't do anything, payloads' data is already initialized to 0
                    break;

                case PAYLOAD_100_WORST:
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size;itt++){                       
                            if(itt%2==0){
                                flit.payload.data[itt] = 1;
                            }                  
                        }  
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size;itt++){  
                            if(itt%2){
                                flit.payload.data[itt] = 1;
                            }                       
                        }
                    } 
                    break;

                case PAYLOAD_100_BEST:   
                    // Fill the flit's payload with bits that switching at the same time in the way
                    // In this case we keep switching activity at 1 
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size;itt++)
                                flit.payload.data[itt] = 1;               
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size;itt++)                   
                                flit.payload.data[itt] = 0;
                    }                
                break;

                case PAYLOAD_RANDOM:
                    for(int itt= 0; itt < GlobalParams::flit_size;itt++)
                        flit.payload.data[itt] = rand()%2;

                    break;

                case PAYLOAD_50_WORST:
                    // Fill the half of the flit's payload as it was worst_case and 0s the bits remaining
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size/2 ;itt++){
                            if(itt%2==0){
                                flit.payload.data[itt] = 1;
                            }               
                        }  
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size/2 ;itt++){  
                            if(itt%2){
                                flit.payload.data[itt] = 1;
                            }                     
                        }
                    } 
                    break;    

                case PAYLOAD_50_BEST:   
                    // Fill the half of the flit's payload with the half at 0 and others that switching at the same time in the way
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size/2 ;itt++)  
                                flit.payload.data[itt] = 1;               
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size/2 ;itt++)                     
                                flit.payload.data[itt] = 0;

                    }                
                break;

                case PAYLOAD_75_BEST:   
                    // Fill the half of the flit's payload with the half at 0 and others that switching at the same time in the way
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                       for(int itt= 0; itt < GlobalParams::flit_size*0.75 ;itt++)  
                                flit.payload.data[itt] = 1;               
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size*0.75 ;itt++)                     
                                flit.payload.data[itt] = 0;

                    }                
                break;

                case PAYLOAD_75_WORST:
                    // Fill the half of the flit's payload as it was worst_case and 0s the bits remaining
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size*0.75;itt++){                         
                            if(itt%2==0){
                                flit.payload.data[itt] = 1;
                            }               
                        }  
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size*0.75;itt++){  
                            if(itt%2){
                                flit.payload.data[itt] = 1;
                            }                     
                        }
                    } 
                    break; 

                     case PAYLOAD_25_BEST:   
                    // Fill the half of the flit's payload with the half at 0 and others that switching at the same time in the way
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size*0.25 ;itt++)  
                                flit.payload.data[itt] = 1;               
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size*0.25 ;itt++)                     
                                flit.payload.data[itt] = 0;

                    }                
                break;

                case PAYLOAD_25_WORST:
                    // Fill the half of the flit's payload as it was worst_case and 0s the bits remaining
                    // In this case we keep switching activity at 0.5 as PAYLOAD_RANDOM but with a different pattern
                    if((flit.sequence_no%2)==0){
                        for(int itt= 0; itt < GlobalParams::flit_size*0.25 ;itt++){                         
                            if(itt%2==0){
                                flit.payload.data[itt] = 1;
                            }               
                        }  
                    }else{
                        for(int itt= 0; itt < GlobalParams::flit_size*0.25 ;itt++){  
                            if(itt%2){
                                flit.payload.data[itt] = 1;
                            }                     
                        }
                    } 
                break; 

                default:
                    assert(false);
                }
        }
    
    return flit.payload.data;
}


bool ProcessingElement::canShot(Packet & packet)
{
    bool shot;
    double threshold;
    double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    
    if (GlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED && GlobalParams::traffic_distribution != TRAFFIC_APPLICATION) {
	if (!transmittedAtPreviousCycle)
	    threshold = GlobalParams::packet_injection_rate;
	else
	    threshold = GlobalParams::probability_of_retransmission;

	shot = (((double) rand()) / RAND_MAX < threshold);
	if (shot) {
	    switch (GlobalParams::traffic_distribution) {
	    case TRAFFIC_RANDOM:
		packet = trafficRandom();
		break;

	    case TRAFFIC_TRANSPOSE1:
		packet = trafficTranspose1();
		break;

	    case TRAFFIC_TRANSPOSE2:
		packet = trafficTranspose2();
		break;

	    case TRAFFIC_BIT_REVERSAL:
		packet = trafficBitReversal();
		break;

	    case TRAFFIC_SHUFFLE:
		packet = trafficShuffle();
		break;

	    case TRAFFIC_BUTTERFLY:
		packet = trafficButterfly();
		break;

	    case TRAFFIC_LOCAL:
		packet = trafficLocal();
		break;

	    default:
		assert(false);
	    }
	}
    } else if(GlobalParams::traffic_distribution == TRAFFIC_TABLE_BASED) { // Table based communication traffic
	if (never_transmit)
	    return false;

	bool use_pir = (transmittedAtPreviousCycle == false);
	vector < pair < int, double > > dst_prob;
	double threshold =
	    traffic_table->getCumulativePirPor(local_id, (int) now,
					       use_pir, dst_prob);

	double prob = (double) rand() / RAND_MAX;
	shot = (prob < threshold);
	if (shot) {
	    for (unsigned int i = 0; i < dst_prob.size(); i++) {
		if (prob < dst_prob[i].second) {
		    packet.make(local_id, dst_prob[i].first, now,
				getRandomSize());
		    break;
		}
	    }
	}
    } else {                         // Application based traffic
        if (never_transmit) 
	  return false;
        
        if (transmissionIsComplete)
          return false;
          
          if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)
          cout << "Simulation time in cycles : " << now << " Time to send : " << cycleNextPacket << endl; 
        
        if(packet_queue.empty()){           
            
            if(now < cycleNextPacket){  //If true, it's not the time to send the packet.
                 shot = false;
            } else {
                
                if(!waitToSendPacket){  //Test if we have already read the line 
                                        //and so if we wait the right time to send the packet 
                    getline(peDataFile, line);
                    
                    if(line[0]!='\0' ){

                        vector<string> readingPacket = splitString(line," ");
                        /*App files contain time to send in ns, 
                         * we must adapt it in cycles knowing noc frequency*/
                        cycleNextPacket = string_to_double(readingPacket[0])/(GlobalParams::clock_period_ps)*1000;
                        int dst = string_to_int(readingPacket[1]);
                        int size_data = 0;
                        int file_index = 0;
                        
                        if( GlobalParams::use_own_header){
                            packet.make(local_id, dst, cycleNextPacket, readingPacket.size()-2); 
                        } else {
                            packet.make(local_id, dst, cycleNextPacket, readingPacket.size()); 
                            //The packet size is the number of data flits + header and tail flits so readingPacket.size()-2 +2
                        }

                        for(unsigned int i=2; i<readingPacket.size(); i++){  //Data packets start at i=2
                            Payload dataFlit;
                            size_data = readingPacket[i].size();
                            file_index = size_data-1;
                            for(int j = 0; j < size_data; j++){ //Convert string to Payload (sc_uint)
                                //if(j>= (MAX_FLIT_PAYLOAD - size_data))
                                dataFlit.data[j] = (readingPacket[i][file_index]=='1')?1:0;
                                
                            file_index--;
                            }       
                            
                            packet.packet_data.push_back(dataFlit);

                            if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)            
                            cout << "Recording flit's payload : " << hex <<  packet.packet_data[i-2].data << dec << endl;
                        }

                        if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)            
                        cout << "dst : " << packet.dst_id << "  timestamp is : " << packet.timestamp << " Size : " << packet.size << endl;
                            

                        if(now < cycleNextPacket){
                            shot = false;
                            waitToSendPacket = true;
                            if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)    
                            cout << "Im waiting to send data" << endl;
                        } else {
                            shot = true;              
                        }

                    } else {        
                        transmissionIsComplete = true;  
                        if(GlobalParams::verbose_mode > VERBOSE_MEDIUM) 
                        cout << "transmission Complete !" << endl;
                        peDataFile.close();
                        shot = false;
                    }
                } else {    //It's time to send the packet!
                        
                    vector<string> readingPacket = splitString(line," ");
                    cycleNextPacket = string_to_double(readingPacket[0])/(GlobalParams::clock_period_ps)*1000;
                    int dst = string_to_int(readingPacket[1]);
                    int size_data = readingPacket[2].size();
                    int file_index = 0;
                    
                    if( GlobalParams::use_own_header){
                            packet.make(local_id, dst, cycleNextPacket, readingPacket.size()-2); 
                        } else {
                            packet.make(local_id, dst, cycleNextPacket, readingPacket.size()); 
                            //The packet size is the number of data flits + header and tail flits so readingPacket.size()-2 +2
                    }
                    
                    for(unsigned int i=2; i<readingPacket.size(); i++){  //Data packets start at i=2
                            Payload dataFlit;
                            size_data = readingPacket[i].size();
                            file_index = size_data-1;
                            for(int j = 0; j < size_data; j++){ //Convert string to Payload (sc_uint)
                                //if(j>= (MAX_FLIT_PAYLOAD - size_data))
                                dataFlit.data[j] = (readingPacket[i][file_index]=='1')?1:0;
                                
                            file_index--;
                            }    
                        
                        packet.packet_data.push_back(dataFlit);
                        
                        if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)            
                        cout << "Recording flit's payload : " << hex <<  packet.packet_data[i-2].data << dec << "  time : " << cycleNextPacket << endl;
                    }
                    
                    shot = true;
                    waitToSendPacket = false;   
                    
                    if(GlobalParams::verbose_mode > VERBOSE_MEDIUM)            
                    cout << "dst : " << packet.dst_id << "  timestamp is : " << packet.timestamp << " Size : " << packet.size << " Can shot ? " << shot << endl;
                    
                }         
            }
        }
    }  
    return shot;
}


Packet ProcessingElement::trafficLocal()
{
    Packet p;
    p.src_id = local_id;
    double rnd = rand() / (double) RAND_MAX;

    vector<int> dst_set;

    int max_id = (GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y) - 1;

    for (int i=0;i<max_id;i++)
    {
	if (rnd<=GlobalParams::locality)
	{
	    if (local_id!=i && sameRadioHub(local_id,i))
		dst_set.push_back(i);
	}
	else
	    if (!sameRadioHub(local_id,i))
		dst_set.push_back(i);
    }


    int i_rnd = rand()%dst_set.size();

    p.dst_id = dst_set[i_rnd];
    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;

}


Packet ProcessingElement::trafficRandom()
{
    Packet p;
    p.src_id = local_id;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;

    int max_id = (GlobalParams::mesh_dim_x * GlobalParams::mesh_dim_y) - 1;

    // Random destination distribution
    do {
	p.dst_id = randInt(0, max_id);

	// check for hotspot destination
	for (size_t i = 0; i < GlobalParams::hotspots.size(); i++) {

	    if (rnd >= range_start
		&& rnd <
		range_start + GlobalParams::hotspots[i].second) {
		if (local_id != GlobalParams::hotspots[i].first) {
		    p.dst_id = GlobalParams::hotspots[i].first;
		}
		break;
	    } else
		range_start += GlobalParams::hotspots[i].second;	// try next
	}
    } while (p.dst_id == p.src_id);

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}
// TODO: for testing only
Packet ProcessingElement::trafficTest()
{
    Packet p;
    p.src_id = local_id;
    p.dst_id = 10;

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

Packet ProcessingElement::trafficTranspose1()
{
    Packet p;
    p.src_id = local_id;
    Coord src, dst;

    // Transpose 1 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = GlobalParams::mesh_dim_x - 1 - src.y;
    dst.y = GlobalParams::mesh_dim_y - 1 - src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

Packet ProcessingElement::trafficTranspose2()
{
    Packet p;
    p.src_id = local_id;
    Coord src, dst;

    // Transpose 2 destination distribution
    src.x = id2Coord(p.src_id).x;
    src.y = id2Coord(p.src_id).y;
    dst.x = src.y;
    dst.y = src.x;
    fixRanges(src, dst);
    p.dst_id = coord2Id(dst);

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void ProcessingElement::setBit(int &x, int w, int v)
{
    int mask = 1 << w;

    if (v == 1)
	x = x | mask;
    else if (v == 0)
	x = x & ~mask;
    else
	assert(false);
}

int ProcessingElement::getBit(int x, int w)
{
    return (x >> w) & 1;
}

inline double ProcessingElement::log2ceil(double x)
{
    return ceil(log(x) / log(2.0));
}

Packet ProcessingElement::trafficBitReversal()
{

    int nbits =
	(int)
	log2ceil((double)
		 (GlobalParams::mesh_dim_x *
		  GlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits; i++)
	setBit(dnode, i, getBit(local_id, nbits - i - 1));

    Packet p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

Packet ProcessingElement::trafficShuffle()
{

    int nbits =
	(int)
	log2ceil((double)
		 (GlobalParams::mesh_dim_x *
		  GlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits - 1; i++)
	setBit(dnode, i + 1, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));

    Packet p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

Packet ProcessingElement::trafficButterfly()
{

    int nbits =
	(int)
	log2ceil((double)
		 (GlobalParams::mesh_dim_x *
		  GlobalParams::mesh_dim_y));
    int dnode = 0;
    for (int i = 1; i < nbits - 1; i++)
	setBit(dnode, i, getBit(local_id, i));
    setBit(dnode, 0, getBit(local_id, nbits - 1));
    setBit(dnode, nbits - 1, getBit(local_id, 0));

    Packet p;
    p.src_id = local_id;
    p.dst_id = dnode;

    p.timestamp = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;
    p.size = p.flit_left = getRandomSize();

    return p;
}

void ProcessingElement::fixRanges(const Coord src,
				       Coord & dst)
{
    // Fix ranges
    if (dst.x < 0)
	dst.x = 0;
    if (dst.y < 0)
	dst.y = 0;
    if (dst.x >= GlobalParams::mesh_dim_x)
	dst.x = GlobalParams::mesh_dim_x - 1;
    if (dst.y >= GlobalParams::mesh_dim_y)
	dst.y = GlobalParams::mesh_dim_y - 1;
}

int ProcessingElement::getRandomSize()
{
    return randInt(GlobalParams::min_packet_size,
		   GlobalParams::max_packet_size);
}
