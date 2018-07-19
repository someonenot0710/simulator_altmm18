clear;
clc;

four_dir = '10x10/12.7/';

wifi_dir = '10x10/37.1/';

fix_dir = '10x10/43.2/';

%tcp_dir = 'tcp/';
%mix_dir = 'mix/';
tcp_dir = 'tr/';
mix_dir = 'tr/';

user_name = 'user';
tcp='_tcp_tr';
udp='_udp_tr';


p_fix = zeros(10,61);
p_wifi = zeros(10,61);
p_four = zeros(10,61);
for i=1:10
    
    p_fix_file = strcat(tcp_dir,fix_dir,user_name,num2str(i-1),tcp);
    p_wifi_file = strcat(tcp_dir,wifi_dir,user_name,num2str(i-1),tcp);
    p_four_file = strcat(tcp_dir,four_dir,user_name,num2str(i-1),tcp);
    
    p_fix_data = csvread(p_fix_file);
    p_wifi_data = csvread(p_wifi_file);
    p_four_data = csvread(p_four_file);
     
    p_fix(i,:)=p_fix_data(1:61);
    p_wifi(i,:)=p_wifi_data(1:61);
    p_four(i,:)=p_four_data(1:61);
    
end



tcp_fix = zeros(10,61);
udp_fix = zeros(10,61);
for i=1:10
    
    tcp_file = strcat(mix_dir,fix_dir,user_name,num2str(i-1),tcp);
    udp_file = strcat(mix_dir,fix_dir,user_name,num2str(i-1),udp);
    
    tcp_data = csvread(tcp_file);
    udp_data = csvread(udp_file);
    
    tcp_fix(i,:)=tcp_data(1:61);
    udp_fix(i,2:61)=udp_data(1:60);
    
end

mix_fix = tcp_fix+udp_fix;



tcp_wifi = zeros(10,61);
udp_wifi = zeros(10,61);
for i=1:10
    
    tcp_file = strcat(mix_dir,wifi_dir,user_name,num2str(i-1),tcp);
    udp_file = strcat(mix_dir,wifi_dir,user_name,num2str(i-1),udp);
    
    tcp_data = csvread(tcp_file);
    udp_data = csvread(udp_file);
    
    tcp_wifi(i,:)=tcp_data(1:61);
    udp_wifi(i,2:61)=udp_data(1:60);
    
end

mix_wifi = tcp_wifi+udp_wifi;

tcp_four = zeros(10,61);
udp_four = zeros(10,61);
for i=1:10
    
    tcp_file = strcat(mix_dir,four_dir,user_name,num2str(i-1),tcp);
    udp_file = strcat(mix_dir,four_dir,user_name,num2str(i-1),udp);
    
    tcp_data = csvread(tcp_file);
    udp_data = csvread(udp_file);
    
    tcp_four(i,:)=tcp_data(1:61);
    udp_four(i,2:61)=udp_data(1:60);
    
end

mix_four = tcp_four+udp_four;


%mix
band_four=127000;
mix_total_four=0;
mix_st_four = zeros(1,10);

band_wifi=371000;
mix_total_wifi=0;
mix_st_wifi = zeros(1,10);

band_fix=432000;
mix_total_fix=0;
mix_st_fix = zeros(1,10);

%pure_tcp
p_total_four=0;
p_st_four = zeros(1,10);

p_total_wifi=0;
p_st_wifi = zeros(1,10);

p_total_fix=0;
p_st_fix = zeros(1,10);


all_fix = zeros(10,2);
all_wifi = zeros(10,2);
all_four = zeros(10,2);

for i=1:10
    %mix
    mix_total_four = mix_total_four+sum(mix_four(i,:)/band_four)/61;
    mix_st_four(1,i)=sum(mix_four(i,:)/band_four)/61; 
    
    mix_total_wifi = mix_total_wifi+sum(mix_wifi(i,:)/band_wifi)/61;
    mix_st_wifi(1,i)=sum(mix_wifi(i,:)/band_wifi)/61;
    
    mix_total_fix = mix_total_fix+sum(mix_fix(i,:)/band_fix)/61;
    mix_st_fix(1,i)=sum(mix_fix(i,:)/band_fix)/61;
    
    %pure tcp
    p_total_four = p_total_four+sum(p_four(i,:)/band_four)/61;
    p_st_four(1,i)=sum(p_four(i,:)/band_four)/61; 
    
    p_total_wifi = p_total_wifi+sum(p_wifi(i,:)/band_wifi)/61;
    p_st_wifi(1,i)=sum(p_wifi(i,:)/band_wifi)/61;
    
    p_total_fix = p_total_fix+sum(p_fix(i,:)/band_fix)/61;
    p_st_fix(1,i)=sum(p_fix(i,:)/band_fix)/61;
    
    all_fix(i,:)=[sum(mix_fix(i,:)/band_fix)/61 sum(p_fix(i,:)/band_fix)/61]
    all_wifi(i,:)=[sum(mix_wifi(i,:)/band_wifi)/61 sum(p_wifi(i,:)/band_wifi)/61]
    all_four(i,:)=[sum(mix_four(i,:)/band_four)/61 sum(p_four(i,:)/band_four)/61]
    
end

mix_total_four=mix_total_four/10;
mix_std_four = 1.96*(std(mix_st_four)/sqrt(10));

mix_total_wifi=mix_total_wifi/10;
mix_std_wifi = 1.96*(std(mix_st_wifi)/sqrt(10));

mix_total_fix=mix_total_fix/10;
mix_std_fix = 1.96*(std(mix_st_fix)/sqrt(10));


p_total_four=p_total_four/10;
p_std_four = 1.96*(std(p_st_four)/sqrt(10));

p_total_wifi=p_total_wifi/10;
p_std_wifi = 1.96*(std(p_st_wifi)/sqrt(10));

p_total_fix=p_total_fix/10;
p_std_fix = 1.96*(std(p_st_fix)/sqrt(10));


y1=[mix_total_fix, mix_total_wifi, mix_total_four];
x1=['12','10','8'];
error1 = [mix_std_fix,mix_std_wifi,mix_std_four];
errorbar(x1,y1,error1, 'linestyle', 'none', 'LineWidth', 2);


ci = [  ;  ]

loss = [mix_total_fix p_total_fix ; mix_total_wifi  p_total_wifi; mix_total_four p_total_four]
ci = [mix_std_fix p_std_fix; mix_std_wifi p_std_wifi; mix_std_four p_std_four ]
% 
figure
b=bar(loss)
hold on
set(b,'BarWidth',1); 
numgroups = size(loss, 1);
numbars = size(loss, 2);
groupwidth = min(0.8, numbars/(numbars+1.5))
 for i = 1:numbars
      % Based on barweb.m by Bolu Ajiboye from MATLAB File Exchange
      x = (1:numgroups) - groupwidth/2 + (2*i-1) * groupwidth / (2*numbars);  % Aligning error bar with individual bar
      errorbar(x, loss(:,i), ci(:,i), 'linestyle', 'none', 'LineWidth', 2);
end

eachname={'BroadBand','WiFi','4G'};
xlabel('Network Types');
ylabel('Bandwidth Utilization (%)');
set(gca,'xticklabel',eachname)
lgd=legend('Hybrid','DASH');



% sequence_fix = [all_fix(5,:); all_fix(7,:); all_fix(9,:); all_fix(1,:); all_fix(2,:); all_fix(4,:); all_fix(3,:); all_fix(8,:);all_fix(6,:);all_fix(10,:)]
% sequence_wifi = [all_wifi(5,:); all_wifi(7,:); all_wifi(9,:); all_wifi(1,:); all_wifi(2,:); all_wifi(4,:); all_wifi(3,:); all_wifi(8,:);all_wifi(6,:);all_wifi(10,:)]
% sequence_four = [all_four(5,:); all_four(7,:); all_four(9,:); all_four(1,:); all_four(2,:); all_four(4,:); all_four(3,:); all_four(8,:);all_four(6,:);all_four(10,:)]
% 
% 
% figure
% b=bar(sequence_four)
% hold on
% set(b,'BarWidth',1); 
% xlabel('Video Sequence');
% ylabel('Bandwidth Utilization (%)');
% lgd=legend('Hybrid','DASH');
 % for 3-column figures
set(gca,'FontSize',20)
set(gca, 'FontName', 'Times New Roman');
set(gca,'TickDir','out')
set(get(gca, 'xlabel'), 'interpreter', 'latex');
set(get(gca, 'xlabel'), 'FontName', 'Times New Roman');
set(get(gca, 'xlabel'), 'FontSize', 20);
set(get(gca, 'ylabel'), 'interpreter', 'latex');
set(get(gca, 'ylabel'), 'FontName', 'Times New Roman');
set(get(gca, 'ylabel'), 'FontSize', 20);
set(legend(), 'interpreter', 'latex');
set(legend(), 'FontName', 'Times New Roman');
set(legend(), 'FontSize', 20);
set(gcf, 'WindowStyle', 'normal');
set(gca, 'Unit', 'inches');
set(gca, 'Position', [.65 .65 4.6 3.125]);
set(gcf, 'Unit', 'inches');
set(gcf, 'Position', [0.25 2.5 5.5 4.05]);

 % for 3-column figures
set(gca,'FontSize',20)
set(gca, 'FontName', 'Times New Roman');
set(gca,'TickDir','out')
set(get(gca, 'xlabel'), 'interpreter', 'latex');
set(get(gca, 'xlabel'), 'FontName', 'Times New Roman');
set(get(gca, 'xlabel'), 'FontSize', 20);
set(get(gca, 'ylabel'), 'interpreter', 'latex');
set(get(gca, 'ylabel'), 'FontName', 'Times New Roman');
set(get(gca, 'ylabel'), 'FontSize', 20);
set(legend(), 'interpreter', 'latex');
set(legend(), 'FontName', 'Times New Roman');
set(legend(), 'FontSize', 20);
set(gcf, 'WindowStyle', 'normal');
set(gca, 'Unit', 'inches');
set(gca, 'Position', [.65 .65 4.6 3.125]);
set(gcf, 'Unit', 'inches');
set(gcf, 'Position', [0.25 2.5 5.5 4.05]);


%loss = [vod_total_fix live_total_fix uni_total_fix mm_total_fix; vod_total_wifi live_total_wifi uni_total_wifi 0; vod_total_four live_total_four uni_total_four 0];

%ci = [vod_std_fix live_std_fix uni_std_fix mm_std_fix; vod_std_wifi live_std_wifi uni_std_wifi 0; vod_std_four live_std_four uni_std_four 0];



