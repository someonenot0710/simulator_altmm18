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

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "http-header.h"
#include "mpeg-header.h"
#include "mpeg-player.h"
#include "dash-client.h"
#include "DudpClient.h"
#include <cmath>
#include "para.h"
NS_LOG_COMPONENT_DEFINE("MpegPlayer");


namespace ns3
{

  MpegPlayer::MpegPlayer() :
      m_state(MPEG_PLAYER_NOT_STARTED), m_interrruptions(0), m_totalRate(0), m_minRate(
          100000000), m_framesPlayed(0), m_bufferDelay("0s")
  {
    NS_LOG_FUNCTION(this);
  }

  MpegPlayer::~MpegPlayer()
  {
    NS_LOG_FUNCTION(this);
  }

  int
  MpegPlayer::GetQueueSize()
  {
    return m_queue.size();
  }

  Time
  MpegPlayer::GetRealPlayTime(Time playTime)
  {
    NS_LOG_INFO(
        " Start: " << m_start_time.GetSeconds() << " Inter: " << m_interruption_time.GetSeconds() << " playtime: " << playTime.GetSeconds() << " now: " << Simulator::Now().GetSeconds() << " actual: " << (m_start_time + m_interruption_time + playTime).GetSeconds());

    return m_start_time + m_interruption_time
        + (m_state == MPEG_PLAYER_PAUSED ?
            (Simulator::Now() - m_lastpaused) : Seconds(0)) + playTime
        - Simulator::Now();
  }

  void
  MpegPlayer::ReceiveFrame(Ptr<Packet> message)
  {

    NS_LOG_FUNCTION(this << message);
    NS_LOG_INFO("Received Frame " << m_state);
   
    Ptr<Packet> msg = message->Copy();

    m_queue.push(msg);

    if (m_state == MPEG_PLAYER_PAUSED)
      {
//	cout<<"user: "<<userID<<"  queue: "<<m_queue.size()<<" series: "<<video_series[userID][v_num]<<endl;
	if(m_queue.size()>=(unsigned) video_series[userID][v_num]){ //Jerry add if
        NS_LOG_INFO("Play resumed");
        m_state = MPEG_PLAYER_PLAYING;
        m_interruption_time += (Simulator::Now() - m_lastpaused);
	
	//tmm
//	cout<<"segment: "<<v_num<<endl;
	std::cout<<"3,"<<userID<<","<<m_lastpaused.GetSeconds()<<","<<Simulator::Now().GetSeconds()<<std::endl;
	record=1;

	std::cout<<"4,"<<userID<<","<<m_interrruptions<<","<<(Simulator::Now() - m_lastpaused).GetSeconds()<<std::endl; //tmm

        PlayFrame();
	}
      }
    else if (m_state == MPEG_PLAYER_NOT_STARTED)
      {
	//tmm
 	/*
	int initial_buffer=0;
	for(int i=0; i<init_buffer; i++){
		initial_buffer+=video_series[userID][i];
	}
	*/	
//	if(m_queue.size()>(unsigned) initial_buffer){
	if (video_series[userID][req_num]>0){
		T_start = true;
//		now_seg++;
	} 


	if(video_series[userID][req_num+1]>0){ //init_buffer
	if(m_queue.size()>(unsigned) (video_series[userID][v_num]+video_series[userID][v_num+1])){
        NS_LOG_INFO("Play started");
        m_state = MPEG_PLAYER_PLAYING;
        m_start_time = Simulator::Now();
//        Simulator::Schedule(Simulator::Now(), &MpegPlayer::PlayFrame, this);
//	std::cout<<"3,"<<userID<<","<<Simulator::Now().GetSeconds()<<std::endl; //tmm
	PlayFrame();
      	}
	}
      }
  }

  void //Jerry
  MpegPlayer::GetID(int m_id)
  {
    userID=m_id;
  }

  void
  MpegPlayer::Start(void)
  {
    NS_LOG_FUNCTION(this);
    m_state = MPEG_PLAYER_PLAYING;
    m_interruption_time = Seconds(0);

  }

  void
  MpegPlayer::PlayFrame(void)
  {
  
  mpeg_play ++ ;

/*
  cout<<"in play frame  "<< m_udpClient <<endl;
   
  if(m_udpClient){
	cout<<"connect sucess"<<endl;
        m_udpClient->CallPlayer(); //Jerry
    }
*/

//   cout <<"in mpeg player play frame" <<endl;
//   cout <<"mpeg-player play segment: " << v_num <<endl;   
//   cout << "now time: "<< Simulator::Now().GetSeconds() << endl; 
//   udp_player.PlayFrame();       
//    T_play=true;

    NS_LOG_FUNCTION(this);
    if (m_state == MPEG_PLAYER_DONE)
      {
        return;
      }
    if (m_queue.empty())
      {
        NS_LOG_INFO(Simulator::Now().GetSeconds() << " No frames to play");
        m_state = MPEG_PLAYER_PAUSED;
        m_lastpaused = Simulator::Now();
        m_interrruptions++;
        return;
      }
    if (m_queue.size()<(unsigned) video_series[userID][v_num])
      {
	m_state = MPEG_PLAYER_PAUSED;
        m_lastpaused = Simulator::Now();
        m_interrruptions++;
        return;	
      }

	T_play=true;
//	now_seg++;
	count_packet =0; //DudpServer Jerry
//	cout<<"play: "<< v_num << " T_play: "<< T_play<<endl;
//	cout <<"mpeg-player play segment: " << v_num <<endl;

	//tmm
	if (record==0){
		cout<<"segment: "<<v_num<<endl;
		std::cout<<"3,"<<userID<<","<<Simulator::Now().GetSeconds()<<","<<Simulator::Now().GetSeconds()<<std::endl; //tmm
	}
	record=0;

   
	MPEGHeader mpeg_header;
        HTTPHeader http_header; 


    if(v_num==0){
   //     cout<<"first v_num: "<<v_num<<endl;
	for (int i=0;i<(int)(video_series[userID][v_num]+video_series[userID][v_num+1]);i++){
        Ptr<Packet> message = m_queue.front();
        m_queue.pop();
        message->RemoveHeader(mpeg_header);
        message->RemoveHeader(http_header);
        m_totalRate += http_header.GetResolution();
        m_framesPlayed++;
      }
	v_num = v_num+2;
    }

    else{
    for (int i=0;i<(int)video_series[userID][v_num];i++){
	Ptr<Packet> message = m_queue.front();
	m_queue.pop();
	message->RemoveHeader(mpeg_header);
        message->RemoveHeader(http_header);
        m_totalRate += http_header.GetResolution();
	m_framesPlayed++; 
      }

      v_num++;
    }

    if(v_num==61 && m_dashClient){
	cout<<"in mepg--- T_play: "<<T_play<<endl; //Jerry
        m_dashClient->StopApplication();
	}
    /*
    Ptr<Packet> message = m_queue.front();
    m_queue.pop();

    MPEGHeader mpeg_header;
    HTTPHeader http_header;
    
    message->RemoveHeader(mpeg_header);
    message->RemoveHeader(http_header);

    m_totalRate += http_header.GetResolution();
*/  

    if (http_header.GetSegmentId() > 0) // Discard the first segment for the minRate
      {                                 // calculation, as it is always the minimum rate
        m_minRate =
            http_header.GetResolution() < m_minRate ?
                http_header.GetResolution() : m_minRate;
      }
   // m_framesPlayed++;


    Time b_t = GetRealPlayTime(mpeg_header.GetPlaybackTime());
/*
    if (m_bufferDelay > Time("0s") && b_t < m_bufferDelay && m_dashClient)
      {
//        m_dashClient->RequestSegment();
        m_bufferDelay = Seconds(0);
        m_dashClient = NULL;
      }
*/

   if((int)v_num<=56){
   if(m_dashClient){
    if(init_buffer>req_num){
        if((int) v_num>=init_buffer-req_num+1){
                m_dashClient->RequestSegment(); //Jerry
        }
    }

    else{
//	cout << "v_num= "<<v_num<<endl;
        if ((int)v_num>=init_buffer){
		if((int)v_num<=61-req_num){
	//	cout<< "player"<<endl; 
                m_dashClient->RequestSegment();
		}
        }
    }
   }
  }
    NS_LOG_INFO(
        Simulator::Now().GetSeconds() << " PLAYING FRAME: " << " VidId: " << http_header.GetVideoId() << " SegId: " << http_header.GetSegmentId() << " Res: " << http_header.GetResolution() << " FrameId: " << mpeg_header.GetFrameId() << " PlayTime: " << mpeg_header.GetPlaybackTime().GetSeconds() << " Type: " << (char) mpeg_header.GetType() << " interTime: " << m_interruption_time.GetSeconds() << " queueLength: " << m_queue.size());

    /*   std::cout << " frId: " << mpeg_header.GetFrameId()
     << " playtime: " << mpeg_header.GetPlaybackTime()
     << " target: " << (m_start_time + m_interruption_time + mpeg_header.GetPlaybackTime()).GetSeconds()
     << " now: " << Simulator::Now().GetSeconds()
     << std::endl;
     */
    Simulator::Schedule(MilliSeconds(1000), &MpegPlayer::PlayFrame, this);

  }

} // namespace ns3
