#include "mpeg-player.h"

using namespace std;
static std::string input_series;
extern int seg_length;
extern int init_buffer;
extern vector <int> video_num;
extern vector<vector<int>> video_series;
extern vector<vector<vector<float>>> probs;
extern vector<vector<double>> brs;
extern vector<vector<double>> dists;
extern vector<vector<vector<double>>> video_size;
extern vector<vector<double>> user_seg_size;
extern vector<vector<vector<float>>> pred_info;
extern vector<vector<vector<float>>> gt_info;
extern vector <string> video_names;
extern vector<vector<int>> user_info;
extern std::string tile_col, tile_row;
extern int tile_num;
extern float rho;
extern std::string experiment_dir;
extern std::string feature_dir;
extern std::string video_info_dir;
extern std::string prob_dir;
extern std::string prob_overall_dir;
extern std::string user_info_file;
extern std::string area_dir;
extern int M;
extern std::string mode;
extern std::string cur_dir;
extern std::string dr_dir;
extern int req_num;

extern vector<vector<vector<float>>> cur_info;
extern vector<vector<vector<float>>> dr_info;


//////---------------for udp-----------------
extern vector<vector<double>> udp_seg_size;
extern vector<vector<double>> udp_seg_size2;
extern vector<vector<double>> udp_seg_size3;
extern vector<vector<int>> udp_series;
extern bool T_play;  // check if mpeg-player is playing
extern bool T_start; // check if dash-cleint has downloaded init_buffer-1 segment

extern double TP; //throughput
extern double t_bw; //totalbandwidth 
extern double p_inter; // packet interval
extern int count_packet; // count pakcet at server
extern bool loss_mark; // mark loss at udp-player
extern int now_seg;

extern int mpeg_play; //record mepg play which segment
