**并行节点数组组合算法 —— 基于点对点通讯的数据收集方案**

**Array Assembly from Parallel Nodes – A Point‑to‑Point Communication Scheme**

**1. 问题背景 | Background**

在Ansys Fluent的UDF环境中，每个计算节点（compute_node）均拥有自己的独立内存空间，而现有的UDF函数虽然支持广播与同步，但没有直接提供将各个并行节点上大小不等的数组无缝组合成一个连续全局数组的功能。因此，当用户需要在主节点（myid == 0）上汇总所有节点的局部数组（例如将每个节点的特征数据拼接成一个完整的训练集）时，必须自行设计通讯算法。

In the Ansys Fluent UDF environment, each compute node has its own private memory. The existing UDF functions (e.g., PRF_CRECV_REAL_ALL) support broadcast and synchronization, but there is no built-in function to directly concatenate arrays of possibly different sizes from all parallel nodes into one contiguous global array. Therefore, when the user needs to collect local arrays from all nodes on the master node (e.g., to form a complete training dataset), a custom communication algorithm must be designed.

**2. 提出的算法 | Proposed Algorithm**

为了克服上述限制，我设计了一个点对点（Point-to-Point, P2P）通讯算法。其核心思想是：
每个节点先构建自己的本地数组（仅保留前N_valid个有效元素）。
主节点依次向每个从节点发送一个握手信号token，然后接收从节点回传的有效元素个数及实际数据，并追加到全局数组的末尾。这仅仅需要一个if条件判断即可实现。
从节点在收到握手信号后，立即发送自己的有效元素个数和数据。
通过PRF_GSYNC保证所有节点在通讯前已完成本地数据构造。
该算法实现简单，无需依赖MPI集合操作，且能灵活处理每个节点有效数据个数不同的情形（只需修改data_sum即可）。

To overcome this limitation, I design a point-to-point (P2P) communication algorithm. Its key ideas are:
Each compute node constructs its local array (only the first N_valid valid entries are kept).
The master node sends a handshake token to each slave node in turn, then receives the number of valid entries and the actual data from that slave, appending the data to the end of a global buffer. This can be achieved with just an if condition.
A slave node, upon receiving the handshake, immediately sends back its valid count and array.
A PRF_GSYNC barrier ensures all nodes finish data construction before communication starts.
The algorithm is simple to implement, does not rely on MPI collective routines, and can easily handle different numbers of valid entries per node (just modify data_sum).

**3. 算法优势与局限 | Advantages & Limitations**

优势：所有通讯均基于Fluent原生PRF_CSEND/PRF_CRECV宏，兼容性强；且通过握手信号实现可靠的数据交换，不存在死锁风险。理论上讲这个算法不会随着Fluent的版本更迭而发生较大改变。
局限：主节点以串行方式收集数据，当节点数很大（如>100）时，通讯延迟会线性增长。

Advantages: All communication uses Fluent’s native PRF_CSEND or PRF_CRECV macros for full compatibility. The handshake mechanism guarantees reliable exchange without deadlock. Theoretically, this algorithm will not change much with the updates of Fluent.
Limitations: The master node collects data sequentially; as the number of nodes grows (e.g., >100), communication latency increases linearly.

**4. 使用说明 | Usage Notes**

本测试代码以DEFINE_ON_DEMAND宏实现，可通过Fluent的Execute on Demand手动调用。
默认每个节点有15个有效数据（data_sum=15）；若要支持变长数据，只需在通讯前让各节点自行计算data_sum值，并在global数组的偏移计算中依赖该值。
打印输出用于验证，实际使用时可将 global 数组用于后续计算或输出文件。

The test code is implemented as a DEFINE_ON_DEMAND macro, triggerable via Fluent’s Execute on Demand.
By default each node provides 15 valid entries (data_sum=15); for variable‑length data, let each node compute its own data_sum, and rely on the received recv_num to manage offsets.
The Message0 output is for verification; in production, the global array can be further processed or written to files.
