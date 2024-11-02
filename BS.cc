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

#include "frame_m.h"

using namespace omnetpp;

/**
 * Derive the Txc1 class from cSimpleModule. In the Tictoc1 network,
 * both the `tic' and `toc' modules are Txc1 objects, created by OMNeT++
 * at the beginning of the simulation.
 */
class BS : public cSimpleModule
{
public:
    int no;
    int FREQS;
    int mac;
    int cco_num;
    /* ipv6 related */
    int ipv6_prefix[2];
    std::vector<std::vector<int>> cco_freqs;
    frame *msg_buffer;
    void broadcast_bs(frame *msg);

protected:
    // The following redefined virtual function holds the algorithm.
    virtual frame *generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data);
    virtual void forwardMessage(frame *msg);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(BS);

void BS::initialize()
{
    // Initialize is called at the beginning of the simulation.
    // To bootstrap the tic-toc-tic-toc process, one of the modules needs
    // to send the first message. Let this be `tic'.

    FREQS = par("freq_num").intValue();
    no = -1;
    mac = -1;
    cco_num = 1;
    /* ipv6 related */
    ipv6_prefix[0] = 0;
    ipv6_prefix[1] = 0;
    for (int i = 0; i < cco_num; i++)
    {
        std::vector<int> tmp = {};
        for (int j = 0; j < FREQS; j++)
        {
            tmp.push_back(j);
        }
        cco_freqs.push_back(tmp);
    }

    std::vector<int> route = {no, 0};
    std::vector<int> data = {};
    /* ipv6 related */
    data.push_back(ipv6_prefix[0]);
    data.push_back(ipv6_prefix[1]);
    for (int k = 0; k < FREQS; k++)
    {
        std::map<int, bool> tmp;
        for (int i = 0; i < cco_num; i++)
        {
            for (int j = 0; j < cco_freqs[i].size(); j++)
            {
                if (tmp.find(cco_freqs[i][j]) == tmp.end())
                {
                    tmp[cco_freqs[i][j]] = 1;
                    data.push_back(cco_freqs[i][j]);
                    cco_freqs[i].erase(cco_freqs[i].begin() + j);
                    break;
                }
            }
        }
    }
    msg_buffer = generateMessage(BST, DOWN, 0, route, data);
    broadcast_bs(msg_buffer);
}

void BS::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module. Here, we just send it to the other module, through
    // gate `out'. Because both `tic' and `toc' does the same, the message
    // will bounce between the two.
    frame *frm_msg = check_and_cast<frame *>(msg);
    int type = frm_msg->getType();
    int dir = frm_msg->getDir();
    //    EV << "SRC: " << frm_msg->getSource() <<", Type: " << type << ", Freq: " << freq << " (BS)" << endl;

    if (dir == UP && type == BST)
    {
        std::vector<int> route = {no, 0};
        std::vector<int> data = {0};
        delete msg_buffer;
        msg_buffer = nullptr;
        msg_buffer = generateMessage(TQU, DOWN, 0, route, data);
        broadcast_bs(msg_buffer);
    }
}

void BS::forwardMessage(frame *msg)
{
    // Increment hop count.
}

void BS::broadcast_bs(frame *msg)
{
    int size = gateSize("bs_port");

    for (int i = 0; i < size; i++)
    {
        send(i == 0 ? msg : msg->dup(), "bs_port$o", i);
    }
}

frame *BS::generateMessage(int type, int dir, int freq, std::vector<int> &route, std::vector<int> &data)
{
    char discriptor[100];
    int route_len = route.size();
    int data_len = data.size();
    int src = this->no;
    int dst = route[route_len - 1];
    sprintf(discriptor, "{Src: %d, Dst: %d, Type: %d, Dir: %d, Freq: %d, Route_len: %d, Data_len: %d}", src, dst, type, dir, freq, route_len, data_len);
    frame *msg = new frame(discriptor);
    msg->setDestination(dst);
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
    msg->setHopCount(2);
    return msg;
}
