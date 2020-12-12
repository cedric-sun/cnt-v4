#!/bin/bash

DIRNAME="cnt_project"
TARBALL="${DIRNAME}.tar.gz"

tar czf "${TARBALL}" "${DIRNAME}"
ssh storm 'rm -rfv cnt; mkdir -v cnt'
scp "${TARBALL}" storm:cnt
ssh storm "cd cnt; tar xf ${TARBALL}; cd ${DIRNAME}"

for node in {00..08};do
    ssh "lin114-${node}.cise.ufl.edu" "cd cnt/${DIRNAME}; ./peerProcess $((10${node}+1)) | tee &"
done
