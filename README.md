# Sumo-NS3-UltraViolet
Integration of SUMO, NS3 and UltraViolet

Phase-1:
  1. Run SUMO simulation using OSMWebWizard.py
  2. Run traceexporter.py to get SUMO trace
Phase-2:
  1. Run Ns3 simulation from /home/jeet/ns3.29/scratch/
     ./waf --run "sumo_ns3_handover"
     
     [ The file you need for Ns3 code --> sumo_ns3_handover.cc 
       From here you can get latency + bandwidth in a csv file ]
Phase-3:
  1. Run UltraViolet experiment using d20 / d272 deployment
  2. Observe the bandwith + latency using "iperf + ping" commands
 
     
  if(stats->first == 380){
        outdata << 64 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
