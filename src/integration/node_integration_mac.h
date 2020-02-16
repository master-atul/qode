// Copyright 2014 GitHub, Inc.
// Copyright 2017 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the MIT license.

#ifndef SRC_NODE_INTEGRATION_MAC_H_
#define SRC_NODE_INTEGRATION_MAC_H_

#include "src/integration/node_integration.h"

namespace qode {

class NodeIntegrationMac : public NodeIntegration {
 public:
  NodeIntegrationMac();
  ~NodeIntegrationMac() override;
  
 private:
  void PollEvents() override;
  void PostTask(const std::function<void()>& task) override;

  // Called when uv's watcher queue changes.
  static void OnWatcherQueueChanged(uv_loop_t* loop);

  DISALLOW_COPY_AND_ASSIGN(NodeIntegrationMac);
};

}  // namespace qode

#endif  // SRC_NODE_INTEGRATION_MAC_H_