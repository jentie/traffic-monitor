#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# evaluate-addresses - ... from esp traffic monitor
#
# 2020-01-24, jens
#


import sys, socket


for line in sys.stdin:
#   print(line)

    addr = line.strip()
    if addr:
        try:
            name, aliases, address_list = socket.gethostbyaddr(addr)
            print(addr, name, sep="\t")
        except socket.herror:
            print(addr, "host not found", sep="\t")
            pass



