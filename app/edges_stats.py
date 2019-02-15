#!/usr/bin/env python

import argparse
import h5py
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import os

args_parser = argparse.ArgumentParser()
args_parser.add_argument("input", type=str, metavar="edge_file")
args_parser.add_argument("--with-edges-per-node", action="store_true",
                         default=False)

try:
    options = args_parser.parse_args()
except Exception as e:
    print(e)
    exit(-1)

def ranges_and_edges(index):

    nodes_to_ranges = np.array(index["node_id_to_ranges"])
    range_to_edges = np.array(index["range_to_edge_id"])

    num_nodes = nodes_to_ranges.shape[0]
    ranges_per_node = \
        (nodes_to_ranges[:, 1] - nodes_to_ranges[:, 0]).reshape(num_nodes)

    if options.with_edges_per_node:
        edges_per_node = np.zeros(num_nodes, dtype="u8")
        for i, (n, m) in enumerate(nodes_to_ranges):
            ranges = range_to_edges[n:m, :]
            edges_per_node[i] += np.sum((ranges[:, 1] - ranges[:, 0]))
    else:
        edges_per_node = None

    return ranges_per_node, edges_per_node

def population_stats(name, population):
    try:
        afferent = population["indices"]["target_to_source"]
        efferent = population["indices"]["source_to_target"]
    except:
        print("Couldn't open node to edge index from population %s" % name)
        return

    afferent_ranges_per_node, afferent_edges_per_node = \
        ranges_and_edges(afferent)
    efferent_ranges_per_node, efferent_edges_per_node = \
        ranges_and_edges(efferent)

    if options.with_edges_per_node:
        fig, axes = plt.subplots(ncols=2, nrows=2, figsize=(8, 8))
        axes00 = axes[0, 0]
        axes01 = axes[0, 1]
    else:
        fig, axes = plt.subplots(ncols=2, nrows=1, figsize=(8, 4))
        axes00 = axes[0]
        axes01 = axes[1]

    fig.suptitle(name)

    axes00.set_title("target to source")
    axes00.hist(afferent_ranges_per_node)
    axes00.axvline(np.median(afferent_ranges_per_node),
                   linestyle='dashed', linewidth=1)
    axes00.set_ylabel("ranges per node")
    if options.with_edges_per_node:
        axes[1, 0].hist(afferent_edges_per_node)
        axes[1, 0].set_ylabel("edges per node")

    axes01.set_title("source to target")
    axes01.hist(efferent_ranges_per_node)
    axes01.axvline(np.median(efferent_ranges_per_node),
                       linestyle='dashed', linewidth=1)
    if options.with_edges_per_node:
        axes[1, 1].hist(efferent_edges_per_node)

    plt.savefig("%s.png" % name, dpi=100)

edge_file = h5py.File(options.input, "r")
edges = edge_file["edges"]
for name in edges:
    population_stats(
        "%s@%s" % (name, os.path.basename(os.path.splitext(options.input)[0])),
        edges[name])

