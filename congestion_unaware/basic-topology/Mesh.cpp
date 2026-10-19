/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_unaware/Mesh.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utility>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionUnaware;

Mesh::Mesh(const int npus_count, const Bandwidth bandwidth, const Latency latency,
           std::vector<int> rank_order) noexcept
    : BasicTopology(npus_count, bandwidth, latency),
      dimension(static_cast<int>(std::lround(std::sqrt(npus_count)))) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    if (dimension * dimension != npus_count) {
        std::cerr << "[Error] (network/analytical/congestion_unaware) "
                  << "Mesh requires a square number of NPUs, got " << npus_count << std::endl;
        std::exit(-1);
    }

    basic_topology_type = TopologyBuildingBlock::Mesh;

    // Empty placement means "NPU r sits at router r".
    if (rank_order.empty()) {
        this->rank_order.resize(npus_count);
        for (auto npu = 0; npu < npus_count; npu++) {
            this->rank_order[npu] = npu;
        }
    } else {
        assert(static_cast<int>(rank_order.size()) == npus_count);
        this->rank_order = std::move(rank_order);
    }
}

int Mesh::compute_hops_count(const DeviceId src, const DeviceId dest) const noexcept {
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);
    assert(src != dest);

    // Hops are measured between the routers occupied by the two NPUs.
    const auto src_router = rank_order[src];
    const auto dest_router = rank_order[dest];
    const auto src_row = src_router / dimension;
    const auto src_col = src_router % dimension;
    const auto dest_row = dest_router / dimension;
    const auto dest_col = dest_router % dimension;

    return std::abs(src_row - dest_row) + std::abs(src_col - dest_col);
}
