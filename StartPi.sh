#!/bin/bash

echo 'Logging into CIS wireless proxy'
curl -F action=login -F username=DSusername -F password=DSpassword http://wireless.cis.strath.ac.uk/ > /dev/null

echo 'Starting CherryPy'
~pi/vendingmachine/WebInterface/VendingMachine.py &