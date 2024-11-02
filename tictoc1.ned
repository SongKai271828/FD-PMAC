//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//


import ned.DelayChannel;

//
// TODO documentation
//
simple CCO
{
    parameters:
//        @signal[arrival](type="long");
        // @statistic[hopCount](title="hop count"; source="arrival"; record=vector,stats; interpolationmode=none);
        @display("i=misc/node_vs,gold"); // add a default icon
        int max_stas;
        int upload_wait_s = default(240);
        int freq_num = default(8);
    gates:
        inout bs_port[];

        inout frm_f0[];
        inout frm_f1[];
        inout frm_f2[];
        inout frm_f3[];
        inout frm_f4[];
        inout frm_f5[];
        inout frm_f6[];
        inout frm_f7[];
}

simple STA
{
    parameters:
        @display("i=misc/node_vs,blue"); // add a default icon
        bool as_a_cco = default(false);
        int base_no = default(0);
        int freq_num = default(8);
    gates:
        inout frm_f0[];
        inout frm_f1[];
        inout frm_f2[];
        inout frm_f3[];
        inout frm_f4[];
        inout frm_f5[];
        inout frm_f6[];
        inout frm_f7[];
}

simple BS
{
    parameters:
        @display("i=block/wrxtx"); // add a default icon
        int freq_num = default(8);
    gates:
        inout bs_port[];
}

//
// TODO documentation
//
network FD_PMAC
{
    parameters:
//        bool Rpmac = default(false);
        @signal[arrival](type="long");
        @statistic[hopCount](title="Depth"; source="arrival"; record=vector,stats; interpolationmode=none);
        @signal[collision](type="long");
        @statistic[Collision](title="Collision"; source="collision"; record=count,stats; interpolationmode=none);
        @signal[checked](type="long");
        @statistic[Checked](title="Collision Checked"; source="checked"; record=count,stats; interpolationmode=none);
        @signal[upload](type="long");
        @statistic[Upload](title="Upload Wait"; source="upload"; record=vector,stats; interpolationmode=none);
        @signal[elapsedTime](type="double");
        @statistic[ElapsedTime](title="Elapsed Time"; source="elapsedTime"; record=vector,stats; interpolationmode=none);
        @signal[uploadDepth](type="long");
        @statistic[UploadDepth](title="Upload Depth"; source="uploadDepth"; record=vector,stats; interpolationmode=none);
        @signal[uploadFinish](type="long");
        @statistic[UploadFinish](title="Upload Finish"; source="uploadFinish"; record=vector,stats; interpolationmode=none);
        @signal[uploadTime](type="double");
        @statistic[UploadTime](title="Upload Time"; source="uploadTime"; record=vector,stats; interpolationmode=none);
        int num_acco = default(8);
        int num_cco = num_acco;
        int num_sta = default(240);
        int freq_num = default(8);
        
        int depth = default(6);
        int num_per_layer = int(num_sta/depth);
        int num_delta = int(num_per_layer/8);
        
        int num_sta1 = num_per_layer+0*num_delta;
        int num_sta2 = 
        	(depth == 6) ? (num_per_layer+4*num_delta) : 
      		(depth == 5) ? (num_per_layer+3*num_delta) : 
        	(depth == 4) ? (num_per_layer+2*num_delta) : 
        	(depth == 3) ? (num_per_layer+3*num_delta) : 1;
        int num_sta3 = 
        	(depth == 6) ? (num_per_layer+5*num_delta) : 
      		(depth == 5) ? (num_per_layer+3*num_delta) : 
        	(depth == 4) ? (num_per_layer+1*num_delta) : 
        	(depth == 3) ? (num_per_layer-3*num_delta) : 1;
        int num_sta4 = 
        	(depth == 6) ? (num_per_layer-2*num_delta) : 
      		(depth == 5) ? (num_per_layer-2*num_delta) : 
        	(depth == 4) ? (num_per_layer-3*num_delta) : 1;
        int num_sta5 = 
        	(depth == 6) ? (num_per_layer-3*num_delta) : 
      		(depth == 5) ? (num_per_layer-4*num_delta) : 1;
        int num_sta6 = 
        	(depth == 6) ? (num_per_layer-4*num_delta) : 1;
        
        int base_sta1 = 1;
        int base_sta2 = base_sta1 + num_sta1;
        int base_sta3 = base_sta2 + num_sta2;
        int base_sta4 = base_sta3 + num_sta3;
        int base_sta5 = base_sta4 + num_sta4;
        int base_sta6 = base_sta5 + num_sta5;
        int base_acco = 2048;

        double interval_r = default(num_per_layer*150/2/3.1415926);
        double r_cco = default(1 * interval_r);
        double r_sta1 = default(2 * interval_r);
        double r_sta2 = default(3 * interval_r);
        double r_sta3 = default(4 * interval_r);
        double r_sta4 = default(5 * interval_r);
        double r_sta5 = default(6 * interval_r);
        double r_sta6 = default(7 * interval_r);
        double origin0 = default(8 * interval_r);
        double origin = default(7 * interval_r);
        double origin1 = default(6 * interval_r);
        double origin2 = default(5 * interval_r);
        double origin3 = default(4 * interval_r);
        double origin4 = default(3 * interval_r);
        double origin5 = default(2 * interval_r);
        double origin6 = default(1 * interval_r);
        double width = default(2 * (r_sta6 + interval_r));
        @display("bgb=$width,$width");
        
         double connectness = default(0.6);
         double connectness_cco = default(0.4);
         int max_neighbor = default(3);
//        double connectness = default(1.0);
//        double connectness_cco = default(1.0);
//        int max_neighbor = default(2);
    types:
        channel chn_frm extends ned.DatarateChannel
        {
            delay = 0ms;
        }
        channel chn_prm extends ned.DelayChannel
        {
            delay = 20ms;
        }
    submodules:
        BS: BS{
            @display("p=$origin0,$origin0");
        }
        ccos[1]: CCO{
            @display("p=$origin,$origin,ri,$r_cco,$r_cco");
            max_stas = parent.num_sta + parent.num_acco - 1;
//            @display("p=0,0,ri,$r_cco,$r_cco");
        }
        accos[num_acco]: STA{
            base_no = parent.base_acco;
            @display("p=$origin,$origin,ri,$r_cco,$r_cco");
//            @display("p=0,0,ri,$r_cco,$r_cco");
        }
        stas1[num_sta1]: STA {
            @display("p=$origin1,$origin1,ri,$r_sta1,$r_sta1");
            base_no = parent.base_sta1;
//            @display("p=0,0,ri,$r_sta1,$r_sta1");
        }
        stas2[num_sta2]: STA {
            base_no = parent.base_sta2;
            @display("p=$origin2,$origin2,ri,$r_sta2,$r_sta2");
//            @display("p=0,0,ri,$r_sta2,$r_sta2");
        }
        stas3[num_sta3]: STA {
            base_no = parent.base_sta3;
            @display("p=$origin3,$origin3,ri,$r_sta3,$r_sta3");
//            @display("p=0,0,ri,$r_sta3,$r_sta3");
        }
        stas4[num_sta4]: STA {
            base_no = parent.base_sta4;
            @display("p=$origin4,$origin4,ri,$r_sta4,$r_sta4");
//            @display("p=0,0,ri,$r_sta4,$r_sta4");
        }
        stas5[num_sta5]: STA {
            base_no = parent.base_sta5;
            @display("p=$origin5,$origin5,ri,$r_sta5,$r_sta5");
//            @display("p=0,0,ri,$r_sta5,$r_sta5");
        }
        stas6[num_sta6]: STA {
            base_no = parent.base_sta6;
            @display("p=$origin6,$origin6,ri,$r_sta6,$r_sta6");
//            @display("p=0,0,ri,$r_sta6,$r_sta6");
        }
    connections:
        for i=0..0 {
            ccos[i].bs_port++ <--> chn_prm <--> BS.bs_port++;
        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f0++ <--> chn_frm <--> stas1[j].frm_f0++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f1++ <--> chn_frm <--> stas1[j].frm_f1++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f2++ <--> chn_frm <--> stas1[j].frm_f2++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f3++ <--> chn_frm <--> stas1[j].frm_f3++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f4++ <--> chn_frm <--> stas1[j].frm_f4++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f5++ <--> chn_frm <--> stas1[j].frm_f5++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f6++ <--> chn_frm <--> stas1[j].frm_f6++;
//        }
//        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_cco*i&&j<(num_sta1+0.0)/num_cco*(i+1) {
//            ccos[i].frm_f7++ <--> chn_frm <--> stas1[j].frm_f7++;
//        }
        
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f0++ <--> chn_frm <--> stas1[j].frm_f0++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f1++ <--> chn_frm <--> stas1[j].frm_f1++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f2++ <--> chn_frm <--> stas1[j].frm_f2++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f3++ <--> chn_frm <--> stas1[j].frm_f3++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f4++ <--> chn_frm <--> stas1[j].frm_f4++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f5++ <--> chn_frm <--> stas1[j].frm_f5++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f6++ <--> chn_frm <--> stas1[j].frm_f6++;
        }
        for i=0..0, for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/1*i&&j<(num_sta1+0.0)/1*(i+1) {
            ccos[i].frm_f7++ <--> chn_frm <--> stas1[j].frm_f7++;
        }
        
        
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f0++ <--> chn_frm <--> stas1[j].frm_f0++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f1++ <--> chn_frm <--> stas1[j].frm_f1++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f2++ <--> chn_frm <--> stas1[j].frm_f2++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f3++ <--> chn_frm <--> stas1[j].frm_f3++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f4++ <--> chn_frm <--> stas1[j].frm_f4++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f5++ <--> chn_frm <--> stas1[j].frm_f5++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f6++ <--> chn_frm <--> stas1[j].frm_f6++;
        }
        for i=1..(num_acco-1), for j=0..(num_sta1-1), if uniform(0, 1)<connectness_cco&&j>=(num_sta1+0.0)/num_acco*(i+0)&&j<(num_sta1+0.0)/num_acco*(i+1) {
            accos[i].frm_f7++ <--> chn_frm <--> stas1[j].frm_f7++;
        }
        
        
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f0++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f0++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f0++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f0++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f0++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f0++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f0++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f0++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f0++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f0++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f0++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f0++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f1++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f1++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f1++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f1++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f1++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f1++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f1++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f1++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f1++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f1++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f1++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f1++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f2++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f2++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f2++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f2++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f2++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f2++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f2++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f2++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f2++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f2++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f2++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f2++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f3++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f3++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f3++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f3++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f3++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f3++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f3++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f3++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f3++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f3++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f3++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f3++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f4++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f4++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f4++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f4++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f4++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f4++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f4++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f4++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f4++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f4++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f4++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f4++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f5++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f5++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f5++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f5++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f5++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f5++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f5++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f5++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f5++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f5++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f5++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f5++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f6++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f6++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f6++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f6++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f6++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f6++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f6++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f6++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f6++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f6++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f6++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f6++;
        }
        for i=0..(num_sta1-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta1 {
        	stas1[i].frm_f7++ <--> chn_frm <--> stas1[(i+j+num_sta1)%num_sta1].frm_f7++;
        }
        for i=0..(num_sta2-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta2 {
        	stas2[i].frm_f7++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f7++;
        }
        for i=0..(num_sta3-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta3 {
        	stas3[i].frm_f7++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f7++;
        }
        for i=0..(num_sta4-1), for j=1..max_neighbor, if depth>3&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta4 {
        	stas4[i].frm_f7++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f7++;
        }
        for i=0..(num_sta5-1), for j=1..max_neighbor, if depth>4&&uniform(0, 1)<connectness&&i!=(j+i)%num_sta5 {
        	stas5[i].frm_f7++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f7++;
        }
        for i=0..(num_sta6-1), for j=1..max_neighbor, if uniform(0, 1)<connectness&&i!=(j+i)%num_sta6 {
        	stas6[i].frm_f7++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f7++;
        }


        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f0++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f0++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f0++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f0++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f0++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f0++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f0++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f0++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f0++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f0++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f1++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f1++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f1++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f1++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f1++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f1++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f1++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f1++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f1++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f1++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f2++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f2++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f2++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f2++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f2++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f2++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f2++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f2++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f2++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f2++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f3++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f3++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f3++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f3++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f3++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f3++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f3++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f3++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f3++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f3++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f4++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f4++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f4++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f4++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f4++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f4++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f4++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f4++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f4++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f4++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f5++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f5++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f5++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f5++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f5++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f5++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f5++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f5++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f5++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f5++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f6++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f6++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f6++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f6++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f6++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f6++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f6++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f6++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f6++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f6++;
        }
        for i=0..(num_sta1-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas1[i].frm_f7++ <--> chn_frm <--> stas2[(i+j+num_sta2)%num_sta2].frm_f7++;
        }
        for i=0..(num_sta2-1), for j=-max_neighbor..max_neighbor, if uniform(0, 1)<connectness {
        	stas2[i].frm_f7++ <--> chn_frm <--> stas3[(i+j+num_sta3)%num_sta3].frm_f7++;
        }
        for i=0..(num_sta3-1), for j=-max_neighbor..max_neighbor, if depth>3&&uniform(0, 1)<connectness {
        	stas3[i].frm_f7++ <--> chn_frm <--> stas4[(i+j+num_sta4)%num_sta4].frm_f7++;
        }
        for i=0..(num_sta4-1), for j=-max_neighbor..max_neighbor, if depth>4&&uniform(0, 1)<connectness {
        	stas4[i].frm_f7++ <--> chn_frm <--> stas5[(i+j+num_sta5)%num_sta5].frm_f7++;
        }
        for i=0..(num_sta5-1), for j=-max_neighbor..max_neighbor, if depth>5&&uniform(0, 1)<connectness {
        	stas5[i].frm_f7++ <--> chn_frm <--> stas6[(i+j+num_sta6)%num_sta6].frm_f7++;
        }

}

