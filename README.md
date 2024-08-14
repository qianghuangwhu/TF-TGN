# TF-TGN
The codebase for TF-TGN, a temporal graph neural network (TGNN) based on the **Transformer decoder** to model the evolution of temporal graphs.

- **TF-TGN structures the message aggregation operation between chronologically occurring nodes and their temporal neighbors in TGNNs as sequence modeling.**
- **TF-TGN unified the traning paradigms of TGNNs as as sequence modeling and fill the gap between TGNNs (or even GNNs) and LLMs.**

## Sampling
This  code is the implementation of the parallel sampling algorithm in the paper to enhance the efficiency of T-CSR converting and  temporal neighbor sampling. 
### compile
```bash
g++ -fopenmp -std=c++11 main.cpp TemporalGraph.cpp readcsv.cpp  utils.cpp -o main
```
`string file_path` in the `main.cpp` is the path of the dataset. For example, `string file_path = "./reddit.csv";` `reddit.csv` is the Reddit dataset. 

### run
For example, `sample_num=128, batch_size=512`
```bash
./main 128 512
```

## TF-TGN 
The code of the TF-TGN model  will be released once we obtain open-source licenses from our collaborators and partner organizations.

TF-TGN incorporates the the following components:
- Flash-attention
- Memeory-efficient attention
- PyTorch FDSP ZeRO-2
- Mixed precision training