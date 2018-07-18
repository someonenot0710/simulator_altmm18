/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 TEI of Western Macedonia, Greece
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
 *
 * Author: Dimitrios J. Vergados <djvergad@gmail.com>
 */

#ifndef UDP_PLAYER_H_
#define UDP_PLAYER_H_

#include <queue>
#include <map>
#include "ns3/ptr.h"
#include "ns3/packet.h"


namespace ns3
{
  enum
  {
    UDP_PLAYER_PAUSED, UDP_PLAYER_PLAYING, UDP_PLAYER_NOT_STARTED, UDP_PLAYER_DONE
  };

  class DudpClient;
  class DashClient;


  class UdpPlayer
  {
  public:
    UdpPlayer();

    virtual
    ~UdpPlayer();

    void
    ReceiveFrame(Ptr<Packet> message);

    int
    GetQueueSize();

    void
    Start();

    Time
    GetRealPlayTime(Time playTime);

    void inline
    SchduleBufferWakeup(const Time t, DashClient * client)
    {
      m_bufferDelay = t;
      m_dashClient = client;
    }

    void inline
    WakeupUdp(DudpClient *d_client)
    {
      m_udpClient = d_client;
    }
   

    void
    GetID(int id); //Jerry

    int m_state;
    Time m_interruption_time;
    int m_interrruptions;

    Time m_start_time;
    uint64_t m_totalRate;
    uint32_t m_minRate;
    uint32_t m_framesPlayed;

    void
    PlayFrame();

    void
    CheckPlay();
  private:
//    void
//    PlayFrame();

    Time m_lastpaused;
    std::queue<Ptr<Packet> > m_queue;
    Time m_bufferDelay;
    DashClient * m_dashClient;
    DudpClient * m_udpClient;
    int userID; //Jerry
    size_t v_num=0;
    int record=0;
  };
} // namespace ns3

#endif /* UDP_PLAYER_H_ */
