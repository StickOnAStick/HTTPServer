#!/bin/bash

gcc main.c -o bin/HTTP_SERVER

if [[ $? -ne 0 ]]; then
    echo "Failed compilation, cannot run"
    exit 1
fi

./bin/HTTP_Server
