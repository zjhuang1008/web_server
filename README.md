# WebServer

This is a c++ web server followed the design of 
[muduo](http://github.com/chenshuo/muduo). 
I write this repo to help me to get familiar with modern c++, 
linux system calls, etc.

## Usage

First, clone the project into `${PROJECT_ROOT}`, then build it by

```shell script
mkdir build
cd build
cmake ..
make -j
```
After that, the executable binary would lie in `${PROJECT_ROOT}/build/bin`. 
We could run the tests in that folder. For example 

```shell script
sh ${PROJECT_ROOT}/build/bin/test_server
``` 

## Options

### Set `MIN_LOG_LEVEL` to control log level
Logger level can be changed by the environment variables `MIN_LOG_LEVEL`.
`MIN_LOG_LEVEL` from 0 to 4 indicate DEBUG, INFO, WARN, ERROR, FATAL. 
For example, to set MIN_LOG_LEVEL as DEBUG, we can type this
```shell script
export MIN_LOG_LEVEL=0
```

### set `USE_RESP_CACHE` to use cache on request
If you want http_response to be cached for the same http_request, set this:
```shell script
export USE_RESP_CACHE=1
```
It will be useful when the request needs long time to handle.

## experiments

### environment:
- OS: Ubuntu 18.04
- CPU: i7-8700 CPU, 12 logical cpus, 6 cores
- memory: 16GB

### tool for benchmarking 
The tool used is based on the open source benchmark tool [webbench](http://home.tiscali.cz/~cz210552/webbench.html).
I make some changes to benchmark more metrics (e.g. average time to connect, average time to response). 
The code is in `tests/webbench/webbench.c`. For all experiments, run it by 
```shell script
./webbench -t 60 -c 1000 -2 --get http://127.0.0.1:45678/long
```
It will try to connect to the url with 1000 clients (processes) for 60s.
The benchmark metrics used are:
- `#conn`: the number of successful connections per minutes
- `#bytes`: the number of bytes received from the connection per seconds
- `conn_t`: the average time to connect (ms)
- `resp_t`: the average time to first response (ms)

### comparison

For fair comparison, the test methods for each experiments are the same: 
- uses 1 listen thread and 4 I/O threads
- disable log
- run in local, and connect from local
- the response is "text/plain" with 5000 'a' inside. 

server | `#conn` | `#bytes` | `conn_t` | `resp_t` |
--- | --- | --- | --- | --- |
[muduo](http://github.com/chenshuo/muduo) | 3304199 | 278873376 | 8.44969 | 4.23476 |
my | 4283414 | 361519808 | 9.3398 | 3.50479 |
my (cache with mutex) | 3336020 | 281560096 | 0.949731 | 16.9795 |
my (cache with spinlock) | 3718228 | 313818464 | 1.58277 | 14.4947 |

We can tell that cache doesn't improve the performance in the light weight job, 
because it takes little time to prepare the content of the response. To show the
effects of the cache, I simulate a delay by sleeping 20ms. The results are:

server | `#conn` | `#bytes` | `conn_t` | `resp_t` |
--- | --- | --- | --- | --- |
my | 11785 | 994654 | 110.124 | 4765.27 |
my (cache with spinlock) | 3732111 | 314990080 | 2.28278 | 13.7384 |

From that, we can see that cache brings great efficiency when the task takes time.