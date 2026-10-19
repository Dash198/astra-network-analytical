/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_unaware/Mesh.h"

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionUnaware {

/**
 * Implements the SM_Long topology: a square mesh plus direct links between
 * routers two hops apart in the east and south directions.
 *
 * The shortcut skips one intermediate router. Hops are shortest-path
 * distances on the augmented graph, not Manhattan distances.
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

    /**
     * Return the shortest-path hop count on the augmented graph.
     */
    [[nodiscard]] int compute_hops_count(DeviceId src, DeviceId dest) const noexcept override;

  private:
    /// all-pairs shortest-path distances, stored row-major
    std::vector<std::vector<int>> distances;
};

}  // namespace NetworkAnalyticalCongestionUnaware
