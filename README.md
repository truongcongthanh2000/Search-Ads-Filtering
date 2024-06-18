# Search Ads Filtering With Multithreading
In Search Ads, there are many systems you may know such as Google Ads, CocCoc ads, .... Each system will have different components. But for me, it should has 4 main parts
- Indexing: Load everything such as keywords, adverts, campaign ... in the system before serve any request to show ads based on the query of User.
- Matching: Focus on how the keyword can match with the search query
- Ranking: Sort the list matching keywords through strategy help to improve quality of ads
- Pricing: Use auction price model to charge for ads to be displayed.

For a large system like CocCoc, which has 30 milions of Users, or Google, which has a billion of Users. It will lead to the number of keywords really huge, we cannot take all that keywords to check matching with the search query. That is why we have a component filtering before matching to list all keyword can match the query.

Filtering component just remove some keyword that we sure it cannot match with the search query within a reasonable time. The fewer keywords that have to be checked to match the search query, the lower the latency. 

This repo is the implementation of a simple filtering and indexing component in Search Ads system with multithreading in C++.

# Table of Contents
- [Features](#features)
- [Build code](#build-code)
- [Usage](#usage)
- [Benchmark](#benchmark)

## Features
- Run on Linux only
- Indexing load the number keywords is 10M and it will load each 30 seconds once. *The original data I get from [Vietnamese Search Engine](https://github.com/greeneley/Vietnamese_Search_Engine/tree/master/data) contains upto 840k, so I expand some random keywords from original data until 10M*
- More details about [server here](https://github.com/nguyenhien1994/simple-http-server?tab=readme-ov-file#features)
- The number of queries used to benchmark is 34k
- The number of threads used in filtering is 10, and the number of tasks is 100

## Build code
To run this server, follow these steps:

1. Clone this repository: `git clone git@github.com:truongcongthanh2000/Search-Ads-Filtering.git`
2. Navigate to the repository directory: `cd Search-Ads-Filtering`
3. Build the source code:
```
mkdir build
cd build
cmake .. && make
```
## Usage
First, move file keywords.txt from data directory: `cp ../data/keywords.txt .` to the build directory.

And then, to start the server, run `./apiGateway`

The server will start on port 8080 by default.

Access the demo API via: `localhost:8080/filter/?query=....`, just copy any query in `data/requests.txt` and paste it into `?query=....`

## Benchmark
TBU

## Algorithm


## Thanks to
It really hard to has this repo without contribution from public repo, I specially thanks to:
- [C++ HttpServer by nguyenhien1994](https://github.com/nguyenhien1994/simple-http-server)
- [Data keywords and queries by greeneley in Vietnamese Search Engine](https://github.com/greeneley/Vietnamese_Search_Engine)

