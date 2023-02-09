#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/lte-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"
#include <math.h>
#include"ns3/node.h"
#include <iostream>
#include <fstream>
#include <vector>

using std::ofstream;

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lte_handover_scenario_manual_handover");

void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,			//IMSI -> International Mobile Subscriber Identity
                               uint16_t cellid,
                               uint16_t rnti)			//RNTI -> Radio Network Temporary Identifier
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " UE IMSI " << imsi
            << ": connected to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti,
                       uint16_t targetCellId)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " UE IMSI " << imsi
            << ": previously connected to CellId " << cellid
            << " with RNTI " << rnti
            << ", doing handover to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkUe (std::string context,
                       uint64_t imsi,
                       uint16_t cellid,
                       uint16_t rnti)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " UE IMSI " << imsi
            << ": successful handover to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

void
NotifyConnectionEstablishedEnb (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " eNB CellId " << cellid
            << ": successful connection of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}

void
NotifyHandoverStartEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " eNB CellId " << cellid
            << ": start handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << " to CellId " << targetCellId
            << std::endl;
}

void
NotifyHandoverEndOkEnb (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
{
  std::cout << Simulator::Now ().GetSeconds () << " " << context
            << " eNB CellId " << cellid
            << ": completed handover of UE with IMSI " << imsi
            << " RNTI " << rnti
            << std::endl;
}

ofstream outdata;
int count = 0;
void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon,Gnuplot2dDataset DataSet)
  {
    


    double localThrou=0;
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
      // Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->second);  //stats->first gives 1st flow...can use second to get 2nd flow [try it] 
      Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);                                  
       if (fiveTuple.sourceAddress == Ipv4Address("1.0.0.2") /*|| fiveTuple.sourceAddress == Ipv4Address("7.0.0.2") || fiveTuple.sourceAddress == Ipv4Address("7.0.0.3")*/)
      {
        
      std::cout<<"Flow ID     : " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
      //std::cout<<"Node ID : " << <<std::endl;
      std::cout<<"Tx Packets : " << stats->second.txPackets<<std::endl;
      std::cout<<"Rx Packets : " << stats->second.rxPackets<<std::endl;
      std::cout<<"Duration    : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
      std::cout<<"Last Received Packet  : "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
      std::cout<<"Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000  << " Kbps"<<std::endl;
      std::cout<< "Mean{Delay}: " << (stats->second.delaySum.GetSeconds()/stats->second.rxPackets) << "\n";
      std::cout<< "Mean{Jitter}: " << (stats->second.jitterSum.GetSeconds()/(stats->second.rxPackets)) << "\n";
      std::cout<< "Total{Delay}: " << (stats->second.delaySum.GetSeconds()) << "\n";
      std::cout<< "Total{Jitter}: " << (stats->second.jitterSum.GetSeconds()) << "\n";
      std::cout<< "Lost Packets: " << (stats->second.lostPackets) << "\n";
      std::cout<< "Dropped Packets: " << (stats->second.packetsDropped.size()) << "\n";
      if(stats->first == 2){
        outdata << 1 << ","<< count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 << "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 8){
        outdata << 2 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 14){
        outdata << 3 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 20){
        outdata << 4 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 26){
        outdata << 5 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 32){
        outdata << 6 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 38){
        outdata << 7 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 44){
        outdata << 8 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 50){
        outdata << 9 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 56){
        outdata << 10 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 62){
        outdata << 11 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 68){
        outdata << 12 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 74){
        outdata << 13 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 80){
        outdata << 14 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 86){
        outdata << 15 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 92){
        outdata << 16 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 98){
        outdata << 17 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 104){
        outdata << 18 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 110){
        outdata << 19 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 116){
        outdata << 20 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 122){
        outdata << 21 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 128){
        outdata << 22 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 134){
        outdata << 23 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 140){
        outdata << 24 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 146){
        outdata << 25 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 152){
        outdata << 26 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 158){
        outdata << 27 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 164){
        outdata << 28 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 170){
        outdata << 29 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 176){
        outdata << 30 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 182){
        outdata << 31 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 188){
        outdata << 32 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 194){
        outdata << 33 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 200){
        outdata << 34 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 206){
        outdata << 35 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 212){
        outdata << 36 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 218){
        outdata << 37 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 224){
        outdata << 38 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 230){
        outdata << 39 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 236){
        outdata << 40 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 242){
        outdata << 41 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 248){
        outdata << 42 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 254){
        outdata << 43 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 260){
        outdata << 44 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 266){
        outdata << 45 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 272){
        outdata << 46 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 278){
        outdata << 47 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 284){
        outdata << 48 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 290){
        outdata << 49 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 296){
        outdata << 50 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 304){
        outdata << 51 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 310){
        outdata << 52 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 316){
        outdata << 53 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 322){
        outdata << 54 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 328){
        outdata << 55 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 334){
        outdata << 56 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 340){
        outdata << 57 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 346){
        outdata << 58 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 352){
        outdata << 59 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 358){
        outdata << 60 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 364){
        outdata << 61 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 370){
        outdata << 62 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 374){
        outdata << 63 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }
      if(stats->first == 380){
        outdata << 64 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      }

      // if(stats->first == 386){
      //   outdata << 16 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      // }
      // if(stats->first == 392){
      //   outdata << 16 << ","<<  count << "," <<stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstRxPacket.GetSeconds())/1000 <<  "," <<(stats->second.delaySum.GetSeconds()/stats->second.rxPackets)*1000 <<std::endl;  
      // }



      
      
      localThrou=(stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024);
      // updata gnuplot data
            DataSet.Add((double)Simulator::Now().GetSeconds(),(double) localThrou);
      std::cout<<"---------------------------------------------------------------------------"<<std::endl;
      }
    }
      Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, flowMon,DataSet);
   //if(flowToXml)
      
  flowMon->SerializeToXmlFile ("ThroughputMonitor.flowmon", true, true);
      
      count++;
  }

int
main (int argc, char *argv[])
{
    outdata.open("SEexample16.txt");
    if (!outdata){
      std::cout<<"Error";
    }

  uint16_t numberOfUes = 16; //4-(30, 100)-done,16-(30, 100)-done,64- (300)-done
  uint16_t numberOfEnbs = 100;

  std::string traceFile;
  uint16_t numBearersPerUe = 2;
  bool disableDl = false;
  bool disableUl = false;

  Time simTime = Seconds(200.0);
  //double distance = 100.0;

  // change some default attributes so that they are reasonable for
  // this scenario, but do this before processing command line
  // arguments, so that the user is allowed to override these settings
  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (5)));  //changed(10->5) -- improved results
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (100000000));
  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue(320));      // allowed values: 2 5 10 20 40 80 160 320 
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (true));
  // Config::SetDefault ("ns3::LteUePhy::EnableRlfDetection", BooleanValue (false));
  // Command line arguments
  
  CommandLine cmd;
  cmd.AddValue ("traceFile", "Ns2 movement trace file", traceFile);
  cmd.Parse (argc, argv);

  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);

  NodeContainer ueNodes;
  ueNodes.Create(numberOfUes);

  ns2.Install();


  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  lteHelper->SetHandoverAlgorithmType ("ns3::A3RsrpHandoverAlgorithm");
  //lteHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm"); // disable automatic handover

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (3000));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);


  // Routing of the Internet Host (towards the LTE network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


  NodeContainer enbNodes;
  enbNodes.Create (numberOfEnbs);


  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  uint8_t line_count=0;
  uint8_t flag=0;
  std::string cell_towerID;
  std::string mytext;
  std::string token;
  std::ifstream ifs ("/home/jeet/repo/UltraViolet/mobility/towerlist.csv", std::ifstream::in);
  getline(ifs,mytext);
  while(getline(ifs,mytext))
  {
    
    line_count++;
    size_t pos = 0;
    flag=1;
    while ((pos = mytext.find(",")) != std::string::npos) {
    token = mytext.substr(0, pos);
    if (flag==1)
    {
      cell_towerID=token.substr(9,token.length());
      flag=0;
    }
    // std::cout << "cell_id:"<<cell_towerID << std::endl;
    //std::cout << "cell_id:"<<token << std::endl;

    mytext.erase(0, pos + 1);
    }
    /*std::cout << "cell_id:"<<mytext << std::endl;
    std::cout << typeid(token).name() << std::endl;
    std::cout << typeid(mytext).name() << std::endl;*/
  
    positionAlloc->Add (Vector(stod(token),stod(mytext),double(0)));
  }
// tower list reading over 
//............................................................

  MobilityHelper mobility;

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install (enbNodes);


  // Install LTE Devices in eNB and UEs
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));


  // Attach all UEs to the first eNodeB
  // for (uint16_t i = 0; i < numberOfUes; i++)
  //   {
  //     lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (0));
  //   }

  lteHelper->AttachToClosestEnb (ueLteDevs, enbLteDevs);


  NS_LOG_LOGIC ("setting up applications");

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 10000;
  uint16_t ulPort = 20000;


 // trace file, for data extraction
  AsciiTraceHelper ascii;
  p2ph.EnableAsciiAll (ascii.CreateFileStream ("tracehandover.tr"));
  // MobilityHelper::EnableAscii (ascii.CreateFileStream ("lena-x2-handover.mob"), ueNodes); //correct for a set of nodes
  mobility.EnableAscii (ascii.CreateFileStream ("lena-x2-handover.mob"), ueNodes); //correct for a set of nodes

  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
  startTimeSeconds->SetAttribute ("Min", DoubleValue (0.05));
  startTimeSeconds->SetAttribute ("Max", DoubleValue (0.06));
  for (uint32_t u = 0; u < numberOfUes; ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

      for (uint32_t b = 0; b < numBearersPerUe; ++b)
        {
          ApplicationContainer clientApps;
          ApplicationContainer serverApps;
          Ptr<EpcTft> tft = Create<EpcTft> ();

          if (!disableDl)
            {
              ++dlPort;

              NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
              UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
              clientApps.Add (dlClientHelper.Install (remoteHost));
              PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                                   InetSocketAddress (Ipv4Address::GetAny (), dlPort));
              serverApps.Add (dlPacketSinkHelper.Install (ue));

              EpcTft::PacketFilter dlpf;
              dlpf.localPortStart = dlPort;
              dlpf.localPortEnd = dlPort;
              tft->Add (dlpf);
            }

          if (!disableUl)
            {
              ++ulPort;

              NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
              UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
              clientApps.Add (ulClientHelper.Install (ue));
              PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                                   InetSocketAddress (Ipv4Address::GetAny (), ulPort));
              serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

              EpcTft::PacketFilter ulpf;
              ulpf.remotePortStart = ulPort;
              ulpf.remotePortEnd = ulPort;
              tft->Add (ulpf);
            }

          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

          Time startTime = Seconds (startTimeSeconds->GetValue ());
          serverApps.Start (startTime);
          clientApps.Start (startTime);
          clientApps.Stop (simTime);

        } // end for b
    }
  
  for (uint32_t u = 0; u < numberOfUes; ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
        {
          ++dlPort;
          ++ulPort;

          ApplicationContainer clientApps;
          ApplicationContainer serverApps;

          NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
          UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
          clientApps.Add (dlClientHelper.Install (remoteHost));
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
          UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          Ptr<EpcTft> tft = Create<EpcTft> ();
          EpcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPort;
          dlpf.localPortEnd = dlPort;
          tft->Add (dlpf);
          EpcTft::PacketFilter ulpf;
          ulpf.remotePortStart = ulPort;
          ulpf.remotePortEnd = ulPort;
          tft->Add (ulpf);
          EpsBearer bearer (EpsBearer::GBR_CONV_VOICE);
          lteHelper->ActivateDedicatedEpsBearer (ueLteDevs.Get (u), bearer, tft);

          Time startTime = Seconds (startTimeSeconds->GetValue ());
          serverApps.Start (startTime);
          clientApps.Start (startTime);

        }
    }


  // Add X2 inteface
    lteHelper->AddX2Interface (enbNodes);

  lteHelper->EnablePhyTraces ();
  lteHelper->EnableMacTraces ();
  lteHelper->EnableRlcTraces ();
  lteHelper->EnablePdcpTraces ();
   Ptr<RadioBearerStatsCalculator> rlcStats = lteHelper->GetRlcStats ();
   rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1)));
  Ptr<RadioBearerStatsCalculator> pdcpStats = lteHelper->GetPdcpStats ();
  pdcpStats->SetAttribute ("EpochDuration", TimeValue (Seconds (1)));

   //pointToPoint.EnablePcapAll ("blabla");


  // connect custom trace sinks for RRC connection establishment and handover notification
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                   MakeCallback (&NotifyHandoverStartUe));
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkEnb));
  Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                   MakeCallback (&NotifyHandoverEndOkUe));


  Simulator::Stop (simTime);
  AnimationInterface anim ("lte2.xml");

  uint32_t tower_img = anim.AddResource ("/home/shreya/ns-allinone-3.27/ns-3.27/scratch/tower.png");

//update the image for node with id number 3 with the resource we created
  anim.UpdateNodeImage (1, tower_img);
  anim.UpdateNodeImage (2, tower_img);
  
  
  //anim.EnablePacketMetadata ();
  anim.SetMaxPktsPerTraceFile (100000000000);
  anim.SetMobilityPollInterval(Seconds(1));
  //anim.EnablePacketMetadata(true);
    std::string fileNameWithNoExtension = "FlowVSThroughput_";
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Flow vs Throughput";
    std::string dataTitle               = "Throughput";

    // Instantiate the plot and set its title.
    Gnuplot gnuplot (graphicsFileName);
    gnuplot.SetTitle (plotTitle);

    // Make the graphics file, which the plot file will be when it
    // is used with Gnuplot, be a PNG file.
    gnuplot.SetTerminal ("png");

    // Set the labels for each axis.
    gnuplot.SetLegend ("Flow", "Throughput");

     
   Gnuplot2dDataset dataset;
   dataset.SetTitle (dataTitle);
   dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  //flowMonitor declaration
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> allMon = fmHelper.InstallAll();
  allMon->CheckForLostPackets ();
  // call the flow monitor function
  ThroughputMonitor(&fmHelper, allMon, dataset); 

  Simulator::Run ();

  //Gnuplot ...continued
  gnuplot.AddDataset (dataset);
  // Open the plot file.
  std::ofstream plotFile (plotFileName.c_str());
  // Write the plot file.
  gnuplot.GenerateOutput (plotFile);
  // Close the plot file.
  plotFile.close ();

  // GtkConfigStore config;
  // config.ConfigureAttributes ();
  

  Simulator::Destroy ();
  return 0;
}
 
