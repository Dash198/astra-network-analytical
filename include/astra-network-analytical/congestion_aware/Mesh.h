/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_aware/BasicTopology.h"
#include <vector>

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionAware {

/**
 * Implements a 2D square mesh topology.
 *
 * Each NPU is connected to its north/south/east/west neighbours. Routes use
 * shortest-path (BFS) hop counts so the congestion-aware backend can model
 * per-link queues.
 */
class Mesh : public BasicTopology {
  public:
    /**
     * Constructor.
     *
     * @param npus_count number of NPUs in the mesh (must be a perfect square)
     * @param bandwidth bandwidth of each link
     * @param latency latency of each link
     * @param rank_order optional endpoint-to-router placement: rank_order[r]
     *        is the router index occupied by NPU r.  Empty means identity.
     */
    Mesh(int npus_count, Bandwidth bandwidth, Latency latency,
         std::vector<int> rank_order = {}) noexcept;

    /**
     * Construct a shortest-path route between two NPUs.
     */
    [[nodiscard]] Route route(DeviceId src, DeviceId dest) const noexcept override;

  protected:
    /// side length of the square mesh
    int dimension;

    /// router index occupied by each NPU (identity when the config omits it)
    std::vector<int> rank_order;

    /// NPU ID occupying each router (inverse of rank_order)
    std::vector<int> device_at_router;

    /// undirected adjacency list over NPU ids
    std::vector<std::vector<int>> adjacency;
};

}  // namespace NetworkAnalyticalCongestionAware
