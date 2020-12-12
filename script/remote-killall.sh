#!/bin/bash

for node in {00..08};do
    echo "node ${node}:"
    ssh "${node}" 'killall -v peerProcess'
done
