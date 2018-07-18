/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "DudpClient.h"
#include "http-header.h"
#include "para.h"
#include "mpeg-player.h"
using namespace std;


vector<vector<double>> udp_seg_size;
vector<vector<double>> udp_seg_size2;
vector<vector<double>> udp_seg_size3;

vector<vector<int>> udp_series;
bool T_play=false;
bool T_start=false;
double t_bw;
int p_num=0;
double p_inter;
int count_packet;
bool loss_mark=false;

int mpeg_play = -1; 
int now_seg=-1; // in para.h record now Server send packet for which segment
int p_seg; //record segmentID in packet
/*
inline MpegPlayer & GetPlayer()
{
      return u_player;
}
*/

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DudpClientApplication");

NS_OBJECT_ENSURE_REGISTERED (DudpClient);

TypeId
DudpClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DudpClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<DudpClient> ()
    .AddAttribute ("MaxPackets", 
                   "The maximum number of packets the application will send",
                   UintegerValue (100),
                   MakeUintegerAccessor (&DudpClient::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("VideoId",
                   "The Id of the video that is played.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&DudpClient::m_videoId),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Interval", 
                   "The time to wait between packets",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&DudpClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemoteAddress", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&DudpClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", 
                   "The destination port of the outbound packets",
                   UintegerValue (0),
                   MakeUintegerAccessor (&DudpClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize", "Size of echo data in outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&DudpClient::SetDataSize,
                                         &DudpClient::GetDataSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&DudpClient::m_txTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

DudpClient::DudpClient ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
  m_data = 0;
  m_dataSize = 0;
  m_segmentId = 0;
}

DudpClient::~DudpClient()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;

  delete [] m_data;
  m_data = 0;
  m_dataSize = 0;
}

void 
DudpClient::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void 
DudpClient::SetRemote (Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void
DudpClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
DudpClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

//  cout <<"user:  " << m_videoId<< endl; //Jerry

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else
        {
          NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&DudpClient::HandleRead, this));
  m_socket->SetAllowBroadcast (true);
  CheckStart ();
//  ScheduleTransmit (Seconds (0.));
  


}


void
DudpClient::CheckStart (){
    if(T_start==false){
        Simulator::Schedule(Seconds(0.001), &DudpClient::CheckStart, this);
    }
    else 
	ScheduleTransmit (Seconds (0.));
}


void 
DudpClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = 0;
    }

  Simulator::Cancel (m_sendEvent);
}

void 
DudpClient::SetDataSize (uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << dataSize);

  //
  // If the client is setting the echo packet data size this way, we infer
  // that she doesn't care about the contents of the packet at all, so 
  // neither will we.
  //
  delete [] m_data;
  m_data = 0;
  m_dataSize = 0;
  m_size = dataSize;
}

uint32_t 
DudpClient::GetDataSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_size;
}

void 
DudpClient::SetFill (std::string fill)
{
  NS_LOG_FUNCTION (this << fill);

  uint32_t dataSize = fill.size () + 1;

  if (dataSize != m_dataSize)
    {
      delete [] m_data;
      m_data = new uint8_t [dataSize];
      m_dataSize = dataSize;
    }

  memcpy (m_data, fill.c_str (), dataSize);

  //
  // Overwrite packet size attribute.
  //
  m_size = dataSize;
}

void 
DudpClient::SetFill (uint8_t fill, uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << fill << dataSize);
  if (dataSize != m_dataSize)
    {
      delete [] m_data;
      m_data = new uint8_t [dataSize];
      m_dataSize = dataSize;
    }

  memset (m_data, fill, dataSize);

  //
  // Overwrite packet size attribute.
  //
  m_size = dataSize;
}

void 
DudpClient::SetFill (uint8_t *fill, uint32_t fillSize, uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << fill << fillSize << dataSize);
  if (dataSize != m_dataSize)
    {
      delete [] m_data;
      m_data = new uint8_t [dataSize];
      m_dataSize = dataSize;
    }

  if (fillSize >= dataSize)
    {
      memcpy (m_data, fill, dataSize);
      m_size = dataSize;
      return;
    }

  //
  // Do all but the final fill.
  //
  uint32_t filled = 0;
  while (filled + fillSize < dataSize)
    {
      memcpy (&m_data[filled], fill, fillSize);
      filled += fillSize;
    }

  //
  // Last fill may be partial
  //
  memcpy (&m_data[filled], fill, dataSize - filled);

  //
  // Overwrite packet size attribute.
  //
  m_size = dataSize;
}

void 
DudpClient::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  m_sendEvent = Simulator::Schedule (dt, &DudpClient::Send, this);
}

void 
DudpClient::Send (void)
{

  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> p;
  if (m_dataSize)
    {
      //
      // If m_dataSize is non-zero, we have a data buffer of the same size that we
      // are expected to copy and send.  This state of affairs is created if one of
      // the Fill functions is called.  In this case, m_size must have been set
      // to agree with m_dataSize
      //
      NS_ASSERT_MSG (m_dataSize == m_size, "DudpClient::Send(): m_size and m_dataSize inconsistent");
      NS_ASSERT_MSG (m_data, "DudpClient::Send(): m_dataSize but no m_data");
      p = Create<Packet> (m_data, m_dataSize);
    }
  else
    {
      //
      // If m_dataSize is zero, the client has indicated that it doesn't care
      // about the data itself either by specifying the data size by setting
      // the corresponding attribute or by not calling a SetFill function.  In
      // this case, we don't worry about it either.  But we do allow m_size
      // to have a value different from the (zero) m_dataSize.
      //
      p = Create<Packet> (m_size);
    }
  // call to the trace sinks before the packet is actually sent,
  // so that tags added to the packet can be sent as well

 double B_remain = t_bw*1000000 - TP;
 double each_time = 1346.0*8/B_remain;
 
  

 double sum_bytes=0;
 sum_bytes = udp_seg_size[0][m_segmentId];
// cout << "high size: "<< sum_bytes<<endl;
 sum_bytes = sum_bytes/1288.0;
 

 double sum_bytes2=0;
 sum_bytes2 = udp_seg_size2[0][m_segmentId];
// cout << "medium size: "<< sum_bytes2<<endl; // Jerry
 sum_bytes2 = sum_bytes2/1288.0;

 double sum_bytes3=0;
 sum_bytes3 = udp_seg_size3[0][m_segmentId];
 sum_bytes3 = sum_bytes3/1288.0; 


 int packet_number=0;
 if (sum_bytes-int(sum_bytes) ==0)
        packet_number = int(sum_bytes);
 else
        packet_number = int(sum_bytes)+1;
 
 if (packet_number==0) packet_number++;


 int packet_number2=0;
 if (sum_bytes2-int(sum_bytes2) ==0)
        packet_number2 = int(sum_bytes2);
 else
        packet_number2 = int(sum_bytes2)+1;

 if (packet_number2==0) packet_number2++;


 int packet_number3=0;
 if (sum_bytes3-int(sum_bytes3) ==0)
        packet_number3 = int(sum_bytes3);
 else
        packet_number3 = int(sum_bytes3)+1;

 if (packet_number3==0) packet_number3++;

 int quality = -1 ; // 1 high 2 meduim 3 low

// if (packet_number>0 || packet_number2>0 || packet_number3>0) {

 double interval = 0.0;
 double p_size = 0.0; //record packet size
// int quality = -1 ; // 1 high 2 meduim 3 low
 if (1-packet_number*each_time >=0){
	interval = (1-packet_number*each_time)/packet_number;
	udp_series [0][m_segmentId] = packet_number;
	quality=1;
	p_size = udp_seg_size[0][m_segmentId];
	cout <<"quality:  "<<"high"<<endl;
       }
 else if (1-packet_number2*each_time >=0) {
	interval = (1-packet_number2*each_time)/packet_number2;
	udp_series [0][m_segmentId] = packet_number2;
	quality=2;
	p_size = udp_seg_size2[0][m_segmentId];
	cout <<"quality:  "<<"medium"<<endl;
	}
// else if (1-packet_number3*each_time >=0){
 else {
	interval = (1-packet_number3*each_time)/packet_number3;
	udp_series [0][m_segmentId] = packet_number3;
	quality=3;
	p_size = udp_seg_size3[0][m_segmentId];
	cout <<"quality:  "<<"low"<<endl;
      }
/* 
 if (interval<0)
	cout<<"no appropriate quality"<<endl;
*/

 cout << "interval:    "<< interval <<endl; //Jerry
 if (interval> each_time)
 	p_inter = interval;
 else
	p_inter = each_time; 
 interval = -1; 

// udp_series [0][m_segmentId] = packet_number;

 //cout <<"segment: "<< m_segmentId+1 << " total: "<<sum_bytes<<endl; 

 HTTPHeader httpHeader;
 httpHeader.SetVideoId(m_videoId);
 httpHeader.SetSegmentId(m_sent);
 httpHeader.SetPacketNum(udp_series [0][m_segmentId]);
 p->AddHeader(httpHeader);
 m_txTrace (p);
 m_socket->Send (p);
// }
 
// else:
//   p_0 = false;


 cout<<"need to receive packet: "<<udp_series [0][m_segmentId]<<endl; //Jerry
 cout<<"u,4,"<<m_sent<<","<<quality<<","<<udp_series [0][m_segmentId]<<","<<p_size<<endl; //Jerry segmentId, quality, packet number, packet_size

  /* for packet=0*/
 // if (udp_series [0][m_segmentId]==0)
///	Simulator::Schedule(Seconds(0.01), &DudpClient::CallPlayer, this);
  /**/
     m_segmentId++;

 

//  m_txTrace (p);
//  m_socket->Send (p);

  ++m_sent;

  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   Ipv4Address::ConvertFrom (m_peerAddress) << " port " << m_peerPort);
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   Ipv6Address::ConvertFrom (m_peerAddress) << " port " << m_peerPort);
    }
  else if (InetSocketAddress::IsMatchingType (m_peerAddress))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   InetSocketAddress::ConvertFrom (m_peerAddress).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (m_peerAddress).GetPort ());
    }
  else if (Inet6SocketAddress::IsMatchingType (m_peerAddress))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   Inet6SocketAddress::ConvertFrom (m_peerAddress).GetIpv6 () << " port " << Inet6SocketAddress::ConvertFrom (m_peerAddress).GetPort ());
    }
/*
  if (m_sent < m_count) 
    {
      ScheduleTransmit (m_interval);
    }
*/

}

void
DudpClient::HandleRead (Ptr<Socket> socket)
{
  
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  HTTPHeader header;
  while ((packet = socket->RecvFrom (from)))
    {
	packet->RemoveHeader(header);
      if (InetSocketAddress::IsMatchingType (from))
        {
//	cout <<"----client receive size:  " << packet->GetSize()<<endl; //Jerry
	
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " <<
                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
        }
    }
//  cout <<"----client receive size:  " << packet->GetSize()<<endl; //Jerry
 
//  cout <<"received u_packet for segment: "<< header.GetSegmentId()<<endl; //Jerry
  p_seg = header.GetSegmentId();
  Packet message(1346);

  PacketReceive (message);

}


void
DudpClient::PacketReceive (Packet message)
{

p_num++;
//cout <<"At time " << Simulator::Now ().GetSeconds ()<<" client receize packet"<<endl;

if (T_play==false && now_seg==p_seg){
	// segment, time
	cout<<"u,2,"<<p_seg<<","<<Simulator::Now ().GetSeconds ()<<endl; // packet for segment id, client receive time 
	udp_player.ReceiveFrame(&message);
    }
else
	udp_player.PlayFrame();

//cout << "still receive packet??"<<endl; 

if (p_num<udp_series[0][m_segmentId-1])
	udp_player.CheckPlay();

else
   CallPlayer();


udp_player.WakeupUdp(this);
}

void
DudpClient::CallPlayer(){
//if (m_segmentId>59)
//	cout<<"in udpclient -- T_play: "<<T_play<<endl;

if (T_play==false)
	Simulator::Schedule(Seconds(0.01), &DudpClient::CallPlayer, this);
else{
//	cout <<"here??"<<endl;
	udp_player.WakeupUdp(this);
	udp_player.PlayFrame();
}

}



}
