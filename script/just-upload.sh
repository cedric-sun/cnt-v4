#!/bin/bash

DIRNAME="cnt_project"
TARBALL="${DIRNAME}.tar.gz"

tar czf "${TARBALL}" "${DIRNAME}"
scp "${TARBALL}" storm:
ssh storm "rm -rfv cnt && mkdir -v cnt && mv ${TARBALL} cnt/ && cd cnt && tar xf ${TARBALL} && cd ${DIRNAME}"
