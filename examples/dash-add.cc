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

// Network topology
//
//       n0 ----------- n1
//            500 Kbps
//             5 ms
//
#include <string>
#include <fstream>
#include <sstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/dash-module.h"
#include <sstream>
#include "../model/para.h"
using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE("DashExample");

#define SIZE 100
char line[SIZE];

//vector <int> video_num;

int
main(int argc, char *argv[])
{
  bool tracing = false;
  uint32_t maxBytes = 100;
  uint32_t users = 1;
  double target_dt = 35.0;
  double stopTime = 100.0;
  std::string linkRate = "500Kbps";
  std::string delay = "0ms";
  std::string protocol = "ns3::DashClient";
  std::string window = "10s";
  video_info_dir="";
  prob_dir="";
  prob_overall_dir="";
  user_info_file="";
  experiment_dir="";
  int tile_rows=0;
  int tile_cols=0;
  rho=0.5;
  init_buffer = 4;
  seg_length = 1;
  std::string gt_dir="";
  std::string pred_dir="";

  mode=""; //Jerry



//
// Allow the user to override any of the defaults at
// run-time, via command-line arguments
//
  CommandLine cmd;
  cmd.AddValue("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue("maxBytes", "Total number of bytes for application to send",
      maxBytes);
  cmd.AddValue("users", "The number of concurrent videos", users);
  cmd.AddValue("targetDt",
      "The target time difference between receiving and playing a frame.",
      target_dt);
  cmd.AddValue("stopTime",
      "The time when the clients will stop requesting segments", stopTime);
  cmd.AddValue("linkRate",
      "The bitrate of the link connecting the clients to the server (e.g. 500kbps)",
      linkRate);
  cmd.AddValue("delay",
      "The delay of the link connecting the clients to the server (e.g. 5ms)",
      delay);
  cmd.AddValue("protocol",
      "The adaptation protocol. It can be 'ns3::DashClient' or 'ns3::OsmpClient (for now).",
      protocol);
  cmd.AddValue("window",
      "The window for measuring the average throughput (Time).", window);

  cmd.AddValue("in_file",
      "input file", input_series); //Jerry

  cmd.AddValue("seg_length",
      "The segment length.", seg_length); //Jerry

  cmd.AddValue("init_buffer",
      "The initial buffer length (segment).", init_buffer); //Jerry
  cmd.AddValue("video_info_dir",
      "The directory storing the video information: tiled-segment size, PSNR, SSIM, OPVQ.", video_info_dir); //yibin
  cmd.AddValue("user_info",
      "The file storing the user information (video_idx, user_idx).", user_info_file); //yibin
  cmd.AddValue("experiment_dir",
      "The directory storing the experiment outputs.", experiment_dir); //yibin
  cmd.AddValue("feature_dir",
      "The directory storing input features.", feature_dir); //yibin
  cmd.AddValue("tile_rows",
      "The number of rows in tile.", tile_rows); //yibin
  cmd.AddValue("tile_cols",
      "The number of columns in tile.", tile_cols); //yibin
  cmd.AddValue("rho",
      "The threshold of viewing probability.", rho); //yibin
  cmd.AddValue("pred_dir",
      "The directory storing the predicted traces.", pred_dir); //yibin
  cmd.AddValue("gt_dir",
      "The directory storing the ground truth traces.", gt_dir); //yibin

  cmd.AddValue("mode",
      "differentiate using CUR or prediction", mode); //Jerry

  //cmd.AddValue("area_dir",
    //  "The directory storing the area information.", area_dir); //yibin
  cmd.Parse(argc, argv);

   //yibin 
   // video info index
  std::string video_info_index="";
  video_info_index=video_info_dir+"video_index";
  ifstream video_info_file(video_info_index);
  if(!video_info_file)
     cout<<"no video info file"<<endl;
  std::string tmp_str="";
  while (video_info_file >> tmp_str){
      video_names.push_back(tmp_str);
  }
  video_info_file.close();
  std::stringstream tmp_tile_col, tmp_tile_row;
  tmp_tile_col<<tile_cols;
  tmp_tile_row<<tile_rows;
  tile_col=tmp_tile_col.str();
  tile_row=tmp_tile_row.str();
  string str;
  //vector<double> tmp;
  // video coef
  for(uint32_t k=0; k<video_names.size(); k++){
     vector<vector<double>> tmp_video_size;
     ifstream video_size_file(video_info_dir+video_names[k]+"_"+tile_col+"x"+tile_row+"_size");
     if(!video_size_file)
         cout<<"no video info file: "<<video_names[k]+"_"+tile_col+"x"+tile_row+"_size"<<endl;
     while(getline(video_size_file,str)){
         istringstream sss(str);
         string token;
         vector<double> tmp_tile_size;
         while(getline(sss,token,' ')){
			tmp_tile_size.push_back(stod(token));
         }
         tmp_video_size.push_back(tmp_tile_size);
     } 
     video_size.push_back(tmp_video_size);
     video_size_file.close(); 
  }
  // user info
  ifstream user_info_fp(user_info_file);
  if(!user_info_fp)
      cout<<"no user info file: "<<user_info_file<<endl;
  string video_idx, user_idx;
  while(user_info_fp >> video_idx >> user_idx){
     vector<int> tmp_user;
     tmp_user.push_back(stoi(video_idx));
     tmp_user.push_back(stoi(user_idx));
     tmp_user.push_back(-1); //yibin: current received packet number of current segment
	 user_info.push_back(tmp_user);
	 //pred info
     char tmp_pred_file[50];
	 sprintf(tmp_pred_file,"%s%s_user%02d_pred",pred_dir.c_str(),video_names[tmp_user[0]].c_str(),stoi(user_idx));
	 std::string pred_info_f(tmp_pred_file);
     ifstream pred_info_fp(pred_info_f);
	 vector<vector<float>> tmp_user_pred;
	 int cnt=0;
	 while(getline(pred_info_fp,str)){
//		 if (cnt%30!=0){
  //           cnt++;
 //            continue;
//		 }
         istringstream sss(str);
         string token;
         vector<float> tmp_tile_pred;
         while(getline(sss,token,' ')){
            tmp_tile_pred.push_back(stof(token));
         }
         tmp_user_pred.push_back(tmp_tile_pred);
//		 cnt++;
     }
	 pred_info.push_back(tmp_user_pred);
	 pred_info_fp.close();
	 //gt info
     char tmp_gt_file[50];
     sprintf(tmp_gt_file,"%s%s_user%02d_gt_n30",gt_dir.c_str(),video_names[tmp_user[0]].c_str(),stoi(user_idx));
     std::string tmp_gt_f(tmp_gt_file);
	 ifstream gt_info_fp(tmp_gt_f);
     vector<vector<float>> tmp_user_gt;
     cnt=0;
     while(getline(gt_info_fp,str)){
         if (cnt%30!=0){
             cnt++;
             continue;
         }
         istringstream sss(str);
         string token;
         vector<float> tmp_tile_gt;
         while(getline(sss,token,' ')){
            tmp_tile_gt.push_back(stof(token));
         }
         tmp_user_gt.push_back(tmp_tile_gt);
         cnt++;
     }
     gt_info.push_back(tmp_user_gt);
     gt_info_fp.close();

  }
  user_info_fp.close();

  tile_num=(int)(tile_cols*tile_rows);
  uint32_t seg_num=(int)(60.0/(double)seg_length);
  for(uint32_t k=0; k<users; k++){
      vector<vector<float>> tmp_seg;
      for(uint32_t m=0; m<seg_num; m++){
          vector<float> tmp_tile;
          for(int v=0; v<tile_num; v++){
              tmp_tile.push_back(0);
          }
         tmp_seg.push_back(tmp_tile);
      }
      probs.push_back(tmp_seg);
  }
  
  for(uint32_t k=0; k<users; k++){
      vector<int> tmp_vseg;
      for(uint32_t m=0; m<seg_num; m++){
          tmp_vseg.push_back(0);
      }
      video_series.push_back(tmp_vseg);
  }
  for(uint32_t k=0; k<users; k++){
      vector<double> tmp_br_seg;
      vector<double> tmp_dist_seg;
      for(uint32_t m=0; m<seg_num; m++){
          tmp_br_seg.push_back(0);
      }
      brs.push_back(tmp_br_seg);
  }



//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO("Create nodes.");
  NodeContainer nodes;
//  nodes.Create(2);

  nodes.Create (5);


  NS_LOG_INFO("Create channels.");

  NodeContainer n0n2 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n3n2 = NodeContainer (nodes.Get (3), nodes.Get (2));
  NodeContainer n4n2 = NodeContainer (nodes.Get (2), nodes.Get (4));

  InternetStackHelper internet;
  internet.Install(nodes);


  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("18Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d2 = p2p.Install (n0n2);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("20Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d1d2 = p2p.Install (n1n2);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("40Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d3d2 = p2p.Install (n3n2);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("150Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d4d2 = p2p.Install (n4n2);

//
// Explicitly create the point-to-point link required by the topology (shown above).
//

  /*
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue(linkRate));
  pointToPoint.SetChannelAttribute("Delay", StringValue(delay));
  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);
  */
//
// Install the internet stack on the nodes
//

/*
  InternetStackHelper internet;
  internet.Install(nodes);
*/
//
// We've got the "hardware" in place.  Now we need to add IP addresses.
//
/*
  NS_LOG_INFO("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  Ipv4InterfaceContainer i = ipv4.Assign(devices);
*/

 // Later, we add IP addresses.
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (d0d2);
  
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);
   
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i2 = ipv4.Assign (d3d2);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i2 = ipv4.Assign (d4d2);

  NS_LOG_INFO("Create Applications.");

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();





  std::vector<std::string> protocols;
  std::stringstream ss(protocol);
  std::string proto;
  uint32_t protoNum = 0; // The number of protocols (algorithms)
  while (std::getline(ss, proto, ',') && protoNum++ < users)
    {
      protocols.push_back(proto);
    }

  uint16_t port = 80;  // well-known echo port number
  std::vector<DashClientHelper> clients;
  std::vector<ApplicationContainer> clientApps;


  for (uint32_t user = 0; user < users; user++)
    {

      if(user==0){
      DashClientHelper client("ns3::TcpSocketFactory",
          InetSocketAddress(i0i2.GetAddress(1), port), protocols[user % protoNum]);
      client.SetAttribute("VideoId", UintegerValue(user + 1));
      ApplicationContainer clientApp = client.Install(nodes.Get(0));
      clientApp.Start(Seconds(0.25));
      clients.push_back(client);
      clientApps.push_back(clientApp);	
      }

      else if(user==1){
      DashClientHelper client("ns3::TcpSocketFactory",
          InetSocketAddress(i1i2.GetAddress(1), port), protocols[user % protoNum]);
      client.SetAttribute("VideoId", UintegerValue(user + 1));
      ApplicationContainer clientApp = client.Install(nodes.Get(1));
      clientApp.Start(Seconds(0.25));
      clients.push_back(client);
      clientApps.push_back(clientApp);
      }

      else if(user==2){
      DashClientHelper client("ns3::TcpSocketFactory",
          InetSocketAddress(i3i2.GetAddress(1), port), protocols[user % protoNum]);
      client.SetAttribute("VideoId", UintegerValue(user + 1));
      ApplicationContainer clientApp = client.Install(nodes.Get(3));
      clientApp.Start(Seconds(0.25));
      clients.push_back(client);
      clientApps.push_back(clientApp);
      }
      //client.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
//      client.SetAttribute("VideoId", UintegerValue(user + 1)); // VideoId should positive
//      client.SetAttribute("TargetDt", TimeValue(Seconds(target_dt)));
//      client.SetAttribute("window", TimeValue(Time(window)));
//      ApplicationContainer clientApp = client.Install(nodes.Get(0));
//      clientApp.Start(Seconds(0.25));
//      clientApp.Stop(Seconds(stopTime));

//      clients.push_back(client);
//      clientApps.push_back(clientApp);

    }

  DashServerHelper server("ns3::TcpSocketFactory",
          InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer serverApps = server.Install(nodes.Get(2));
  serverApps.Start(Seconds(0.0));


/*
  DashServerHelper server("ns3::TcpSocketFactory",
      InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer serverApps = server.Install(nodes.Get(2));
  serverApps.Start(Seconds(0.0));
*/


//  serverApps.Stop(Seconds(stopTime + 5.0));

//
// Set up tracing if enabled
//
/*
  if (tracing)
    {
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll(ascii.CreateFileStream("dash-send.tr"));
      pointToPoint.EnablePcapAll("dash-send", false);
    }

*/
//
// Now, do the actual simulation.
//
  NS_LOG_INFO("Run Simulation.");
  /*Simulator::Stop(Seconds(100.0));*/
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");


  uint32_t k;
  for (k = 0; k < users; k++)
    {
      Ptr<DashClient> app = DynamicCast<DashClient>(clientApps[k].Get(0));
      std::cout << protocols[k % protoNum] << "-Node: " << k;
      app->GetStats();
    }

  return 0;

}
