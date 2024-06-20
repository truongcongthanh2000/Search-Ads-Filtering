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
- [Algorithm](#algorithm)
    - [Filtering with single thread](#filtering-with-single-thread)
    - [Filtering with multithreading](#filtering-with-multithreading)
- [Benchmark](#benchmark)
- [Thanks to](#thanks-to)


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

## Algorithm
### Filtering with single thread
First, which also crucial thing to my implementation for filtering, is the definition of define keyword can be match with the search query. From what I see logic matching (ex: [Google Ads](https://support.google.com/google-ads/answer/7478529)), the keyword cannot be match with the search query if keyword contains any token, *which include a collection of 1 word or more words to create a meaningful phrase*, **that not belong to the search query**. For example:
- keyword = `ghế massage` can match with query = `ghế massage giá tốt nhất` because tokenize(`ghế massage`) = `{ghế, massage}` $\subset$ tokenize(`ghế massage giá tốt nhất`) = {`ghế, massage, giá, tốt nhất`}
- keyword = `iphone 15` cannot match with query = `giá samsung s24 ultra` because `iphone` or `15` does not belong in query

In indexing step, for each keyword, I create a `map<token, list<keywords>>` to list all keywords that contains the token. When serve api with search query, list all tokens of the query and use map in indexing to counting all keywords. After all, I will list all keyword that all tokens of the keyword has been counted. Here is the pseucode 
```
n keywords:
keyword[i] -> {token_1, token_2, .., token_m} -> 
    map_token_keywords[token_1] = [keyword[i] ... ]
    map_token_keywords[token_2] = [keyword[i] ... ]
    map_keyword_tokens_size[keyword[i]] = m

query_user -> query_tokens = {token_1, token_2, ..., token_t}
Filter(map_token_keywords, map_keyword_tokens_size, query_tokens): list[keyword]
    count = map[keyword][int] // store upto n keywords
    for token in query_tokens:
        for keyword in map_token_keywords[token]:
            count[keyword] += 1
    ans = []
    for keyword, cnt in count:
        if cnt == map_keyword_tokens_size[keyword]:
            ans = append(ans, keyword)
    return ans
Time complexity: O(n * t) in the worst case
Space complexity: O(n)
```
**Optimize 1:** The above code has cons in storing huge keywords, which can be all keywords in the worst case. So I divide all n keywords into sqrt(n) blocks, each containing sqrt(n) keywords. When traversing each block, I use [Sliding window technique](https://www.geeksforgeeks.org/window-sliding-technique/) for each token in the query to keep track of the current block. So I just need to use a counting array with size sqrt(n) to count the occurrence for each keyword.

Time complexity: O($\frac{n}{sqrt(n)} \times (t \times sqrt(n))$) = O(n * t) in the average case

Space complexity: O($sqrt(n)$)
### Filtering with multithreading
**Optimize 2:** As you can see that, the keywords are completely independent of each other, so I applied a [Square Root (Sqrt) Decomposition Algorithm](https://www.geeksforgeeks.org/square-root-sqrt-decomposition-algorithm/) in optimize 1. In addition, we can also apply parallel programming. Here I use a threadpool with the number of threads being 4 and the number of tasks being 100, with each task handling about $\frac{n}{100}$ keywords with code according to optimize 1.

## Benchmark
TBU

## Thanks to
It really hard to has this repo without contribution from public repo, I specially thanks to:
- [C++ HttpServer by nguyenhien1994](https://github.com/nguyenhien1994/simple-http-server)
- [Data keywords and queries by greeneley in Vietnamese Search Engine](https://github.com/greeneley/Vietnamese_Search_Engine)
