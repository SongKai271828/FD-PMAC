/*
 * Txc1.cc
 *
 *  Created on: 2022��10��15��
 *      Author: Lenovo
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <math.h>
#include <algorithm>

#include "frame_m.h"

using namespace omnetpp;

/**
 * Derive the Txc1 class from cSimpleModule. In the Tictoc1 network,
 * both the `tic' and `toc' modules are Txc1 objects, created by OMNeT++
 * at the beginning of the simulation.
 */
class STA : public cSimpleModule
{
public:
#ifdef debug0
    simsignal_t collisionSignal;
    simsignal_t checkedSignal;
    simsignal_t timeSignal;
    simsignal_t uploadSignal;
    simsignal_t uploadDepthSignal;
    simsignal_t uploadFinishSignal;
#endif
    int online;
    int FREQS;
    bool pco_finish;
    int networking_start_s;
    int failure_cnt;
    bool debug_flag;
    bool bsy_flag;
    int upload_wait_s;
    int upload_start_s;
    int upload_state;
    int recv_state;
    simtime_t release_time[8];
    simtime_t startTime, endTime;
    int recv_deltaT[8];
    std::vector<int> upload_deltaT;
    std::vector<int> local_data;
    int local_index;
    std::map<int, int> recvd_list;
    bool used_freq[8];
    bool busy_freq[8];
    int uploaded_freq[8];
    int no;
    int mac;
    int focus;
    int pco_state;
    int depth;
    int deltaTr[8];
    /* ipv6 related */
    int ipv6_prefix[2];
    /* ipv6 related */
    std::map<int, std::vector<int>> ipv6_list;
    int ipv6_addr[4];
    std::map<int, int> deltaT_cnt[8];
    std::map<int, std::vector<int>> deltaT[8];
    std::vector<int> tqu_buffer;
    std::vector<int> ncf_buffer[8];
    std::vector<int> route_buffer;
    std::map<int, int> parents;
    std::map<int, std::vector<int>> route_list;
    simtime_t tic[8];
    simtime_t toc[8];
    simtime_t global_time[8];
    int collision_checked[8];
    int collision_times[8];
    std::map<int, int> sons_nc[8];
    cMessage *events[8][NUM_EVENT_S];
    char names[8][NUM_EVENT_S][50];
    int node_state[8];
    int state[8];
    void broadcast_freq(frame *msg, int freq, simtime_t delay);
    void debug(int node_state, int freq);
    void load_data(frame *msg, std::vector<int> data);
    void save_route(frame *msg);

protected:
    // The following redefined virtual function holds the algorithm.
    virtual frame *generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data);
    virtual void forwardMessage(frame *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(STA);

void STA::load_data(frame *msg, std::vector<int> data)
{
    int data_len = data.size();
    for (int i = 1; i <= data_len; i++)
    {
        msg->data[i] = data[i - 1];
    }
    msg->data[0] = data_len;
}

void STA::debug(int node_state, int freq)
{
    // EV << "----------------------------------------" << endl;
    // EV << "deltaTr[" << freq << "] of STA_" << no << ": (size is " << deltaTt[freq].size() << ")" << endl;
    // EV << "{ ";
    // for (int i = 0; i < deltaTt[freq].size(); i++)
    // {
    //     EV << deltaTt[freq][i] << ", ";
    //     if ((i + 1) % 20 == 0)
    //     {
    //         EV << endl;
    //     }
    // }
    // EV << " }" << endl;
    // EV << "deltaTt[" << freq << "] of STA_" << no << ": (size is " << deltaTt[freq].size() << ")" << endl;
    // EV << "{ ";
    // for (int i = 0; i < deltaTt[freq].size(); i++)
    // {
    //     EV << deltaTt[freq][i] << ", ";
    //     if ((i + 1) % 20 == 0)
    //     {
    //         EV << endl;
    //     }
    // }
    // EV << " }" << endl;
    // EV << "----------------------------------------" << endl;
}

void STA::save_route(frame *msg)
{
    route_buffer.clear();
    for (int i = 1; i <= msg->route[0]; i++)
    {
        route_buffer.push_back(msg->route[i]);
    }
    route_buffer.push_back(msg->freq);
}

void STA::initialize()
{
#ifdef debug0
    collisionSignal = registerSignal("collision");
    checkedSignal = registerSignal("checked");
    timeSignal = registerSignal("elapsedTime");
    uploadSignal = registerSignal("upload");
    uploadDepthSignal = registerSignal("uploadDepth");
    uploadFinishSignal = registerSignal("uploadFinish");
#endif
    no = getIndex() + par("base_no").intValue();
    FREQS = par("freq_num").intValue();
    mac = no;
    focus = -1;
    pco_state = init;
    char tmp[50];
    for (int j = 0; j < FREQS; j++)
    {
        global_time[j] = 0;
        collision_checked[j] = 0;
        collision_times[j] = 0;
        deltaTr[j] = 0;
        tic[j] = 0;
        toc[j] = 0;
        used_freq[j] = 0;
        busy_freq[j] = 0;
        uploaded_freq[j] = 0;
        node_state[j] = init;
        state[j] = init;
        for (int i = 0; i < NUM_EVENT_S; i++)
        {
            sprintf(tmp, "{STA: %d, Freq: %d, Event: %d}", no, j, i);
            strcpy(names[j][i], tmp);
            events[j][i] = new cMessage(names[j][i]);
        }
        release_time[j] = 0;
    }
    online = 0;
    depth = -1;
    pco_finish = false;
    upload_state = init;
    recv_state = init;
    local_index = 0;
    failure_cnt = 0;
    debug_flag = 0;
    upload_start_s = 0;
    for (int i = 0; i < frm_load; i++)
    {
        local_data.push_back(mac);
    }
}

void STA::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through
    // gate `out'. Because both `tic' and `toc' does the same, the message
    // will bounce between the two.
    // if (no > 80 && no < 2048)
    // {
    //     return;
    // }
    // if (uniform(0, 1) < 0.001)
    // {
    //     return;
    // }
    for (int i = 0; i < FREQS; i++)
    {
        if (msg == events[i][pte_m])
        {
            node_state[i] = pte_out;
            toc[i] = tic[i];
            simtime_t tmp = simTime();
            deltaTr[i] = (int)round((long double)(tmp.dbl() - tic[i].dbl()) / prm_slot_s);
            std::vector<int> route = {no, 0};
            std::vector<int> data = {};
            frame *msg_buffer = generateMessage(REQ, DOWN, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            return;
        }
        else if (msg == events[i][pte_e])
        {
            state[i] = pte_out;
            if (i + 1 == FREQS)
            {
                std::vector<int> data = {};
                for (int j = 0; j < FREQS; j++)
                {
                    data.push_back(deltaT[j].size());
                    if (deltaT[j].size() > 0)
                        pco_state = pte_out;
                }
                int freq = route_buffer[route_buffer.size()-1];
                route_buffer.pop_back();
                frame *msg_buffer = generateMessage(PTE_S, UP, freq, route_buffer, data);
                broadcast_freq(msg_buffer, freq, 0);
                if (pco_state == pte_in)
                {
                    pco_state = init;
                }
            }
            return;
        }
        else if (msg == events[i][pte_s])
        {
            state[i] = pte_in;
            std::vector<int> data = {};
            std::vector<int> route = {no, 0};
            frame *msg_buffer = generateMessage(NET, DOWN, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            tic[i] = simTime();
            scheduleAt(tic[i] + (pte_slot_num + 2) * prm_slot_s, events[i][pte_e]);
            return;
        }
        else if (msg == events[i][tqu_s])
        {
            state[i] = tqu_in;
            std::vector<int> data = {};
            /* ipv6 related */
            data.push_back(ipv6_prefix[0]);
            data.push_back(ipv6_prefix[1]);
            data.push_back(depth);
            std::vector<int> route = {no, 0};
            for (auto j : deltaT[i])
            {
                if (j.second.size() == 0)
                {
                    if (deltaT_cnt[i][j.first] < 2)
                        data.push_back(j.first);
                    else
                    {
#ifdef debug0
                        for (int k = 0; k < deltaT_cnt[i][j.first]; k++) emit(collisionSignal, (long)(1));
#endif
                    }
                }
            }
            if (data.size() > tqu_offset)
            {
                frame *msg_buffer = generateMessage(TQU, DOWN, i, route, data);
                broadcast_freq(msg_buffer, i, 0);
                scheduleAt(simTime() + (data.size() + 2 - tqu_offset) * frm_slot_s, events[i][tqu_e]);
            }
            else
            {
                if (i + 1 < FREQS)
                {
                    scheduleAt(simTime() + interval_s, events[i + 1][tqu_s]);
                }
                else
                {
                    scheduleAt(simTime() + interval_s, events[i][tqu_e]);
                }
            }
            return;
        }
        else if (msg == events[i][tqu_e])
        {
            std::vector<int> data = {};
            for (int k = 0; k < tqu_buffer.size(); k += 2 + FREQS)
            {
                if (tqu_buffer[k])
                {
                    for (int j = 0; j < 2 + FREQS; j++)
                    {
                        data.push_back(tqu_buffer[k + j]);
                    }
                }
            }
            tqu_buffer = data;
            if (tqu_buffer.size() == 0)
            {
                tic[i] = 0;
                toc[i] = 0;
                state[i] = init;
                tqu_buffer.clear();
            }
            else
            {
                for (int k = 0; k < tqu_buffer.size(); k += 2 + FREQS)
                {
                    if (tqu_buffer[k] > 0)
                    {
                        for (int j = 0; j < FREQS; j++)
                        {
                            if (tqu_buffer[k + j + 2] > 0 && deltaT[j].find(tqu_buffer[k + j + 2]) != deltaT[j].end())
                            {
                                bool found = 0;
                                for (auto m : deltaT[j][tqu_buffer[k + j + 2]])
                                {
                                    if (m == tqu_buffer[k])
                                    {
                                        found = 1;
                                        break;
                                    }
                                }
                                if (!found)
                                {
                                    deltaT[j][tqu_buffer[k + j + 2]].push_back(tqu_buffer[k]);
                                    if (deltaT[j][tqu_buffer[k + j + 2]].size() > 2 && j != i)
                                    {
#ifdef debug0
                                        emit(checkedSignal, (long)(1));
#endif
                                    }
                                    else if (deltaT[j][tqu_buffer[k + j + 2]].size() > 1 && j != i)
                                    {
#ifdef debug0
                                        // emit(checkedSignal, (long)(1));
                                        emit(checkedSignal, (long)(1));
#endif
                                    }
                                }
                            }
                            else if (tqu_buffer[k + j + 2] > 0 && deltaT[j].find(tqu_buffer[k + j + 2]) == deltaT[j].end() && j != i)
                            {
                                collision_checked[j]++;
                                deltaT[j][tqu_buffer[k + j + 2]].push_back(tqu_buffer[k]);
#ifdef debug0
                                emit(checkedSignal, (long)(1));
#endif
                            }
                        }
                        if (tqu_buffer[k + 1] == 1)
                        {
                            for (int j = 0; j < FREQS; j++)
                            {
                                if (tqu_buffer[k + j + 2] > 0) {
                                    if (route_list.find(tqu_buffer[k]) == route_list.end())
                                    {
                                        route_list[tqu_buffer[k]] = {};
                                        route_list[tqu_buffer[k]].push_back(1);
                                        route_list[tqu_buffer[k]].push_back(tqu_buffer[k]);
                                        route_list[tqu_buffer[k]].push_back(0x00000001 << j);
                                    }
                                    else
                                    {
                                        route_list[tqu_buffer[k]][2] |= (0x00000001 << j);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            tqu_buffer.clear();
            if (i + 1 < FREQS)
            {
                scheduleAt(simTime() + interval_s, events[i + 1][tqu_s]);
            }
            else
            {
                std::map<int, int> tmp;
                std::vector<int> data = {};
                for (int j = 0; j < FREQS; j++)
                {
                    for (auto k : deltaT[j])
                    {
                        for (auto n : k.second)
                        {
                            if (tmp.find(n) == tmp.end())
                            {
                                tmp[n] = 1;
                            }
                            else
                            {
                                tmp[n]++;
                            }
                            
                        }
                    }
                }
                for (auto j : tmp)
                {
                    data.push_back(j.first);
                    data.push_back(j.first);
                    data.push_back(j.second);
                }
                if (data.size() > 0)
                    pco_state = tqu_out;
                else
                    pco_state = init;
                int freq = route_buffer[route_buffer.size() - 1];
                route_buffer.pop_back();
                frame *msg_buffer = generateMessage(TQU_S, UP, freq, route_buffer, data);
                broadcast_freq(msg_buffer, freq, 0);
            }
            return;
        }
        else if (msg == events[i][ncf_s])
        {
            // if (i == 0) {startTime = simTime(); endTime = startTime;}
            state[i] = ncf_in;
            std::vector<int> route = {no, 0};
            std::vector<int> data = {};
            data.push_back(depth);
            data.push_back(upload_wait_s - ((int)round(simTime().dbl() + 2 * frm_slot_s) - networking_start_s));
            for (auto j : deltaT[i])
            {
                for (auto k : j.second)
                {
                    if (route_list.find(k) == route_list.end())
                    {
                        sons_nc[i][k] = j.first;
                        data.push_back(k);
                        /* ipv6 related */
                        ipv6_list[k] = {};
                        ipv6_list[k].push_back(ipv6_prefix[0]);
                        ipv6_list[k].push_back(ipv6_prefix[1]);
                        ipv6_list[k].push_back(0);
                        ipv6_list[k].push_back(k);
                    }
                }
            }
            if (data.size() > ncf_offset)
            {
                frame *msg_buffer = generateMessage(NCF, DOWN, i, route, data);
                broadcast_freq(msg_buffer, i, 0);
                scheduleAt(simTime() + (data.size() + 2 - ncf_offset) * frm_slot_s, events[i][ncf_e]);
            }
            else
            {
                if (i + 1 < FREQS)
                {
                    scheduleAt(simTime() + interval_s, events[i + 1][ncf_s]);
                }
                else
                {
                    scheduleAt(simTime() + interval_s, events[i][ncf_e]);
                }
            }
            return;
        }
        else if (msg == events[i][ncf_e])
        {
            state[i] = ncf_out;
            for (int j = 0; j < ncf_buffer[i].size(); j += 2 + 4)
            {
                /* ipv6 related */
                std::vector<int> ipv6 = ipv6_list[ncf_buffer[i][j]];
                if (ncf_buffer[i][j + 2] == ipv6[0] && ncf_buffer[i][j + 3]  == ipv6[1] && ncf_buffer[i][j + 4] == ipv6[2] && ncf_buffer[i][j + 5] == ipv6[3])
                {
                    sons_nc[i].erase(ncf_buffer[i][j]);
                    if (route_list.find(ncf_buffer[i][j]) == route_list.end())
                    {
                        route_list[ncf_buffer[i][j]] = {};
                        route_list[ncf_buffer[i][j]].push_back(1);
                        route_list[ncf_buffer[i][j]].push_back(no);
                        route_list[ncf_buffer[i][j]].push_back(0x00000001 << i);
                    }
                    else
                    {
                        route_list[ncf_buffer[i][j]][2] |= (0x00000001 << i);
                    }
                }
            }
            std::vector<int> route = {no, 0};
            std::vector<int> data = {};
            for (auto j : sons_nc[i])
            {
                j.second = 0;
                data.push_back(j.first);
            }
            frame *msg_buffer = generateMessage(ERR, DOWN, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            if (i + 1 < FREQS)
            {
                scheduleAt(simTime() + frm_slot_s + interval_s, events[i + 1][ncf_s]);
            }
            else
            {
                data.clear();
                for (auto j : route_list)
                {
                    data.push_back(j.first);
                    data.push_back(j.second[2]);
                }
                int freq = route_buffer[route_buffer.size() - 1];
                route_buffer.pop_back();
                frame *msg_buffer = generateMessage(NCF_S, UP, freq, route_buffer, data);
                broadcast_freq(msg_buffer, freq, 0);
                pco_state = (data.size() > 0) ? ncf_out : init;
                pco_finish = true;
                if (data.size() > 0) 
                {
                    upload_state = -1;
                    // cancelEvent(events[0][upload]);
                }
                else
                {
                    state[i] = init;
                }
            }
            state[i] = ncf_out;
            ncf_buffer[i].clear();
            tqu_buffer.clear();
            sons_nc[i].clear();
            return;
        }
        else if (msg == events[i][upload])
        {
            if (online && (upload_state == init || upload_state == -2) && i == 0)
            {
#ifdef debug0
                emit(uploadDepthSignal, (long)(depth));
#endif
                if (depth == 1)
                {
#ifdef debug0
                    emit(uploadFinishSignal, (long)(depth));
#endif
                    std::vector<int> data = {};
                    data.push_back(0);
                    data.push_back(mac);
                    data.push_back(0);
                    data.push_back(depth);
                    std::vector<int> route = {0, 0};
                    std::vector<int> available = {};
                    int available_freq = 0;
                    for (auto j : parents)
                    {
                        available_freq |= j.second;
                    }
                    for (int j = 0; j < FREQS; j++)
                    {
                        if ((available_freq >> j) % 2 && busy_freq[j] == 0) available.push_back(j);
                    }
                    frame *msg_buffer = generateMessage(DAT, UP, available[0], route, data);
                    broadcast_freq(msg_buffer, available[0], 0);
                    upload_state = upload_out;
                    return;
                }
                upload_state = upload_in;
                local_index = 0;
                for (int j = 0; j < FREQS; j++) uploaded_freq[j] = 0;
                printf("STA %d START\n", mac);
                cancelEvent(events[i][upload]);
                scheduleAt(simTime() + prm_slot_s * (int)ceil(uniform(0, 1) * upload_delay_num), events[i][upload]);
                return;
            }
            else if (upload_state == -1)
            {
                return;
            }
            else if (upload_state == upload_in || upload_state == upload_wait)
            {
                upload_deltaT.clear();
                int available_freq = 0;
                std::vector<int> available = {};
                for (auto j : parents)
                {
                    available_freq |= j.second;
                }

//                 bool upload_finish = true;
//                 for (int j = 0; j < FREQS; j++)
//                 {
//                     if (uploaded_freq[j] == 0 && (available_freq >> j) % 2) upload_finish = 0;
//                 }
//                 if (upload_finish)
//                 {
//                     upload_state = upload_out;
// #ifdef debug0
//                     if (depth > 0) emit(uploadFinishSignal, (long)(depth));
// #endif
//                     return;
//                 }

                bool try_other_freq = true;
                for (int j = 0; j < FREQS; j++)
                {
                    if ((available_freq >> j) % 2 && busy_freq[j] == 0) available.push_back(j);
                }
                
                if (available.size() == 0)
                {
                    upload_state = upload_wait;
                    return;
                }
                random_shuffle(available.begin(), available.end());
                focus = available[0];
                tic[focus] = simTime();
                std::vector<int> data = {0};
                std::vector<int> route = {0, 0};
                frame *msg_buffer = generateMessage(UPL, UP, focus, route, data);
                broadcast_freq(msg_buffer, focus, 0);
                cancelEvent(events[focus][upload_timeout]);
                scheduleAt(simTime() + busy_slot_num * prm_slot_s, events[focus][upload_timeout]);
                return;
            }
            else
            {
                return;
            }
        }
        else if (msg == events[i][upload_timeout])
        {
            if (busy_freq[i] == 1 || i != focus)
            {
                upload_state = upload_in;
                focus = -1;
                cancelEvent(events[i][upload]);
                scheduleAt(simTime() + prm_slot_s, events[i][upload]);
                return;
            }
            upload_state = upload_ready;
            upload_deltaT.clear();
            scheduleAt(simTime() + upload_slot_num * prm_slot_s, events[i][upload_start]);
            return;
        }
        else if (msg == events[i][upload_start])
        {
            if (upload_deltaT.size() == 0)
            {
                // if (depth <= 1)
                // {
                //     return;
                // }
                // uploaded_freq[i] = -1;
                if (++failure_cnt >= 20)
                {
                //    emit(uploadFinishSignal, (long)(depth));
                    upload_state = upload_out;
                    return;
                }
                focus = -1;
                upload_state = upload_in;
                busy_freq[i] = 1;
                if (release_time[i] < simTime() + prm_slot_s * (upload_slot_num + busy_slot_num) + frm_slot_s)
                {
                    release_time[i] = simTime() + prm_slot_s * (upload_slot_num + busy_slot_num) + frm_slot_s;
                    cancelEvent(events[i][release_freq]);
                    scheduleAt(release_time[i], events[i][release_freq]);
                }
                cancelEvent(events[i][upload]);
                scheduleAt(simTime() + interval_s, events[i][upload]);
                return;
            }
            upload_state = upload_ongoing;
            std::vector<int> data = {};
            data.push_back(upload_deltaT[upload_deltaT.size() / 2]);
            data.push_back(mac);
            data.push_back(0);
            data.push_back(depth);
            if (!debug_flag)
            {
                debug_flag = 1;
            }
            for (int j = 0; j < frm_load; j++) 
            {
                data.push_back(local_data[local_index]);
                local_index++;
                if (local_index >= local_data.size())
                {
                    data[2] = 1;
                    upload_deltaT.clear();
                    upload_state = upload_out;
#ifdef debug0
                    emit(uploadFinishSignal, (long)(depth));
#endif
                    break;
                }
            }
            std::vector<int> route = {0, 0};
            frame *msg_buffer = generateMessage(DAT, UP, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            return;
        }
        else if (msg == events[i][release_freq])
        {
            busy_freq[i] = 0;
            if (upload_state == upload_wait)
            {
                focus = -1;
                cancelEvent(events[i][upload]);
                scheduleAt(simTime() + interval_s, events[i][upload]);
            }
            return;
        }
        else if (msg == events[i][recv_req])
        {
            std::vector<int> data = {0};
            std::vector<int> route = {0, 0};
            frame *msg_buffer = generateMessage(BSY, DOWN, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            return;
        }
    }
    frame *frm_msg = check_and_cast<frame *>(msg);
    int type = frm_msg->getType();
    int dir = frm_msg->getDir();
    int freq = frm_msg->freq;
    int src = frm_msg->getSource();
    int dst = frm_msg->route[frm_msg->getHopCount()];
    if (dst != 0 && dst != no)
        return;
    if (dst == no && frm_msg->getDestination() != no)
    {
        forwardMessage(frm_msg);
        return;
    }

    if (type == NET && !pco_finish)
    {
        simtime_t tmp = simTime();
        for (int j = 0; j < FREQS; j++) if (node_state[j] != pte_in) deltaTr[freq] = 0;
        if (node_state[freq] == init)
        {
            node_state[freq] = pte_in;
            tic[freq] = tmp;
            toc[freq] = tmp + (2 + (uniform(0, 1) * (pte_slot_num - 4))) * prm_slot_s;
            // int wait = intrand(pte_slot_num - 4) + 2;
            // toc[freq] = tmp + (double)(wait * prm_slot_s);
            // deltaTr[freq] = wait;
            scheduleAt(toc[freq], events[freq][pte_m]);
        }
    }
    else if (type == REQ && state[freq] == pte_in)
    {
        simtime_t tmp = simTime();
        int delta_t = (int)round((long double)(tmp.dbl() - tic[freq].dbl()) / prm_slot_s);
        deltaT_cnt[freq][delta_t]++;
        if (deltaT[freq].find(delta_t) == deltaT[freq].end())
        {
            deltaT[freq][delta_t] = {};
            deltaT_cnt[freq][delta_t] = 1;
        }
        else
        {
            deltaT_cnt[freq][delta_t]++;
#ifdef debug0
            // emit(collisionSignal, (long)(1));
#endif
        }
        return;
    }
    else if (type == TQU && node_state[freq] == pte_out && dst == 0 && dir == DOWN && (depth < 0 || frm_msg->data[3] == depth - 1))
    {
        int delta_T = deltaTr[freq];
        /* ipv6 related */
        ipv6_prefix[0] = frm_msg->data[1];
        ipv6_prefix[1] = frm_msg->data[2];
        ipv6_addr[0] = ipv6_prefix[0];
        ipv6_addr[1] = ipv6_prefix[1];
        ipv6_addr[2] = 0;
        ipv6_addr[3] = mac;
        for (int i = 1 + tqu_offset; i <= frm_msg->data[0]; i++)
        {
            if (delta_T == frm_msg->data[i])
            {
                EV << "SRC: " << frm_msg->getSource() << ", Type: " << type << ", Freq: " << freq << " (STA " << no << ")" << frm_msg->data[i] << endl;
                std::vector<int> route = {frm_msg->getSource(), 0};
                std::vector<int> data = {};
                data.push_back(mac);
                if (frm_msg->data[3] == depth - 1 && depth > 0)
                {
                    if (used_freq[freq])
                    {
                        node_state[freq] = ncf_out;
                    }
                    data.push_back(1);
                    for (int j = 0; j < FREQS; j++)
                    {
                        if (deltaTr[j] > 0)
                        {
                            if (parents.find(src) == parents.end())
                            {
                                parents[src] = 0x00000001 << j;
                            }
                            else
                            {
                                parents[src] |= 0x00000001 << j;
                            }
                        }
                    }
                }
                else
                {
                    data.push_back(0);
                }
                for (int j = 0; j < FREQS; j++)
                {
                    data.push_back(deltaTr[j]);
                    if (deltaTr[j] > 0 && node_state[freq] < ncf_in) node_state[j] = ncf_in;
                    deltaTr[j] = 0;
                }
                frame *msg_buffer = generateMessage(TQU, UP, freq, route, data);
                EV << "sta_" << no << " send TQU on freq " << freq << endl;
                /* ipv6 related */
                broadcast_freq(msg_buffer, freq, (i - tqu_offset) * frm_slot_s);
            }
        }
    }
    else if (type == NCF && node_state[freq] == ncf_in && dst == 0 && dir == DOWN)
    {
        for (int i = 1 + ncf_offset; i <= frm_msg->data[0]; i++)
        {
            if (frm_msg->data[i] == mac)
            {
                std::vector<int> route = {frm_msg->getSource(), 0};
                std::vector<int> data = {};
                data.push_back(mac);
                data.push_back(no);
                /* ipv6 related */
                data.push_back(ipv6_addr[0]);
                data.push_back(ipv6_addr[1]);
                data.push_back(ipv6_addr[2]);
                data.push_back(ipv6_addr[3]);
                frame *msg_buffer = generateMessage(NCF, UP, freq, route, data);
                EV << "sta_" << no << " send NCF on freq " << freq << endl;
                broadcast_freq(msg_buffer, freq, (i - ncf_offset) * frm_slot_s);
                depth = frm_msg->data[1] + 1;
                node_state[freq] = ncf_out;
                deltaTr[freq] = 0;
                if (online == 0)
                {
                    upload_wait_s = frm_msg->data[2];
                    networking_start_s = int(round(simTime().dbl()));
                    upload_start_s = networking_start_s - depth * frm_slot_s + upload_wait_s + 2 * (10 - depth);
#ifdef debug0
                    emit(uploadSignal, (long)(upload_start_s));
#endif
                    printf("STA: %d, upload time: %d\n", mac, networking_start_s + upload_wait_s);
                    cancelEvent(events[0][upload]);
                    scheduleAt(upload_start_s, events[0][upload]);
                }
                online++;
                return;
            }
        }
    }
    else if (type == ERR && node_state[freq] == ncf_out && dst == 0 && dir == DOWN)
    {
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            if (frm_msg->data[i] == mac)
            {
                node_state[freq] = init;
                deltaTr[freq] = 0;
                online--;
                if (online == 0) 
                {
                    cancelEvent(events[0][upload]);
                }
                return;
            }
        }
        used_freq[freq] = 1;
//        node_state[freq] = ncf_out;
        if (parents.find(src) == parents.end())
        {
            parents[src] = (0x00000001 << freq);
        }
        else
        {
            parents[src] |= (0x00000001 << freq);
        }
        return;
    }
    else if (type == PTE_S && dst == no && dir == DOWN && node_state[freq] == ncf_out)
    {
        save_route(frm_msg);
        simtime_t delay = interval_s;
        for (int i = 0; i < FREQS; i++)
        {
            if (pco_state == init)
            {
                scheduleAt(simTime() + delay, events[i][pte_s]);
            }
            delay += prm_slot_s;
        }
        pco_state = pte_in;
        return;
    }
    else if (type == TQU_S && dst == no && dir == DOWN && pco_state == pte_out)
    {
        pco_state = tqu_in;
        save_route(frm_msg);
        scheduleAt(simTime() + interval_s, events[0][tqu_s]);
        return;
    }
    else if (type == NCF_S && dst == no && dir == DOWN && pco_state == tqu_out)
    {
        pco_state = ncf_in;
        save_route(frm_msg);
        scheduleAt(simTime() + interval_s, events[0][ncf_s]);
        return;
    }
    else if (type == TQU && dir == UP && src == no && state[freq] == tqu_in)
    {
        if (simTime().dbl() - global_time[freq].dbl() > 0.9 * frm_slot_s)
        {
            global_time[freq] = simTime();
        }
        else
        {
            global_time[freq] = simTime();
            tqu_buffer[tqu_buffer.size() - FREQS - 2] = 0;
            return;
        }
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            tqu_buffer.push_back(frm_msg->data[i]);
        }
    }
    else if (type == NCF && dir == UP && src == no && state[freq] == ncf_in)
    {
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            ncf_buffer[freq].push_back(frm_msg->data[i]);
        }
    }
    else if (type == BSY && dir == DOWN)
    {
        if (focus == freq && upload_state == upload_ready)
        {
            busy_freq[freq] = 0;
            upload_deltaT.push_back((int)round((simTime().dbl() - tic[freq].dbl()) / prm_slot_s));
        }
        else if (focus == freq && upload_state == upload_ongoing)
        {
            cancelEvent(events[freq][upload_start]);
            scheduleAt(simTime() + prm_slot_s, events[freq][upload_start]);
        }
        // else if (upload_state >= init && upload_state < upload_out)
        // else if (upload_state == upload_in)
        else if (recv_state != recv_in && upload_state < upload_out)
        {
            busy_freq[freq] = 1;
            if (release_time[freq] < simTime() + prm_slot_s * upload_slot_num + frm_slot_s)
            {
                release_time[freq] = simTime() + prm_slot_s * upload_slot_num + frm_slot_s;
                cancelEvent(events[freq][release_freq]);
                scheduleAt(release_time[freq], events[freq][release_freq]);
            }
            if (upload_state == upload_in && focus == freq)
            {
                focus = -1;
                cancelEvent(events[freq][upload_timeout]);
                cancelEvent(events[freq][upload]);
                scheduleAt(simTime() + prm_slot_s, events[freq][upload]);
            }
        }
    }
    else if (type == UPL && dir == UP && upload_state < 0)
    {
        if (focus == -1 && busy_freq[freq] == 0)
        {
            focus = freq;
            recv_state = recv_in;
            tic[freq] = simTime();
            int wait = 1 + (int)floor(uniform(0, 1) * (upload_slot_num - 1));
            recv_deltaT[freq] = busy_slot_num + wait;
            toc[freq] = tic[freq] + recv_deltaT[freq] * prm_slot_s;
            cancelEvent(events[freq][recv_req]);
            scheduleAt(toc[freq], events[freq][recv_req]);
        }
        else if (focus == freq && recv_state == recv_in)
        {
            int delta = (int)ceil((toc[freq].dbl() - simTime().dbl()) / prm_slot_s);
            if (delta >= busy_slot_num)
            {
                cancelEvent(events[freq][recv_req]);
                int interval = (int)round((simTime().dbl() - tic[freq].dbl()) / prm_slot_s);
                int wait = 1 + (int)floor(uniform(0, 1) * (busy_slot_num - 1));
                recv_deltaT[freq] = interval + wait;
                toc[freq] = tic[freq] + recv_deltaT[freq] * prm_slot_s;
                scheduleAt(toc[freq], events[freq][recv_req]);
            }
        }
    }
    else if (type == DAT && dir == UP && (focus == freq || focus == -1) && upload_state < 0)
    {
        if (frm_msg->data[4] != depth + 1)
        {
            focus = -1;
            recv_state = init;
            return;
        }
        upload_state = -2;
        if (recvd_list.find(frm_msg->data[2]) == recvd_list.end())
        {
            recvd_list[frm_msg->data[2]] = 1;
        }
        else
        {
            recvd_list[frm_msg->data[2]] += 1;
        }
        if (frm_msg->data[1] == recv_deltaT[freq] && recv_deltaT[freq] > 0)
        {
            for (int j = dat_offset; j <= frm_msg->data[0]; j++)
            {
                local_data.push_back(frm_msg->data[j]);
            }
            
            if (frm_msg->data[3] == 0)
            {
                std::vector<int> data = {0};
                std::vector<int> route = {0, 0};
                frame *msg_buffer = generateMessage(BSY, DOWN, freq, route, data);
                if (frm_msg->data[1] == recv_deltaT[freq] && recv_deltaT[freq] > 0)
                broadcast_freq(msg_buffer, freq, frm_slot_s);
            }
            else
            {
                focus = -1;
                recv_deltaT[freq] = 0;
                recv_state = init;
                uploaded_freq[freq]--;
                // bool enable_upload = true;
                // for (auto route_no : route_list)
                // {
                //     if (recvd_list.find(route_no.first) == recvd_list.end()) enable_upload = false;
                // }
                // if (enable_upload)
                // {
                //     for (int j = 0; j < FREQS; j++)
                //     {
                //         if (uploaded_freq[j] > 0) enable_upload = false;
                //     }
                // }
                // if (enable_upload)
                // {
                //     recv_state = recv_out;
                //     upload_state = init;
                //     cancelEvent(events[0][upload]);
                //     scheduleAt(simTime() + frm_slot_s + interval_s, events[0][upload]);
                // }

                // upload_state = -2;
                // cancelEvent(events[0][upload]);
                // if (simTime().dbl() > upload_start_s) scheduleAt(simTime() + interval_s, events[0][upload]);
                // else scheduleAt(upload_start_s, events[0][upload]);
            }
        }
        else
        {
            focus = -1;
            recv_deltaT[freq] = 0;
            busy_freq[freq] = 1;
            if (release_time[freq] < simTime() + prm_slot_s * upload_slot_num + frm_slot_s)
            {
                release_time[freq] = simTime() + prm_slot_s * upload_slot_num + frm_slot_s;
                cancelEvent(events[freq][release_freq]);
                scheduleAt(release_time[freq], events[freq][release_freq]);
            }
        }
    }
}

void STA::forwardMessage(frame *frm_msg)
{
    // Increment hop count.
    if (frm_msg->getDir() == DOWN)
    {
        frm_msg->setHopCount(frm_msg->getHopCount() + 1);
        int next = frm_msg->route[frm_msg->getHopCount()];
        if (route_list.find(next) != route_list.end()) {
            for (int i = 0; i < FREQS; i++) {
                if ((route_list[next][2] >> i) % 2) {
                    frm_msg->freq = i;
                    break;
                }
            }
        }
        broadcast_freq(frm_msg, frm_msg->freq, frm_slot_s);
    }
    else
    {
        frm_msg->setHopCount(frm_msg->getHopCount() - 1);
        int next = frm_msg->route[frm_msg->getHopCount()];
        if (parents.find(next) != parents.end())
        {
            for (int i = 0; i < FREQS; i++)
            {
                if ((parents[next] >> i) % 2)
                {
                    frm_msg->freq = i;
                    break;
                }
            }
        }
        broadcast_freq(frm_msg, frm_msg->freq, frm_slot_s);
    }
}

void STA::broadcast_freq(frame *msg, int freq, simtime_t delay)
{
    char frm_name[10];
    sprintf(frm_name, "frm_f%d", freq);
    int size = gateSize(frm_name);
    sprintf(frm_name, "frm_f%d$o", freq);

    for (int i = 0; i < size; i++)
    {
        sendDelayed((i == 0) ? msg : msg->dup(), delay, frm_name, i);
    }
}

frame *STA::generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data)
{
    char discriptor[100];
    int route_len = route.size();
    int data_len = data.size();
    int src = route[0];
    int dst = route[route_len - 1];
    int hop = (dir == DOWN) ? 2 : route.size() - 1;
    sprintf(discriptor, "{Src: %d, Dst: %d, Type: %d, Dir: %d, Freq: %d, Route_len: %d, Data_len: %d, Hop: %d}", src, dst, type, dir, freq, route_len, data_len, hop);
    frame *msg = new frame(discriptor);
    msg->setDestination(dir == DOWN ? dst : src);
    msg->setSource(src);
    msg->setType(type);
    msg->setDir(dir);
    msg->freq = freq;
    msg->data[0] = data_len;
    msg->route[0] = route_len;
    for (int i = 1; i <= data_len; i++)
    {
        msg->data[i] = data[i - 1];
    }
    for (int i = 1; i <= route_len; i++)
    {
        msg->route[i] = route[i - 1];
    }
    msg->setHopCount(hop);
    return msg;
}
