#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
#include <math.h>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("lte_basic");

int main (int argc, char *argv[])
{

  uint16_t numofUes = 1;
  uint16_t numofEnbs = 2;
  Time simTime = MilliSeconds (1000);

  // To use EPC with LTE  
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper ->SetEpcHelper (epcHelper);
  
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  
  // Create a Single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);
  

  // Create Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");  
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
//Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  // Routing of Internet Host 
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting;
  remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
//remoteHostStaticRouting->AddNetworkRouteTo (epcHelper->GetEpcIpv4NetworkAddress (),Ipv4Mask ("255.255.0.0"), 1);
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"),1);


  // Creating UEs and EnBs
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create (numofEnbs);
  ueNodes.Create (numofUes);

  // Installing Mobility for the EnBs
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();        
  for (uint16_t i = 0; i<numofEnbs; i++)
  {
     // X,Y co-ordinates for real-life LTE Towers --> (273.890279027517, 127.135688700248) (124.842975830659, 125.632829552051) (1005.28760875657, 1502.71367422282) (1046.76811661676, 1611.70460214559)
     if (i==0){
       //positionAlloc->Add (Vector (1005.28760875657, 1502.71367422282, 0));
	positionAlloc->Add (Vector (100, 0 ,0));
     }
     else {
       //int j=0;
	positionAlloc->Add (Vector (0, 100,0));
//       positionAlloc->Add (Vector  (1046.76811661676, 1611.70460214559, 0));
     }
  }

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);
  

  // Installing Mobility for UEs
  /*Ptr<ListPositionAllocator> uepositionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i<numofUes; i++)
  { 
    positionAlloc->Add (Vector (1424.22, 1658.15, 0));
  }*/
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (-50.0),
                                 "MinY", DoubleValue (50.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (ueNodes);
  

  // Install LTE protocol stack on enBs and UEs
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
  
  // Install IP Stack on UEs
  internet.Install (ueNodes);
  
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  // Attach UE to closest enB
  lteHelper->AttachToClosestEnb (ueLteDevs, enbLteDevs);

  // Set default gateway for the UE
  for (uint32_t u = 0; u < numofUes; ++u){
    Ptr<Node> ue = ueNodes.Get (u);
    Ptr<Ipv4StaticRouting> ueStaticRouting;
    ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
    ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    Ptr<EpcTft> tft = Create<EpcTft> ();
    EpcTft::PacketFilter pf;
    pf.localPortStart = 1234;
    pf.localPortEnd = 1234;
    tft->Add (pf);
    lteHelper->ActivateDedicatedEpsBearer (ueLteDevs,EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT),tft);
  
    uint16_t dlPort = 1234;
    PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), dlPort));
    ApplicationContainer serverApps = packetSinkHelper.Install (ue);
    serverApps.Start (Seconds (0.01));
    UdpClientHelper client (ueIpIfaces.GetAddress (0), dlPort);
    ApplicationContainer clientApps = client.Install (remoteHost);
    clientApps.Start (Seconds (0.01));
  } 
  


  Simulator::Stop (simTime);
  AnimationInterface anim ("my_lte_basic.xml");
  //anim.SetMobilityPollInterval(Seconds(1));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
