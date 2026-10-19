/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#pragma once

#include "common/Type.h"
#include "congestion_unaware/BasicTopology.h"
#include <vector>

using namespace NetworkAnalytical;

namespace NetworkAnalyticalCongestionUnaware {

/**
 * Implements a 2D square mesh topology.
 *
 * Mesh(16) is the 4x4 topology used by the P08 chiplet study. A send between
 * two NPUs takes the Manhattan distance in hops.
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
     * Compute the Manhattan distance between two NPUs.
     */
    [[nodiscard]] int compute_hops_count(DeviceId src, DeviceId dest) const noexcept override;

  protected:
    /// side length of the square mesh
    int dimension;

    /// router index occupied by each NPU (identity when the config omits it)
    std::vector<int> rank_order;
};

}  // namespace NetworkAnalyticalCongestionUnaware
