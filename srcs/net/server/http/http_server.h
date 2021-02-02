#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "srcs/net/types.h"
#include "srcs/utils/uncopyable.h"
#include "srcs/net/server/tcp_server.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_response.h"

#include <unordered_map>
#include <list>
#include <string>
#include <functional>
#include <mutex>
//#include <atomic>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>


namespace net {

class SpinLock {
private:
  std::atomic_flag lck = ATOMIC_FLAG_INIT;
public:
  void lock() { while(lck.test_and_set(std::memory_order_acquire)) {} }
  void unlock() { lck.clear(std::memory_order_release); }
};


template<typename BufferType>
class HTTPServer : private Uncopyable {
public:
  using ResponseCallback = std::function<void(const HTTPRequestPtr&, HTTPResponsePtr&)>;
  HTTPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_address);

//  void setResponseCallback(ResponseCallback cb) { responseCallback_ = std::move(cb); };
  void setResponseCallback(const std::string& path, ResponseCallback cb) {
    responseCallbacks_[path] = std::move(cb);
  }

  void start() { server_.start(); }
private:
  TCPServer<BufferType> server_;
  HTTPResponsePtr bad_request_resp_;
  HTTPResponsePtr not_found_resp_;

//  ResponseCallback responseCallback_;
  std::unordered_map<std::string, ResponseCallback> responseCallbacks_;

  static bool kUseRespCache;
  using CacheNode = struct {
    HTTPRequestPtr req;
    HTTPResponsePtr resp;
  };
  using CacheNodeList = std::list<CacheNode>;
  CacheNodeList cache_node_list_;
  std::unordered_map<
    HTTPRequestPtr, typename CacheNodeList::iterator,
    HTTPRequstPtrHash, HTTPRequstPtrEqualTo> req_to_cache_node_iter_;

  static const int kMaxReqCached;
//  mutable std::mutex mutex_g_;
  mutable SpinLock spin_lock_;
//  mutable boost::shared_mutex mutex_g_;
  void addRespCache(const HTTPRequestPtr& req, const HTTPResponsePtr& resp);
  bool getRespCache(const HTTPRequestPtr& req, HTTPResponsePtr& resp);

  void connectionOnCreate(const TCPConnectionPtr<BufferType>& conn);

  void connectionOnRead(BufferType& in_buffer, const TCPConnectionPtr<BufferType>& conn);
};

} // namespace net

#endif // HTTP_SERVER_H
