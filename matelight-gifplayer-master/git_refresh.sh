#!/bin/bash
git fetch origin master
git reset --hard FETCH_HEAD
git clean -df
ps aux | grep python | grep -v "grep python" | awk '{print $2}' | xargs kill -9