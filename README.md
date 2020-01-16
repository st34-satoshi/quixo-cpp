# quixo-cpp
solve real quixo 

## find reachable states
- `c++ -std=c++17 reachable_state.cpp`
- `./a.out`

## compute all states value from back
### prepare
windows may need to modify the fileName function in state.cpp (line 435). 
### Run
- `c++ -std=c++17 -Wall -O3 from_back.cpp `
- `./a.out`

results are saved in results directory.