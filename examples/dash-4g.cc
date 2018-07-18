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


void BandwidthTrace(){

Config::Set("/NodeList/0/DeviceList/0/$ns3::PointToPointNetDevice/DataRate", StringValue("5Mbps") );
//Config::Set("/NodeList/1/DeviceList/1/$ns3::PointToPointNetDevice/DataRate", StringValue("10Mbps") );
//Config::Set("/NodeList/2/DeviceList/2/$ns3::PointToPointNetDevice/DataRate", StringValue("3Mbps") );
//Config::Set("/NodeList/3/DeviceList/3/$ns3::PointToPointNetDevice/DataRate", StringValue("7Mbps") );
//Config::Set("/NodeList/4/DeviceList/4/$ns3::PointToPointNetDevice/DataRate", StringValue("3Mbps") );
//Config::Set("/NodeList/5/DeviceList/5/$ns3::PointToPointNetDevice/DataRate", StringValue("50Mbps") );

}

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
  req_num = 2;
  std::string gt_dir="";
  std::string pred_dir="";
  std::string size_dir="";

  mode=""; //Jerry
  std::string algo=""; //yibin
  vector<vector<float>> bw; //yibin
  int bw_area=2;
  std::string bw_type;
  uint32_t bw_idx=0;
  vector<float> user_bw;

  std::string cur_dir="";
  std::string dr_dir="";
  

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
  cmd.AddValue("cur_dir",
      "The directory storing the predicted traces.", cur_dir); //Jerry
  cmd.AddValue("dr_dir",
      "The directory storing the predicted traces.", dr_dir); //Jerry
  cmd.AddValue("mode",
      "differentiate using CUR or prediction", mode); //Jerry
  cmd.AddValue("algo",
      "The bitrate allocation algorithms: uni, vod, live, mm", algo); //yibin
  cmd.AddValue("bw_area",
      "The area considered for the bandwidth", bw_area); //yibin
  cmd.AddValue("bw_type",
      "The file indicating the bw types allocation for each user", bw_type); //yibin
  cmd.AddValue("bw_idx",
      "The index of bw type file", bw_idx); //yibin 
  cmd.AddValue("area_dir",
      "The directory storing the area information.", area_dir); //yibin
  cmd.AddValue("size_dir",
      "The directory storing the video size information.", size_dir); //yibin
  cmd.AddValue("req_num",
      "The directory storing the video size information.", req_num);
  cmd.Parse(argc, argv);
   //yibin 
  //network bw info kbps
  for(int i=0; i<6; i++){
    vector<float> tmp_bw;
    for(int j=0; j<3; j++){
      tmp_bw.push_back(0);
    }
	bw.push_back(tmp_bw);
  }
  bw[0][0]=46.2; // Asia Pacific
  bw[0][1]=26.7;
  bw[0][2]=12.6;
  bw[1][0]=11.7; // Latin America
  bw[1][1]=9.0;
  bw[1][2]=9.1;
  bw[2][0]=43.2; // North America
  bw[2][1]=37.1;
  bw[2][2]=12.7;
  bw[3][0]=37.9; // Western Europe
  bw[3][1]=25.0;
  bw[3][2]=15.9;
  bw[4][0]=32.8; // Central and Eastern Europe
  bw[4][1]=19.5;
  bw[4][2]=14.2;
  bw[5][0]=39.0; // Middle East and Africa
  bw[5][1]=6.2;
  bw[5][2]=4.6;
  //user bw
  std::string tmp_user_bw;
  ifstream user_bw_file(bw_type);
  if(!user_bw_file)
    cout<<"no user_bw_file"<<endl;
  uint32_t cnt=0;
  while(user_bw_file>>tmp_user_bw){
      cnt=cnt+1;
      if(cnt>bw_idx*users && cnt<=(bw_idx+1)*users){
	    user_bw.push_back(bw[bw_area][stoi(tmp_user_bw)]);
      }
  }
  user_bw_file.close();

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
  // user info
  ifstream user_info_fp(user_info_file);
  if(!user_info_fp)
      cout<<"no user info file: "<<user_info_file<<endl;
  string video_idx, user_idx;
  cnt=0;
  cout<<"tile_row: "<<tile_rows<<"  tile_col: "<<tile_cols<<endl;
  while(user_info_fp >> video_idx >> user_idx){
     vector<int> tmp_user;
     tmp_user.push_back(stoi(video_idx));
     tmp_user.push_back(stoi(user_idx));
     tmp_user.push_back(-1); //yibin: current received packet number of current segment
	 // segment size info based on network speed and user
     char tmp_size_file[100];
     sprintf(tmp_size_file,"%s%s_user%02d_%dx%d_%.1f_gt_%s",size_dir.c_str(),video_names[tmp_user[0]].c_str(),stoi(user_idx),tile_cols,tile_rows,user_bw[cnt],algo.c_str());
     std::string size_file(tmp_size_file);
     ifstream size_fp(size_file);
     if(!size_fp)
       cout<<"no size file"<<size_file<<endl;
     vector<double> tmp_video_size;
     while(getline(size_fp,str)){
         istringstream sss(str);
         string token;
         double tile_size=0;
//         vector<double> tmp_tile_size;
         while(getline(sss,token,' ')){
           tile_size=tile_size+stod(token);
           // tmp_tile_size.push_back(stod(token));
         }
         tmp_video_size.push_back(tile_size);
     }
     user_seg_size.push_back(tmp_video_size);
     size_fp.close();
     cnt=cnt+1;
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

  nodes.Create (users+1);


  NS_LOG_INFO("Create channels.");

  vector<NodeContainer> user_node;
  for (uint32_t user = 0; user < users; user++){
	NodeContainer x = NodeContainer (nodes.Get (user), nodes.Get (users));
	user_node.push_back(x);
  }
  
  /*
  NodeContainer n0n2 = NodeContainer (nodes.Get (0), nodes.Get (2));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n3n2 = NodeContainer (nodes.Get (3), nodes.Get (2));
  */
//  NodeContainer n4n2 = NodeContainer (nodes.Get (2), nodes.Get (4));

  InternetStackHelper internet;
  internet.Install(nodes);

  vector<NetDeviceContainer> user_device;
  string rate[15]= {"18.2Mbps","18.2Mbps","18.2Mbps","18.2Mbps","20Mbps","20Mbps","20Mbps","20Mbps","20Mbps","40Mbps","40Mbps","40Mbps","40Mbps","40Mbps","40Mbps"};
  vector<string> user_bwr;
  
  for (uint32_t i=0; i<user_bw.size(); i++){
//    for (uint32_t i=0; i<users; i++){	
	ostringstream ss;
	ss << user_bw[i];
	string tmp = ss.str();
	tmp = tmp+"Mbps";
//	cout<<tmp<<endl;
	user_bwr.push_back(tmp);
	cout<<"Band: "<<tmp<<endl;
  }


  for (uint32_t user = 0; user < users; user++){ 
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (user_bwr[user]));
	p2p.SetChannelAttribute ("Delay", StringValue ("40ms"));
	NetDeviceContainer x = p2p.Install (user_node[user]);
	user_device.push_back(x);
  }

  /*
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("18Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d2 = p2p.Install (user_node[0]);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("20Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d1d2 = p2p.Install (user_node[1]);

  p2p.SetDeviceAttribute ("DataRate", StringValue ("40Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d3d2 = p2p.Install (user_node[2]);
  */

/*
  p2p.SetDeviceAttribute ("DataRate", StringValue ("150Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d4d2 = p2p.Install (n4n2);
*/
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
  vector<Ipv4InterfaceContainer> user_ipv4;
  for (uint32_t user = 0; user < users; user++){
	string tmp1 = "10.1.";
	stringstream ss;
	ss << user;
	string str = ss.str();
	string ips = tmp1+str+".0";
	ipv4.SetBase (ips.c_str(), "255.255.255.0");
	Ipv4InterfaceContainer x = ipv4.Assign (user_device[user]);
	user_ipv4.push_back(x);

  }

/*
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2 = ipv4.Assign (user_device[0]);
  
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (user_device[1]);
   
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i2 = ipv4.Assign (user_device[2]);
*/

/*
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i2 = ipv4.Assign (d4d2);
*/
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
      DashClientHelper client("ns3::TcpSocketFactory",
	  InetSocketAddress(user_ipv4[user].GetAddress(1), port), protocols[user % protoNum]);
      client.SetAttribute("VideoId", UintegerValue(user + 1));
      ApplicationContainer clientApp = client.Install(nodes.Get(user));
      clientApp.Start(Seconds(0.25));
      clients.push_back(client);
      clientApps.push_back(clientApp);
	
 	/*
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
      ApplicationContainer clientApp = client.Install(nodes.Get(2));
      clientApp.Start(Seconds(0.25));
      clients.push_back(client);
      clientApps.push_back(clientApp);
      }
	*/
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
/*
  DashServerHelper server("ns3::TcpSocketFactory",
          InetSocketAddress(i4i2.GetAddress(1), port));
  ApplicationContainer serverApps = server.Install(nodes.Get(2));
  serverApps.Start(Seconds(0.0));
*/

  DashServerHelper server("ns3::TcpSocketFactory",
      InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer serverApps = server.Install(nodes.Get(users));
  serverApps.Start(Seconds(0.0));



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
  //new add
//  Simulator::Schedule (Seconds(20) , &BandwidthTrace);
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
