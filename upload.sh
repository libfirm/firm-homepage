#!/bin/sh

set -eu

./generate.py
cp -rvpu output/* /ben/www/firm
