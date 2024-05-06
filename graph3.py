#!/usr/bin/python3

import sys
import os
import time
import matplotlib.pyplot as plt

# associtivity range
assoc_range = [4]
# block size range
bsize_range = [b for b in range(1, 14)]
# capacity range
cap_range = [16]
# number of cores (1, 2, 4)
cores = [1]
# coherence protocol: (none, vi, or msi)
protocol='none'

expname='exp1'
figname='graph3.png'


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
    bus_traffic_wt = {a:[] for a in assoc_range}

    for a in assoc_range:
        for b in bsize_range:
            for c in cap_range:
                for d in cores:
                    logfile = folder+"%s-%02d-%02d-%02d-%02d.out" % (
                            protocol, d, c, b, a)
                    run_exp(logfile, d, c, b, a)
                    bus_traffic_wb[a].append(get_stats(logfile, 'B_written_cache_to_bus_wb'))
                    bus_traffic_wt[a].append(get_stats(logfile, 'B_written_cache_to_bus_wt'))

    plots = []
    for a in bus_traffic_wb:
        p, = plt.plot([2**i for i in bsize_range], bus_traffic_wb[a], label='Write-Back')
        plots.append(p)
    plots2 = []
    for a in bus_traffic_wt:
        p, = plt.plot([2**i for i in bsize_range], bus_traffic_wt[a], label='Write-Through')
        plots2.append(p)

    plt.legend(plots + plots2, ['Write-Back Traffic'] + ['Write-Through Traffic'])
    plt.xscale('log', base=2)
    plt.yscale('log', base=2)
    plt.title('Graph #3: Traffic in Bytes vs Block Size')
    plt.xlabel('Block Size of Cache (in bytes)')
    plt.ylabel('Cache-to-Bus Traffic (in bytes)')
    plt.savefig(figname)
    plt.show()

graph()