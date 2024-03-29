# Solving Quixo
## About Quixo
see [Gigamic Quixo](https://en.gigamic.com/game/quixo)

## Algorith
see [Quixo Is Solved](https://arxiv.org/abs/2007.15895) or [Quixoの強解決](https://ipsj.ixsq.nii.ac.jp/ej/index.php?active_action=repository_view_main_item_detail&page_id=13&block_id=8&item_id=207673&item_no=1)

## Code
We have two types of codes which solves quixo. One is in this directory. 

The other one is written by [François Bonnet](https://github.com/Fanfoue). If you want to see the other one see [here](./others/multi-fb/codeFrancois_v7/main.cpp).

### Result
See [Quixo Is Solved](https://arxiv.org/abs/2007.15895).

The total number of Win, Loss, and Draw states is written in [here](./python/result-5by5-activeIsO.txt)

<hr>

## compute all states value from back
You can change board size in global.cpp.  
### time  

- size 4: 50 seconds
- size 3: soon
- size 2: soon


### Run
- `g++ -std=c++17 -Wall -O3 from_back.cpp `
- `./a.out`

results are saved in results directory.

## find optimal strategy
Compute the step to the end. The winner choose min, the loser choose max.  
You need to run `from_back.cpp` at first with the same board size.

### time
- size 4: 150 seconds
- size 3: soon
- size 2: soon

### Run
- `g++ -std=c++17 -Wall -O3 search-step.cpp `
- `./a.out`

results are saved in results directory.

## see the results
You need to run `search-step.cpp` at first with the same board size.  
If you want to see all states result, change the comment out of the file `convertResult.cpp`.

### Run

- `g++ -std=c++17 -Wall -O3 convertResult.cpp  `
- `./a.out`

## check the result
You can check the result using the different algorithm.  
(But I think the checking algorithm is not perfect)  
You need to run `search-step.cpp` at first with the same board size.  

### Run
- `g++ -std=c++17 -Wall -O3 check-result.cpp `
- `./a.out`

<hr> 

## find reachable states

- `g++ -std=c++17 -O3 -Wall reachable_state.cpp`
- `./a.out`

### result
you can see the result using the function `outputReachableStates` or `countNotReachableStates` in `convertResult.cpp`
