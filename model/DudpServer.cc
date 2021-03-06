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
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

#include "DudpServer.h"
#include "http-header.h"
#include "para.h"


//int count_packet;

uint32_t s_id;

using namespace std;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DudpServerApplication");

NS_OBJECT_ENSURE_REGISTERED (DudpServer);

TypeId
DudpServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DudpServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<DudpServer> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&DudpServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

DudpServer::DudpServer ()
{
  NS_LOG_FUNCTION (this);
}

DudpServer::~DudpServer()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  m_socket6 = 0;
}

void
DudpServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
DudpServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      if (m_socket->Bind (local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  if (m_socket6 == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket6 = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      if (m_socket6->Bind (local6) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      if (addressUtils::IsMulticast (local6))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket6);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, local6);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&DudpServer::HandleRead, this));
  m_socket6->SetRecvCallback (MakeCallback (&DudpServer::HandleRead, this));
}

void 
DudpServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  if (m_socket6 != 0) 
    {
      m_socket6->Close ();
      m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

void 
DudpServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
        }


      HTTPHeader header;
      packet->RemoveHeader(header);
//      cout << "user: " << header.GetVideoId() << "  segmentID: "<< header.GetSegmentId() <<endl; //Jerry
//      cout << "total packet_num: " <<header.GetPacketNum()<<endl; // Jerry
      packet->RemoveAllPacketTags ();
      packet->RemoveAllByteTags ();
      NS_LOG_LOGIC ("Echoing packet");

      uint32_t packet_num = header.GetPacketNum();
      count_packet = packet_num;
      s_id = header.GetSegmentId();
     now_seg = s_id;
//      now_seg = header.GetSegmentId(); // record new server send for which segment 

      /*
      for(uint32_t i=0; i<packet_num; i++){
	m_client = from;
	ScheduleTransmit(Seconds (0.));
	}
      */

     /*---*/
      m_client = from;
      T_play=false;  //Jerry
      loss_mark=false; //Jerry
      ScheduleTransmit(Seconds (0.));
     /*---*/

      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << packet->GetSize () << " bytes to " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }
      else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << packet->GetSize () << " bytes to " <<
                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
        }
    }
}


void
DudpServer::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);

/*-----*/
  count_packet--;
  //m_sendEvent = 
  if (count_packet>=0 && T_play!=true){
  	Simulator::Schedule (Seconds (0.), &DudpServer::Send, this);
	Simulator::Schedule (Seconds (p_inter), &DudpServer::ScheduleTransmit, this,Seconds (0.));
  }
  else
	count_packet=-1;
 /*-----*/
 
//  Simulator::Schedule (dt, &DudpServer::Send, this);
}


void
DudpServer::Send (void)
{

HTTPHeader httpHeader;
httpHeader.SetSegmentId(s_id);

Ptr<Packet> packet;
packet = Create<Packet> (1346); // cause add header
packet->AddHeader(httpHeader);
// segment , time
cout <<"u,"<<"1,"<<s_id<<","<<Simulator::Now ().GetSeconds ()<<endl; //Jerry server send time
//cout<<"At time " << Simulator::Now ().GetSeconds () << "s server sent " << packet->GetSize()<<endl;
m_socket->SendTo (packet, 0, m_client);

}

} // Namespace ns3
