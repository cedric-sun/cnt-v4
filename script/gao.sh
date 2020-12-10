#!/bin/bash

for node in {00..08};do
    echo "lin114-${node}.cise.ufl.edu" "$((10${node} + 1))"
done
