/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/SMLong.h"
#include <cassert>
#include <utility>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

SMLong::SMLong(const int npus_count, const Bandwidth bandwidth, const Latency latency,
               std::vector<int> rank_order) noexcept
    : Mesh(npus_count, bandwidth, latency, std::move(rank_order)) {
    assert(npus_count > 0);

    basic_topology_type = TopologyBuildingBlock::SM_Long;

    // Express links are defined between router positions, then mapped to the
    // NPUs that occupy those routers.
    for (auto router = 0; router < npus_count; router++) {
        const auto row = router / dimension;
        const auto col = router % dimension;
        if (col + 2 < dimension) {
            const auto src = device_at_router[router];
            const auto dest = device_at_router[router + 2];
            connect(src, dest, bandwidth, latency, true);
            adjacency[src].push_back(dest);
            adjacency[dest].push_back(src);
        }
        if (row + 2 < dimension) {
            const auto src = device_at_router[router];
            const auto dest = device_at_router[router + 2 * dimension];
            connect(src, dest, bandwidth, latency, true);
            adjacency[src].push_back(dest);
            adjacency[dest].push_back(src);
        }
    }
}
