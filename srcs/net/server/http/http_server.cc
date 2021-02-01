#include "srcs/net/server/http/http_server.h"

#include <boost/any.hpp>

#include "srcs/net/connection/buffer/buffer_factory.h"
#include "srcs/net/connection/tcp_connection.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/server/http/http_response.h"
#include "srcs/net/server/http/http_context.h"
#include "srcs/logger/logger.h"

using namespace net;

template<typename BufferType>
const int HTTPServer<BufferType>::kMaxReqCached = 1024;

template<typename BufferType>
HTTPServer<BufferType>::HTTPServer(EventLoopPtr& loop, size_t num_io_threads, SocketAddress host_address)
  : server_(loop, num_io_threads, host_address),
    bad_request_resp_(std::make_shared<HTTPResponse>()),
    not_found_resp_(std::make_shared<HTTPResponse>()) {
  server_.setConnectionCreateCallback([this](const TCPConnectionPtr<BufferType>& conn) {
    this->connectionOnCreate(conn);
  });
  server_.setConnectionReadCallback([this](BufferType& in_buffer, const TCPConnectionPtr<BufferType>& conn) {
    this->connectionOnRead(in_buffer, conn);
  });

  bad_request_resp_->setStatusCode(HttpStatusCode::k400BadRequest);
  bad_request_resp_->setCloseConnection(true);

  not_found_resp_->setStatusCode(HttpStatusCode::k404NotFound);
  not_found_resp_->setCloseConnection(true);
  #ifdef USE_RESP_CACHE
  LOG(DEBUG) << "use resp_cache";
  #endif
}

template<typename BufferType>
void HTTPServer<BufferType>::connectionOnCreate(const TCPConnectionPtr<BufferType>& conn) {
  conn->setContext(HTTPContext());
}

template<typename BufferType>
void HTTPServer<BufferType>::connectionOnRead(BufferType& in_buffer, const TCPConnectionPtr<BufferType>& conn) {
  HTTPResponsePtr response;
  HTTPContext* http_context = boost::any_cast<HTTPContext>(conn->getMutableContext());

  auto&& request = http_context->request();

  bool to_send = false;
  if (!http_context->parseRequest(in_buffer)) {
    // bad request
    to_send = true;

    response = bad_request_resp_;
  }

  if (http_context->parseFinished()) {
    http_context->resetState();
    to_send = true;

    const std::string& path = request->getPath();
    LOG(DEBUG) << "connection " << conn->name() << " receive " << path;

    if (responseCallbacks_.count(path)) {
      // request can be handled
      #ifdef USE_RESP_CACHE
      {
//        std::lock_guard<std::mutex> lk(mutex_g_);
        std::lock_guard<SpinLock> lk(spin_lock_);
        if (!getRespCache(request, response)) {
          response = std::make_shared<HTTPResponse>();
          responseCallbacks_[path](request, response);
          addRespCache(request, response);
        }
      }
//      LOG(ERROR) << "cache value: " << request->to_hash_value();
//      LOG(ERROR) << "size of cache_node_list: " << cache_node_list_.size();
      #else
        response = std::make_shared<HTTPResponse>();
        responseCallbacks_[path](request, response);
      #endif

      // decide whether to close the connection
      if (request->hasHeader("Connection")) {
        const std::string& conn_value = request->getHeader("Connection");
        bool to_close = conn_value == "close" ||
                        (request->getVersion() == HttpVersion::kHttp10 && conn_value == "Keep-Alive");
        response->setCloseConnection(to_close);
      }
    } else {
      // not found
      response = not_found_resp_;
//      net::notFoundHttpCallback(request, response);
    }
  }

  if (to_send) {
    conn->send([&response](BufferType& buff) {
      response->appendToBuffer(buff);
    });

    if (response->getCloseConnection()) {
      conn->shutdown();
    }
  }
}

template<typename BufferType>
void HTTPServer<BufferType>::addRespCache(const HTTPRequestPtr& req, const HTTPResponsePtr& resp) {
//  std::lock_guard<std::mutex> lk(mutex_g_);
//  boost::unique_lock<boost::shared_mutex> lk(mutex_g_);
//  std::lock_guard<SpinLock> lk(spin_lock_);

  if (req_to_cache_node_iter_.size() >= kMaxReqCached) {
    req_to_cache_node_iter_.erase(cache_node_list_.back().req);
    cache_node_list_.pop_back();
  }

  CacheNode cache_node({req, resp});
  cache_node_list_.push_front(cache_node);
//  req_to_cache_node_iter_[req] = cache_node_list_.begin();
  req_to_cache_node_iter_.emplace(req, cache_node_list_.begin());
}

template<typename BufferType>
bool HTTPServer<BufferType>::getRespCache(const HTTPRequestPtr &req, HTTPResponsePtr& resp) {
//    RLock(mutex_g_, mutex_r_, num_reader_);
//    std::lock_guard<std::mutex> lk(mutex_g_);
//    boost::shared_lock<boost::shared_mutex> lk(mutex_g_);
//  std::lock_guard<SpinLock> lk(spin_lock_);
  if (!req_to_cache_node_iter_.count(req)) return false;

  auto &&cache_node_iter = req_to_cache_node_iter_[req];
  if (cache_node_iter != cache_node_list_.begin()) {
    cache_node_list_.splice(cache_node_list_.begin(), cache_node_list_, cache_node_iter);
  }

  resp = cache_node_list_.begin()->resp;

  return true;
}

// explict instantiations
template class net::HTTPServer<LinkedBuffer>;
template class net::HTTPServer<Buffer>;
