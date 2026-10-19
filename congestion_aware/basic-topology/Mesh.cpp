/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_aware/Mesh.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <utility>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionAware;

Mesh::Mesh(const int npus_count, const Bandwidth bandwidth, const Latency latency,
           std::vector<int> rank_order) noexcept
    : BasicTopology(npus_count, npus_count, bandwidth, latency),
      dimension(static_cast<int>(std::lround(std::sqrt(npus_count)))) {
    assert(npus_count > 0);
    assert(bandwidth > 0);
    assert(latency >= 0);

    if (dimension * dimension != npus_count) {
        std::cerr << "[Error] (network/analytical/congestion_aware) "
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
    device_at_router.assign(npus_count, -1);
    for (auto npu = 0; npu < npus_count; npu++) {
        device_at_router[this->rank_order[npu]] = npu;
    }

    adjacency.assign(npus_count, {});

    const auto add_link = [this, bandwidth, latency](const int src, const int dest) {
        connect(src, dest, bandwidth, latency, true);
        adjacency[src].push_back(dest);
        adjacency[dest].push_back(src);
    };

    // Links are defined between router positions, then mapped to the NPUs that
    // occupy those routers, so a ring over NPU IDs follows the placement.
    for (auto router = 0; router < npus_count; router++) {
        const auto row = router / dimension;
        const auto col = router % dimension;
        if (col + 1 < dimension) {
            add_link(device_at_router[router], device_at_router[router + 1]);
        }
        if (row + 1 < dimension) {
            add_link(device_at_router[router], device_at_router[router + dimension]);
        }
    }
}

Route Mesh::route(const DeviceId src, const DeviceId dest) const noexcept {
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);

    // BFS predecessor search over the undirected NPU graph.
    std::vector<int> predecessor(npus_count, -1);
    std::vector<bool> visited(npus_count, false);
    std::queue<int> frontier;
    visited[src] = true;
    frontier.push(src);
    while (!frontier.empty() && !visited[dest]) {
        const auto current = frontier.front();
        frontier.pop();
        for (const auto neighbor : adjacency[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                predecessor[neighbor] = current;
                frontier.push(neighbor);
            }
        }
    }
    assert(visited[dest]);

    std::vector<int> path;
    for (auto current = dest; current != -1; current = predecessor[current]) {
        path.push_back(current);
        if (current == src) {
            break;
        }
    }
    std::reverse(path.begin(), path.end());

    Route result;
    for (const auto node : path) {
        result.push_back(devices[node]);
    }
    return result;
}
