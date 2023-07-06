#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/traffic-control-module.h"
#include "ns3/queue-disc.h"
#include "ns3/queue.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("p2-v3");

static bool firstCwnd = true;
static bool firstSshThr = true;
static bool firstRtt = true;
static bool firstRto = true;
static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static Ptr<OutputStreamWrapper> rttStream;
static Ptr<OutputStreamWrapper> rtoStream;
static Ptr<OutputStreamWrapper> nextTxStream;
static Ptr<OutputStreamWrapper> nextRxStream;
static Ptr<OutputStreamWrapper> inFlightStream;
static uint32_t cWndValue;
static uint32_t ssThreshValue;


static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
    {
      *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
      firstCwnd = false;
    }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
    {
      *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
    }
}


static void SsThreshTracer (uint32_t oldval, uint32_t newval)
{

  if (firstSshThr)
    {
      *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
      firstSshThr = false;
    }


  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
    {
      *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
    }
}

static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer (Time oldval, Time newval)
{
  if (firstRto)
    {
      *rtoStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto = false;
    }
  *rtoStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
NextTxTracer (SequenceNumber32 old, SequenceNumber32 nextTx)
{
  NS_UNUSED (old);
  *nextTxStream->GetStream () << Simulator::Now ().GetSeconds () << " " << nextTx << std::endl;
}

static void
InFlightTracer (uint32_t old, uint32_t inFlight)
{
  NS_UNUSED (old);
  *inFlightStream->GetStream () << Simulator::Now ().GetSeconds () << " " << inFlight << std::endl;
}

static void
NextRxTracer (SequenceNumber32 old, SequenceNumber32 nextRx)
{
  NS_UNUSED (old);
  *nextRxStream->GetStream () << Simulator::Now ().GetSeconds () << " " << nextRx << std::endl;
}

//METODOS Que llaman a los tracer. Primero, Se establece el archivo donde se escribe
//Luego, se hace utiliza ConnectWithoutContext del CONFIG. Esto recibe un path, y
//Un callback. El path se usa para encontrar el atributo a trazar, y el CallBack
//llama al tracer correspondiente.
//En el Path el NodeList son todos los nodos que se crean. TCPL4Protocol crea los
//sockets TCP y maneja los datos entre los sockets TCP de los nodos. SOCKETLIST son
//la lista de sockets asociado al protocolo. y el valor final, es el dato utilizado
//en la TRAZA. (trace source)

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow", MakeCallback (&CwndTracer));
  NS_LOG_UNCOND ("Trace cwnd");
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/SlowStartThreshold", MakeCallback (&SsThreshTracer));
  NS_LOG_UNCOND ("Trace ssthresh");
}

static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTT", MakeCallback (&RttTracer));
  NS_LOG_UNCOND ("Trace rtt");
}

static void
TraceRto (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RTO", MakeCallback (&RtoTracer));
  NS_LOG_UNCOND ("Trace rto");
}

static void
TraceNextTx (std::string &next_tx_seq_file_name)
{
  AsciiTraceHelper ascii;
  nextTxStream = ascii.CreateFileStream (next_tx_seq_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/NextTxSequence", MakeCallback (&NextTxTracer));
  NS_LOG_UNCOND ("Trace next tx");
}

static void
TraceInFlight (std::string &in_flight_file_name)
{
  AsciiTraceHelper ascii;
  inFlightStream = ascii.CreateFileStream (in_flight_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/BytesInFlight", MakeCallback (&InFlightTracer));
  NS_LOG_UNCOND ("Trace in flight");
}

static void
TraceNextRx (std::string &next_rx_seq_file_name)
{
  AsciiTraceHelper ascii;
  nextRxStream = ascii.CreateFileStream (next_rx_seq_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/RxBuffer/NextRxSequence", MakeCallback (&NextRxTracer));
  NS_LOG_UNCOND ("Trace next rx");
}


int main (int argc, char *argv[])
{
  std::string prefix_file_name = "";
  std::string cwndName = "cwnd";
  bool pcap = true;
  uint64_t data_mbytes = 400; 
  bool tracing = true;
  double duration = 150;
  uint16_t num_flows = 3;
  double start_time = 0.1;
  double stop_time = start_time + duration;

  CommandLine cmd;
  cmd.AddValue ("data", "Number of Megabytes of data to transmit", data_mbytes);
  cmd.AddValue ("pcap_tracing", "Enable or disable PCAP tracing", pcap);
 
  // Configurar TCP New Reno
  NS_LOG_UNCOND ("Configuration of TcpNewReno");
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  
 
  // Topologia dumbbell y las Ips de los nodos
  //
  // n0 -----+       +----- n5  |  10.0.2.1 ---------+          +--------- 10.0.3.2
  //         |       |          |                    |          |          
  // n1 -----n3 --- n4----- n6  |  10.0.4.1 -----10.0.1.1--- 10.0.1.2----- 10.0.5.2
  //         |       |          |                    |          |  
  // n2 -----+       +----- n7  |  10.0.6.2 ---------+          +--------- 10.0.7.2  

  //Se crean los nodos (emitters = emisor, receivers = receptores)
  NS_LOG_UNCOND ("Create nodes");
  NodeContainer gateways;
  gateways.Create (2);
  NodeContainer emitters;
  emitters.Create (num_flows);
  NodeContainer receivers;
  receivers.Create (num_flows);


  //Creamos los canales (PointToPoint)
  NS_LOG_UNCOND ("Create channels");
  PointToPointHelper rigthLink;
  rigthLink.SetDeviceAttribute("DataRate", StringValue("50MBps"));
  rigthLink.SetChannelAttribute("Delay", StringValue("1ms"));

  PointToPointHelper leftLink;
  leftLink.SetDeviceAttribute ("DataRate", StringValue ("50MBps"));
  leftLink.SetChannelAttribute ("Delay", StringValue ("1ms"));
  
  PointToPointHelper centerLink;
  centerLink.SetDeviceAttribute ("DataRate", StringValue ("50KBps"));
  centerLink.SetChannelAttribute ("Delay", StringValue ("10ms"));
  centerLink.Install (gateways.Get (0), gateways.Get (1));

  Ipv4InterfaceContainer sink_interfaces;

  //Instalo la pila de protocolos de Interne
  NS_LOG_UNCOND ("Install internet protocol stack");
  InternetStackHelper stack;
  stack.InstallAll ();

  //Establecemos la direcion IP base
  NS_LOG_UNCOND ("Stable IP address");
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");

  //Instalo el dispositivo red y asigno las direcciones IP a los gateway
  NS_LOG_UNCOND ("Install device and assing IP in gateway");
  NetDeviceContainer nodeDeviceGateway;
  nodeDeviceGateway = centerLink.Install (gateways.Get (0), gateways.Get (1));
  address.NewNetwork ();
  Ipv4InterfaceContainer intcont = address.Assign (nodeDeviceGateway);


  //Instalo los dispositivos red y asigno las direcciones IP a los emitters y receivers
  // 10.0.2.1 ---------+          +--------- 10.0.3.2
  //                   |          |          
  // 10.0.4.1 -----10.0.1.1--- 10.0.1.2----- 10.0.5.2
  //                   |          |  
  // 10.0.6.2 ---------+          +--------- 10.0.7.2  
  NS_LOG_UNCOND ("Install devices and assing IP in emitters & receivers");
  NetDeviceContainer devices;

  for (uint32_t i = 0; i < num_flows; i++)
    {
      devices = leftLink.Install (emitters.Get (i), gateways.Get (0));

      address.NewNetwork ();
      Ipv4InterfaceContainer interfaces = address.Assign (devices);

      devices = rigthLink.Install (gateways.Get (1), receivers.Get (i));
     
      address.NewNetwork ();
      interfaces = address.Assign (devices);
      sink_interfaces.Add (interfaces.Get (1));
  }


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  //Creo las aplicaciones apra los emisores y los receptores
  NS_LOG_UNCOND ("Create Applications");
  uint16_t port = 50000;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  PacketSinkHelper sinkHelper2 ("ns3::UdpSocketFactory", sinkLocalAddress);
  sinkHelper2.SetAttribute ("Protocol", TypeIdValue (UdpSocketFactory::GetTypeId ()));
  

  for (uint16_t i = 0; i < emitters.GetN (); i++)
    {
      //Se crean las apllicaciones UDP en nodos 1 de los emitters y receivers
      if(i == 1){
        OnOffHelper ftp ("ns3::UdpSocketFactory", InetSocketAddress (sink_interfaces.GetAddress (i, 0), port));

        ApplicationContainer sourceApp = ftp.Install (emitters.Get (i));
        sourceApp.Start (Seconds (start_time * i));
        sourceApp.Stop (Seconds (stop_time - 3));

        ApplicationContainer sinkApp = sinkHelper2.Install (receivers.Get (i));
        sinkApp.Start (Seconds (start_time * i));
        sinkApp.Stop (Seconds (stop_time));

      }
      //Se crean las apllicaciones TCP en nodos 0 y 2 de los emitters y receivers
      else{
        OnOffHelper ftp ("ns3::TcpSocketFactory", InetSocketAddress (sink_interfaces.GetAddress (i, 0), port));
        ftp.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        ftp.SetAttribute ("MaxBytes", UintegerValue (data_mbytes * 1000000));

        ApplicationContainer sourceApp = ftp.Install (emitters.Get (i));
        sourceApp.Start (Seconds (start_time * i));
        sourceApp.Stop (Seconds (stop_time ));

        ApplicationContainer sinkApp = sinkHelper.Install (receivers.Get (i));
        sinkApp.Start (Seconds (start_time * i));
        sinkApp.Stop (Seconds (stop_time));
        }
    }
 

  //Captura el trafico en un archivo pcap
  NS_LOG_UNCOND ("Capturing traffic");
  if (pcap)
  {
    rigthLink.EnablePcapAll ("trafico", true);    
  }

  //Crea el archivo animacion.xml para vizualizar en NetAnim
  NS_LOG_UNCOND ("Create the animacion.xml file for NetAnim");
  AnimationInterface anim ("animacion.xml");
  anim.SetConstantPosition(emitters.Get(0), 20, 10);
  anim.SetConstantPosition(emitters.Get(1), 20, 25);
  anim.SetConstantPosition(emitters.Get(2), 20, 40);
  anim.SetConstantPosition(gateways.Get(0), 40, 25);
  anim.SetConstantPosition(gateways.Get(1), 50, 25);
  anim.SetConstantPosition(receivers.Get(0), 70, 10);
  anim.SetConstantPosition(receivers.Get(1), 70, 25);
  anim.SetConstantPosition(receivers.Get(2), 70, 40);
  anim.EnablePacketMetadata(true);
    

  if (tracing)
    {
      std::ofstream ascii;
      Ptr<OutputStreamWrapper> ascii_wrap;
      ascii.open ((prefix_file_name + "ascii").c_str ());
      ascii_wrap = new OutputStreamWrapper ((prefix_file_name + "ascii").c_str (),
                                            std::ios::out);
      stack.EnableAsciiIpv4All (ascii_wrap);

      Simulator::Schedule (Seconds (0.00001), &TraceCwnd, cwndName + ".data");
      Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, prefix_file_name + "ssth.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "rtt.data");
      Simulator::Schedule (Seconds (0.00001), &TraceRto, prefix_file_name + "rto.data");
      Simulator::Schedule (Seconds (0.00001), &TraceNextTx, prefix_file_name + "next-tx.data");
      Simulator::Schedule (Seconds (0.00001), &TraceInFlight, prefix_file_name + "inflight.data");
      Simulator::Schedule (Seconds (0.1), &TraceNextRx, prefix_file_name + "next-rx.data");
    }
 

  NS_LOG_UNCOND ("Start simulation");
  Simulator::Stop (Seconds(duration));
  Simulator::Run ();
  NS_LOG_UNCOND ("Stop simulation");

  

  NS_LOG_UNCOND ("Print IPs");
  //Imprimo IP de los nodos gateway
  for (uint32_t i = 0; i < gateways.GetN(); i++) {
    Ipv4InterfaceAddress interfaceAddress = gateways.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0);
    std::cout << "Dirección IP del nodo gateway  " << i << ": " << interfaceAddress.GetLocal() << std::endl;
  }

  //Imprimo IP de los nodos emisores
  for (uint32_t i = 0; i < emitters.GetN(); i++) {
    Ipv4InterfaceAddress interfaceAddress = emitters.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0);
    std::cout << "Dirección IP del nodo emitter  " << i << ": " << interfaceAddress.GetLocal() << std::endl;
  }

  //Imprimo IP de los nodos receptores
  for (uint32_t i = 0; i < receivers.GetN(); i++) {
    Ipv4InterfaceAddress interfaceAddress = receivers.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0);
    std::cout << "Dirección IP del nodo receiver " << i << ": " << interfaceAddress.GetLocal() << std::endl;
  }

  Simulator::Destroy ();
  NS_LOG_UNCOND ("Done.");
}

