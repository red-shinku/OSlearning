#!/bin/bash
echo "start compile..."
g++ -std=c++17 -Wall -Wextra main.cpp TcpServer.cc webhandler.cc error.cc -o runtcps
echo "gen runtcps, run it"
