#!/usr/bin/python3

import sys
import os
import time
import matplotlib.pyplot as plt

# associtivity range
assoc_range = [1, 2, 4]
# block size range
bsize_range = [b for b in range(6, 7)]
# capacity range
cap_range = [c for c in range(11, 21)]
# number of cores (1, 2, 4)
cores = [1]
# coherence protocol: (none, vi, or msi)
protocol='none'

expname='exp1'
figname='graph2.png'


def get_stats(logfile, key):
    for line in open(logfile):
        if line[2:].startswith(key):
            line = line.split()
            return float(line[1])
    return 0

def run_exp(logfile, core, cap, bsize, assoc):
    trace = 'trace.%dt.long.txt' % core
    cmd="./p5 -t %s -p %s -n %d -cache %d %d %d >> %s" % (
            trace, protocol, core, cap, bsize, assoc, logfile)
    print(cmd)
    os.system(cmd)

def graph():
    timestr = time.strftime("%m.%d-%H_%M_%S")
    folder = "results/"+expname+"/"+timestr+"/"
    os.system("mkdir -p "+folder)

    bus_traffic_wb = {a:[] for a in assoc_range}

    for a in assoc_range:
        for b in bsize_range:
            for c in cap_range:
                for d in cores:
                    logfile = folder+"%s-%02d-%02d-%02d-%02d.out" % (
                            protocol, d, c, b, a)
                    run_exp(logfile, d, c, b, a)
                    bus_traffic_wb[a].append(get_stats(logfile, 'B_written_cache_to_bus_wb'))

    plots = []
    for a in bus_traffic_wb:
        p,=plt.plot([2**i for i in cap_range], bus_traffic_wb[a])
        plots.append(p)
    plt.legend(plots, ['assoc %d' % a for a in assoc_range])
    plt.xscale('log', base=2)
    plt.yscale('log', base=2)
    plt.title('Graph #2: Bus Write Traffic vs Cache Size')
    plt.xlabel('Capacity (log)')
    plt.ylabel('Bus Traffic')
    plt.savefig(figname)
    plt.show()

graph()
