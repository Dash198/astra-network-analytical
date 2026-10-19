/******************************************************************************
This source code is licensed under the MIT license found in the
LICENSE file in the root directory of this source tree.
*******************************************************************************/

#include "congestion_unaware/SMLong.h"
#include <cassert>
#include <queue>
#include <utility>

using namespace NetworkAnalytical;
using namespace NetworkAnalyticalCongestionUnaware;

SMLong::SMLong(const int npus_count, const Bandwidth bandwidth, const Latency latency,
               std::vector<int> rank_order) noexcept
    : Mesh(npus_count, bandwidth, latency, std::move(rank_order)) {
    assert(npus_count > 0);

    basic_topology_type = TopologyBuildingBlock::SM_Long;

    // Build the augmented graph over router positions.
    std::vector<std::vector<int>> adjacency(npus_count);
    const auto add_edge = [&adjacency](const int source, const int target) {
        adjacency[source].push_back(target);
    };
    for (auto node = 0; node < npus_count; node++) {
        const auto row = node / dimension;
        const auto col = node % dimension;
        if (col + 1 < dimension) {
            add_edge(node, node + 1);
            add_edge(node + 1, node);
        }
        if (row + 1 < dimension) {
            add_edge(node, node + dimension);
            add_edge(node + dimension, node);
        }
        if (col + 2 < dimension) {
            add_edge(node, node + 2);
            add_edge(node + 2, node);
        }
        if (row + 2 < dimension) {
            add_edge(node, node + 2 * dimension);
            add_edge(node + 2 * dimension, node);
        }
    }

    // All-pairs BFS. Sixteen nodes makes this negligible.
    distances.assign(npus_count, std::vector<int>(npus_count, -1));
    for (auto source = 0; source < npus_count; source++) {
        auto& distance = distances[source];
        distance[source] = 0;
        std::queue<int> frontier;
        frontier.push(source);
        while (!frontier.empty()) {
            const auto current = frontier.front();
            frontier.pop();
            for (const auto neighbor : adjacency[current]) {
                if (distance[neighbor] == -1) {
                    distance[neighbor] = distance[current] + 1;
                    frontier.push(neighbor);
                }
            }
        }
    }
}

int SMLong::compute_hops_count(const DeviceId src, const DeviceId dest) const noexcept {
    assert(0 <= src && src < npus_count);
    assert(0 <= dest && dest < npus_count);
    assert(src != dest);

    // Distances are in router space; map the NPUs through the placement.
    const auto hops = distances[rank_order[src]][rank_order[dest]];
    assert(hops > 0);
    return hops;
}
