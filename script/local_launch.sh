#!/bin/bash

for i in {1001..1009};do
    ./peerProcess ${i} &
done
