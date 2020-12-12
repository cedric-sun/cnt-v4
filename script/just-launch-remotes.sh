#!/bin/bash

DIRNAME="cnt_project"

for node in {00..08};do
    ssh "${node}" -t "cd cnt/${DIRNAME} && ./peerProcess $((10${node}+1)) &"
    echo "peer started on lin114-${node}"
done
