#include "srcs/net/server/http/http_response.h"
#include "srcs/net/server/http/http_request.h"
#include "srcs/net/types.h"

#include <iostream>

using namespace net;
using namespace std;

int main() {
  std::unordered_map<HTTPRequestPtr, int, HTTPRequstPtrHash, HTTPRequstPtrEqualTo> mp;

  HTTPRequestPtr req = std::make_shared<HTTPRequest>();
  cout << req->to_hash_value() << endl;
  mp.emplace(req, 0);

  cout << mp.count(req) << endl;
  HTTPRequestPtr req1 = std::make_shared<HTTPRequest>();
  cout << req1->to_hash_value() << endl;
  cout << mp.count(req1) << endl;
  mp.emplace(req1, 1);
  cout << mp.size() << endl;
}