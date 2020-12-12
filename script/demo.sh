#!/bin/bash

DIRNAME="cnt_project"
TARBALL="${DIRNAME}.tar.gz"

tar czf "${TARBALL}" "${DIRNAME}"
scp "${TARBALL}" storm:
ssh storm "rm -rfv cnt && mkdir -v cnt && mv ${TARBALL} cnt/ && cd cnt && tar xf ${TARBALL} && cd ${DIRNAME}"

for node in {00..08};do
    ssh "${node}" -t "cd cnt/${DIRNAME} && ./peerProcess $((10${node}+1)) &"
    echo "peer started on lin114-${node}"
done
