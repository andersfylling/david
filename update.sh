#!/bin/bash

git pull
git submodule update --init
git submodule foreach git pull origin master