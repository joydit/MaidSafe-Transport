/* Copyright (c) 2010 maidsafe.net limited
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    * Neither the name of the maidsafe.net limited nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MAIDSAFE_TRANSPORT_MANAGED_CONNECTIONS_H_
#define MAIDSAFE_TRANSPORT_MANAGED_CONNECTIONS_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "boost/date_time/posix_time/posix_time_duration.hpp"
#include "boost/asio/deadline_timer.hpp"

#include "maidsafe/common/asio_service.h"

#include "maidsafe/transport/rudp_transport.h"
#include "maidsafe/transport/transport.h"

#include "maidsafe/transport/version.h"

#if MAIDSAFE_TRANSPORT_VERSION != 300
# error This API is not compatible with the installed library.\
  Please update the maidsafe_transport library.
#endif

namespace maidsafe {

namespace transport {

typedef std::function<void(const std::string&)> MessageReceivedFunctor;
typedef std::function<void(const Endpoint&)> ConnectionLostFunctor;


class ManagedConnections {
 public:
  ManagedConnections();
  ~ManagedConnections();

  // Creates new transport objects and bootstraps each to one of the provided
  // bootstrap_endpoints.  All the endpoints to which successful bootstrap
  // connections are made are returned.
  std::vector<Endpoint> Bootstrap(
      const std::vector<Endpoint> &bootstrap_endpoints,
      MessageReceivedFunctor message_received_functor,
      ConnectionLostFunctor connection_lost_functor);

  // Returns one of the transport's external endpoints.
  Endpoint GetAvailableEndpoint() const;

  void Add(const Endpoint &peer_endpoint, const std::string &this_node_id);
  void Remove(const Endpoint &peer_endpoint);

  void Send(const Endpoint &this_endpoint,
            const Endpoint &peer_endpoint,
            const std::string &message) const;
  void Ping(const Endpoint &peer_endpoint) const;

 private:
  MessageReceivedFunctor message_received_functor_;
  ConnectionLostFunctor connection_lost_functor_;
  std::unique_ptr<AsioService> asio_service_;
  boost::posix_time::time_duration keep_alive_interval_;
  boost::asio::deadline_timer keep_alive_timer_;
  std::vector<std::unique_ptr<RudpTransport>> rudp_transports_;
  boost::mutex mutex_;
};

}  // namespace transport

}  // namespace maidsafe

#endif  // MAIDSAFE_TRANSPORT_MANAGED_CONNECTIONS_H_