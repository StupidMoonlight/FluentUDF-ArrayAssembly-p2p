#include "udf.h"
#define DATA_SIZE 20

DEFINE_ON_DEMAND(collect_all_data)
{
#if RP_NODE
    // ---------- 1. Each compute node constructs its own local data ----------
    real* local = (real*)malloc(DATA_SIZE * sizeof(real));
    int data_sum = 15;                           // number of valid entries (first 15)
    for (int i = 0; i < DATA_SIZE; i++) 
    {
        // Each node uses `myid` to uniquely label data
        local[i] = myid * 100.0 + i + 1;        // e.g., node 0: 1..15, node 1: 101..115, etc.
        if (i > data_sum)
            local[i] = 0;                        // zero out extra positions
    }

    PRF_GSYNC();   // global barrier: ensure all nodes finish before communication

    // ---------- 2. Master node (myid==0) gathers data from all nodes ----------
    if (myid == 0)
    {
        // allocate a large buffer (size = number_of_nodes * DATA_SIZE)
        real* global = (real*)malloc(compute_node_count * DATA_SIZE * sizeof(real));

        // copy own valid data to the beginning of global buffer
        for (int i = 0; i < data_sum; i++)
            global[i] = local[i];

        int recv_num;
        int offset = data_sum;                  // current writing position in global

        // collect data from each other node sequentially
        for (int node = 1; node < compute_node_count; node++)
        {
            int token = node;                   // any integer as a handshake signal
            PRF_CSEND_INT(node, &token, 1, 0);  // send token to slave (tag=0)

            PRF_CRECV_INT(node, &recv_num, 1, node);  // receive number of entries (tag=node)
            PRF_CRECV_REAL(node, &global[offset], recv_num, node); // receive real data (tag=node)

            offset += recv_num;                 // move write pointer
        }

        // print the entire gathered data (for verification)
        for (int i = 0; i < compute_node_count * DATA_SIZE; i++)
            Message0("global_host[%d] = %g\n", i, global[i]);

        free(global);
        Message0("total = %d\n", offset);       // total number of valid entries
    }
    // ---------- 3. Slave nodes respond to master's requests ----------
    else
    {
        int token;
        PRF_CRECV_INT(0, &token, 1, 0);         // wait for handshake from master (tag=0)
        PRF_CSEND_INT(0, &data_sum, 1, myid);   // send own valid count (tag=myid)
        PRF_CSEND_REAL(0, local, data_sum, myid); // send own valid data (tag=myid)
    }

    free(local);   // release local memory
#endif
}
