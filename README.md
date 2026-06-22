# FluentUDF-ArrayAssembly-p2p
A point-to-point communication algorithm for gathering arrays from parallel nodes in Ansys Fluent UDF. Suitable for combining local data into Node0. Supports variable-length data per node.
> **English** | [中文](#中文)
## English
**A point-to-point communication algorithm for gathering arrays from parallel nodes in Ansys Fluent UDF.**  
Suitable for combining local training data (e.g., CFD arrays) into a global buffer. Supports variable-length data per node.
### Requirements
- Ansys Fluent **2021 R1** (The version tested by the author, but theoretically applicable to all versions.)
### How to Use
1. Compile with Fluent's built-in UDF compiler or command line.
2. Load the library in Fluent and assign the UDF to the appropriate hook.
3. Please modify the code based on the actual case; this work only provides test code.
### Author
- [StupidMoonlight](https://github.com/StupidMoonlight)  
Feel free to open issues or pull requests. Please cite our work if you use this code in academic publications. For commercial use, we kindly request that you contact us for notification (no cost required).
### License
This project is distributed under the MIT License. See `LICENSE` for details.
---
## 中文
**一种用于在 Ansys Fluent UDF 中通过点对点通信收集并行节点数组的算法。**  
适用于将局部 CFD 数据（如网格变量、训练数据）组合成全局缓存。支持每个节点发送可变长度数据。
### 环境要求
- Ansys Fluent **2021 R1**（作者测试的版本，但理论上各个版本都可使用）
### 使用方法
1. 使用 Fluent 内置编译器或命令行进行编译。
2. 在 Fluent 中加载库，并将 UDF 分配给相应钩子。
3. 请结合实际算例对代码进行修改，本文只提供测试代码。
### 作者
- [StupidMoonlight](https://github.com/StupidMoonlight)  
欢迎提交 Issue 或 Pull Request。
### 许可证
本项目采用 MIT 许可证，详情参见 `LICENSE` 文件。如您在学术出版物中使用此代码，请引用我们的工作。如需商业用途，请联系我们以获取通知（无需费用）。
