/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_aware/Mesh.h"

namespace NetworkAnalyticalCongestionAware {

/**
 * Implements the SM_Long topology: a square mesh plus direct links between
 * routers two hops apart in the east and south directions.
 *
 * Shortcuts are ordinary links in the congestion-aware backend, so they carry
 * their own queues and serialization bandwidth.
 */
class SMLong final : public Mesh {
  public:
    /**
     * Constructor.
     *
     * @param npus_count number of NPUs in the mesh (must be a perfect square)
     * @param bandwidth bandwidth of each link
     * @param latency latency of each link
     */
    SMLong(int npus_count, Bandwidth bandwidth, Latency latency,
           std::vector<int> rank_order = {}) noexcept;
};

}  // namespace NetworkAnalyticalCongestionAware
