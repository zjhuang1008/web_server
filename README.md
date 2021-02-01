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

## Set Minimal Logger Level to reduce log

Logger level can be changed by the environment variables `MIN_LOG_LEVEL`.

`MIN_LOG_LEVEL` from 0 to 4 indicate DEBUG, INFO, WARN, ERROR, FATAL. 

For example, to set MIN_LOG_LEVEL as DEBUG, we can type this
```shell script
export MIN_LOG_LEVEL=0
```

## compile options

set USE_RESP_CACHE=ON to cache the http_request, such that 
http_response will be cached for the same http_request.   
