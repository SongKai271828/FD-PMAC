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

class CCO : public cSimpleModule
{
public:
#ifdef debug0
    simsignal_t arrivalSignal;
    simsignal_t collisionSignal;
    simsignal_t checkedSignal;
    simsignal_t timeSignal;
    simsignal_t uploadTimeSignal;
#endif
    bool is_master;
    int networking_start_s;
    int upload_wait_s;
    int FREQS;
    int no;
    int mac;
    int freq_now;
    int freq_max;
    int freq_seq[8];
    int max_wait_slot;
    /* ipv6 related */
    int ipv6_prefix[2];
    /* ipv6 related */
    std::map<int, std::vector<int>> ipv6_list;
    simtime_t startTime, endTime;
    simtime_t global_time[8];
    int collision_checked[8];
    int collision_times[8];
    std::map<int, int> sons_nc[8];
    std::map<int, int> deltaT_cnt[8];
    std::map<int, std::vector<int>> deltaT[8];
    std::map<int, std::vector<int>> route_list;
    std::vector<int> tqu_buffer;
    std::vector<int> ncf_buffer[8];
    std::vector<int> sons;
    std::map<int, std::vector<int>> direct_sons;
    int now_son;
    simtime_t tic[8];
    simtime_t toc[8];
    cMessage *events[8][NUM_EVENT];
    char names[8][NUM_EVENT][50];
    int state[8];
    void broadcast_freq(frame *msg, int freq, simtime_t delay);
    void debug(int stage, int freq);
    int vectorFind(std::vector<int> &vec, int num);
    void get_route(std::vector<int> &route);
protected:
    // The following redefined virtual function holds the algorithm.
    virtual frame *generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data);
    virtual void forwardMessage(frame *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(CCO);

void CCO::debug(int event, int freq)
{
    EV << "----------------------------------------" << endl;
    if (event == pte_e)
    {
        EV << "deltaT[" << freq << "] of CCO_" << no << ": (size is " << deltaT[freq].size() << ")" << endl;
        EV << "{ ";
        int i = 0;
        for (auto j : deltaT[freq])
        {
            EV << j.first << ", ";
            i++;
            if (i % 20 == 0)
            {
                EV << endl;
            }
        }
        EV << " }" << endl;
    }
    else if (event == tqu_e)
    {
        EV << "MAC of Sons of CCO_" << no << " on freq " << freq << ": (size is " << tqu_buffer.size() << ")" << endl;
        EV << "{ ";
        for (int i = 0; i < tqu_buffer.size(); i++)
        {
            EV << tqu_buffer[i] << ", ";
            if ((i + 1) % 20 == 0)
            {
                EV << endl;
            }
        }
        EV << " }" << endl;
    }
    else if (event == ncf_e)
    {
        EV << "SID of Sons of CCO_" << no << " on freq " << freq << ": (size is " << ncf_buffer[freq].size() << ")" << endl;
        EV << "{ ";
        for (int i = 0; i < ncf_buffer[freq].size(); i++)
        {
            EV << ncf_buffer[freq][i] << ", ";
            if ((i + 1) % 20 == 0)
            {
                EV << endl;
            }
        }
        EV << " }" << endl;
    }
    EV << "----------------------------------------" << endl;
}

int CCO::vectorFind(std::vector<int> &input, int num)
{
    std::vector<int>::iterator iter = std::find(input.begin(), input.end(), num);
    if (iter == input.end())
    {
        return -1;
    }
    return std::distance(input.begin(), iter);
}

void CCO::get_route(std::vector<int> &route)
{
    int num = route[0];
    if (num)
    {
        num = route_list[num][1];
        while (num != this->no)
        {
            route.insert(route.begin(), num);
            num = route_list[num][1];
        }
    }
    route.insert(route.begin(), this->no);
    return;
}

void CCO::initialize()
{
    // Initialize is called at the beginning of the simulation.
    // To bootstrap the tic-toc-tic-toc process, one of the modules needs
    // to send the first message. Let this be `tic'.
#ifdef debug0
    arrivalSignal = registerSignal("arrival");
    collisionSignal = registerSignal("collision");
    checkedSignal = registerSignal("checked");
    timeSignal = registerSignal("elapsedTime");
    uploadTimeSignal = registerSignal("uploadTime");
#endif
    upload_wait_s = par("upload_wait_s").intValue();
    FREQS = par("freq_num").intValue();
    is_master = (getIndex() == 0);
    no = getIndex() | 0x0800;
    mac = no;
    freq_now = 0;
    freq_max = 0;
    char tmp[50];
    sons = {};
    now_son = -1;
    tqu_buffer = {};
    for (int j = 0; j < FREQS; j++)
    {
        global_time[j] = 0;
        collision_checked[j] = 0;
        collision_times[j] = 0;
        tic[j] = 0;
        toc[j] = 0;
        state[j] = init;
        ncf_buffer[j] = {};
        for (int i = 0; i < NUM_EVENT; i++)
        {
            sprintf(tmp, "{CCO: %d, Freq: %d, Event: %d}", no, j, i);
            strcpy(names[j][i], tmp);
            events[j][i] = new cMessage(names[j][i]);
        }
    }
}

void CCO::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through
    // gate `out'. Because both `tic' and `toc' does the same, the message
    // will bounce between the two.
    for (int i = 0; i < FREQS; i++)
    {
        if (msg == events[i][pte_e])
        {
            state[i] = pte_out;
            // EV << "CCO_" << no << " get heard by " << deltaT[i].size() << " on freq " << i << endl;
            debug(pte_e, i);
            if (i + 1 == FREQS)
            {
                scheduleAt(simTime() + frm_slot_s * 2, events[0][tqu_s]);
            }
            return;
        }
        else if (msg == events[i][pte_s])
        {
            state[i] = pte_in;
            std::vector<int> data = {};
            std::vector<int> route = {0};
            get_route(route);
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
            data.push_back(0);
            std::vector<int> route = {0};
            get_route(route);
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
                if (i + 1 < FREQS) {
                    scheduleAt(simTime() + interval_s, events[i + 1][tqu_s]);
                }
                else
                {
                    scheduleAt(simTime() + interval_s, events[0][ncf_s]);
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
            else {
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
                scheduleAt(simTime() + interval_s, events[0][ncf_s]);
            }
            return;
        }
        else if (msg == events[i][ncf_s])
        {
            // if (i == 0) {startTime = simTime(); endTime = startTime;}
            state[i] = ncf_in;
            std::vector<int> route = {no, 0};
            std::vector<int> data = {};
            data.push_back(0);
            data.push_back(upload_wait_s - ((int)round(simTime().dbl() + 2 * frm_slot_s) - networking_start_s));
            for (auto j : deltaT[i])
            {
                for (auto k : j.second)
                {
                    sons_nc[i][k] = j.first;
                    data.push_back(k);
                    /* ipv6 related */
                    if (ipv6_list.find(k) == ipv6_list.end())
                    {
                        ipv6_list[k] = {};
                        ipv6_list[k].push_back(ipv6_prefix[0]);
                        ipv6_list[k].push_back(ipv6_prefix[1]);
                        ipv6_list[k].push_back(0);
                        ipv6_list[k].push_back(k);
                    }
                }
            }
            frame *msg_buffer = generateMessage(NCF, DOWN, i, route, data);
            broadcast_freq(msg_buffer, i, 0);
            scheduleAt(simTime() + (data.size() + 2 - ncf_offset) * frm_slot_s, events[i][ncf_e]);
            return;
        }
        else if (msg == events[i][ncf_e])
        {   
            for (int j = 0; j < ncf_buffer[i].size(); j += 2 + 4)
            {
                /* ipv6 related */
                std::vector<int> ipv6 = ipv6_list[ncf_buffer[i][j]];
                if (ncf_buffer[i][j + 2] == ipv6[0] && ncf_buffer[i][j + 3]  == ipv6[1] && ncf_buffer[i][j + 4] == ipv6[2] && ncf_buffer[i][j + 5] == ipv6[3])
                {
                    sons_nc[i].erase(ncf_buffer[i][j]);
                    if (route_list.find(ncf_buffer[i][j]) == route_list.end())
                    {
                        sons.push_back(ncf_buffer[i][j]);
                        route_list[ncf_buffer[i][j]] = {};
                        if (now_son < 0)
                        {
                            route_list[ncf_buffer[i][j]].push_back(1);
                            route_list[ncf_buffer[i][j]].push_back(no);
                            route_list[ncf_buffer[i][j]].push_back(0x00000001 << i);
                        }
                        else
                        {
                            route_list[ncf_buffer[i][j]].push_back(route_list[sons[now_son]][0] + 1);
                            route_list[ncf_buffer[i][j]].push_back(sons[now_son]);
                            route_list[ncf_buffer[i][j]].push_back(ncf_buffer[i][j + 1]);
                        }
#ifdef debug0
                        emit(arrivalSignal, (long)(route_list[ncf_buffer[i][j]][0]));
#endif
                    }
                    else
                    {
                        if (now_son < 0)
                        {
                            route_list[ncf_buffer[i][j]][2] |= (0x00000001 << i);
                        }
                        else
                        {
                            route_list[ncf_buffer[i][j]].push_back(sons[now_son]);
                            route_list[ncf_buffer[i][j]].push_back(ncf_buffer[i][j + 1]);
                        }
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
                scheduleAt(simTime() + frm_slot_s + interval_s, events[0][pco_s]);
                EV << "sons: ";
                for (auto i : sons)
                {
                    EV << i << " ";
                }
                EV << endl;
            }
            debug(ncf_e, i);
            state[i] = ncf_out;
            ncf_buffer[i].clear();
            tqu_buffer.clear();
            sons_nc[i].clear();
            return;
        }
        else if (msg == events[i][pco_s] && sons.size() > 0)
        {
            now_son++;
            if (now_son >= sons.size() || sons.size() == par("max_stas").intValue())
            // if (now_son >= sons.size())
            {
                endTime = simTime();
#ifdef debug0
                emit(timeSignal, (endTime - startTime).dbl());
#endif
                EV << "Networking Finished, total number of STAs is: " << sons.size() << endl;
                EV << "sons"
                    << "(" << sons.size() << "): ";
                for (auto i : sons)
                {
                    EV << i << " ";
                }
                EV << endl;

                return;
            }
            for (int k = 0; k < FREQS; k++)
            {
                global_time[k] = 0;
            }
            std::vector<int> route = {sons[now_son]};
            std::vector<int> data = {};
            get_route(route);
            int j;
            for (j = 0; j < FREQS; j++)
            {
                if ((route_list[route[1]][2] >> j) % 2)
                {
                    EV << "PCO Freq: " << j << endl;
                    break;
                }
            }
            state[j] = pte_in;
            frame *msg_buffer = generateMessage(PTE_S, DOWN, j, route, data);
            broadcast_freq(msg_buffer, j, 0);
            tic[j] = simTime() + interval_s;
            scheduleAt(tic[j] + ((route.size() - 1) * 2 + 2) * frm_slot_s + (pte_slot_num + 2 + FREQS) * prm_slot_s, events[j][timeout]);
            EV << "PCOS: " << sons[now_son] << endl;
            return;
        }
        else if (msg == events[i][timeout])
        {
            if (state[i] == pte_in) {

            }
            EV << "TIMEOUT: " << sons[now_son] << " Status: " << state[i] << endl;
            scheduleAt(simTime() + interval_s, events[i][pco_s]);
            return;
        }
    }

    frame *frm_msg = check_and_cast<frame *>(msg);
    int type = frm_msg->getType();
    int dir = frm_msg->getDir();
    int freq = frm_msg->freq;
    int src = frm_msg->route[1];
    int dst = frm_msg->route[frm_msg->getHopCount()];
    EV << "SRC: " << frm_msg->getSource() <<", Type: " << type << ", Freq: " << freq << ", Dst: " << dst << ", Sent: " << frm_msg->route[2] << endl;
    if (dst != no && dst != 0)
    {
        return;
    }
    if (now_son >= 0 && dst == no)
    {
        EV << global_time[freq] << "--" << simTime() << endl;
        if (global_time[freq] == simTime())
        {
            EV << global_time[freq] << "--" << simTime() << endl;
            return;
        }
        else {
            global_time[freq] = simTime();
            EV << global_time[freq] << "--" << simTime() << endl;
        }
    }
    if (type == DAT && frm_msg->data[4] == 1)
    {
#ifdef debug0
        emit(uploadTimeSignal, (simTime() - upload_wait_s).dbl());
#endif
        return;
    }
    else if (type == BST && dir == DOWN)
    {
        max_wait_slot = 1;
        /* ipv6 related */
        ipv6_prefix[0] = frm_msg->data[1];
        ipv6_prefix[1] = frm_msg->data[2];
        // simtime_t delay = (double)ceil(uniform(0, 1) * max_wait_slot);
        simtime_t delay = 1;
        networking_start_s = int(ceil((long double)(simTime().dbl())));
        EV << max_wait_slot << endl;
        for (int i = 0; i < FREQS; i++)
        {
            scheduleAt(simTime() + delay, events[i][pte_s]);
            delay += prm_slot_s;
        }
        return;
    }
    else if (type == REQ && state[freq] == pte_in)
    {
        simtime_t tmp = simTime();
        toc[freq] = tmp;
        // if (tmp - toc[freq] >= prm_slot_s)
        // {
        //     toc[freq] = tmp;
        // }
        // else
        // {
        //     emit(collisionSignal, (long)(1));
        //     toc[freq] = tmp;
        //     collision_times[freq]++;
        //     // return;
        // }
        // if (tmp - tic[freq] >= prm_slot_s)
        // {
        //     int delta_t = (int)round(((tmp - tic[freq]) / prm_slot_s).dbl());
        //     if (deltaT[freq].find(delta_t) == deltaT[freq].end())
        //     {
        //         deltaT[freq][delta_t] = {};
        //     }
        //     else
        //     {
        //         emit(collisionSignal, (long)(1));
        //     }
        //     return;
        // }
        // int delta_t = (int)round(((float)((tmp.dbl() - tic[freq].dbl())) / prm_slot_s));
        int delta_t = (int)round((long double)(tmp.dbl() - tic[freq].dbl()) / prm_slot_s);
        
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
    else if (type == TQU && dir == UP && src == no && state[freq] == tqu_in)
    {
        // EV << "SRC: " << frm_msg->getSource() <<", Type: " << type << ", Freq: " << freq << " (CCO " << no << ")" << frm_msg->route[frm_msg->getHopCount()] << endl;
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
        EV << "SRC: " << frm_msg->getSource() << ", Type: " << type << ", Freq: " << freq << " (CCO " << no << ")" << frm_msg->route[frm_msg->getHopCount()] << endl;
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            ncf_buffer[freq].push_back(frm_msg->data[i]);
        }
    }
    else if (type == PTE_S && dir == UP && src == no && dst == no && state[freq] == pte_in)
    {
        cancelEvent(events[freq][timeout]);
        int total_slots = 0;
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            total_slots += frm_msg->data[i] + 1;
        }
        if (total_slots == frm_msg->data[0])
        {
            scheduleAt(simTime() + interval_s, events[freq][pco_s]);
        }
        else
        {
            state[freq] = tqu_in;
            std::vector<int> data = {total_slots};
            std::vector<int> route = {sons[now_son]};
            get_route(route);
            frame *msg_buffer = generateMessage(TQU_S, DOWN, freq, route, data);
            broadcast_freq(msg_buffer, freq, interval_s);
            tic[freq] = simTime() + interval_s;
            scheduleAt(tic[freq] + ((route.size() - 1) * 2 + 2 + total_slots + FREQS * 2) * (frm_slot_s + interval_s), events[freq][timeout]);
            EV << "PTES UP: " << sons[now_son] << " total slots: " << total_slots << endl;
        }
    }
    else if (type == TQU_S && dir == UP && src == no && dst == no && state[freq] == tqu_in)
    {
        cancelEvent(events[freq][timeout]);
        std::vector<int> data = {};
        int total_slots = FREQS * 2;
        for (int i = 1; i <= frm_msg->data[0]; i++)
        {
            data.push_back(frm_msg->data[i]);
            if (i % 3 == 1)
                total_slots += frm_msg->data[i];
        }
        if (data.size() == 0)
        {
            scheduleAt(simTime() + interval_s, events[freq][pco_s]);
        }
        else
        {
            state[freq] = ncf_in;
            std::vector<int> route = {sons[now_son]};
            get_route(route);
            frame *msg_buffer = generateMessage(NCF_S, DOWN, freq, route, data);
            broadcast_freq(msg_buffer, freq, interval_s);
            tic[freq] = simTime() + interval_s;
            scheduleAt(tic[freq] + ((route.size() - 1) * 2 + 2 + total_slots + 2 * FREQS) * (frm_slot_s + interval_s), events[freq][timeout]);
        }
        EV << "TQUS UP: " << sons[now_son] << endl;
    }
    else if (type == NCF_S && dir == UP && src == no && dst == no && state[freq] == ncf_in)
    {
        cancelEvent(events[freq][timeout]);
        for (int i = 1; i <= frm_msg->data[0]; i += 2)
        {
            if (route_list.find(frm_msg->data[i]) == route_list.end())
            {
                sons.push_back(frm_msg->data[i]);
                route_list[frm_msg->data[i]] = {};
                route_list[frm_msg->data[i]].push_back(route_list[sons[now_son]][0] + 1);
                /* ipv6 related */
                if (ipv6_list.find(frm_msg->data[i]) == ipv6_list.end())
                {
                    ipv6_list[frm_msg->data[i]] = {};
                    ipv6_list[frm_msg->data[i]].push_back(ipv6_prefix[0]);
                    ipv6_list[frm_msg->data[i]].push_back(ipv6_prefix[1]);
                    ipv6_list[frm_msg->data[i]].push_back(0);
                    ipv6_list[frm_msg->data[i]].push_back(frm_msg->data[i]);
                }
#ifdef debug0
                emit(arrivalSignal, (long)(route_list[frm_msg->data[i]][0]));
#endif
            }
            route_list[frm_msg->data[i]].push_back(sons[now_son]);
            route_list[frm_msg->data[i]].push_back(frm_msg->data[i + 0]);
        }
        EV << "NCFS UP: " << sons[now_son] << endl;
        EV << "sons" << "(" << sons.size() <<"): ";
        for (auto i : sons)
        {
            EV << i << " ";
        }
        EV << endl;
        if (now_son + 1 < sons.size())
        {
            scheduleAt(simTime() + interval_s, events[freq][pco_s]);
        }
        else
        {
            EV << "Networking Finished, total number of STAs is: " << sons.size() << endl;
        }
    }
}

void CCO::forwardMessage(frame *msg)
{
    // Increment hop count.
}

void CCO::broadcast_freq(frame *msg, int freq, simtime_t delay)
{
    char frm_name[20];
    sprintf(frm_name, "frm_f%d", freq);
    int size = gateSize(frm_name);
    sprintf(frm_name, "frm_f%d$o", freq);

    for (int i = 0; i < size; i++)
    {
        sendDelayed((i == 0) ? msg : msg->dup(), delay, frm_name, i);
    }
}

frame *CCO::generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data)
{
    char discriptor[100];
    int route_len = route.size();
    int data_len = data.size();
    int src = route[0];
    int dst = route[route_len - 1];
    sprintf(discriptor, "{Src: %d, Dst: %d, Type: %d, Dir: %d, Freq: %d, Route_len: %d, Data_len: %d}", src, dst, type, dir, freq, route_len, data_len);
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
    msg->setHopCount((dir == DOWN) ? 2 : route.size() - 1);
    return msg;
}
