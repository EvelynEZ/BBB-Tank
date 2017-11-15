# This python program runs ./test 
# test is an execution file for driver.c so this program is just a quick way to run driver.c

import os
import time

def getch():
	os.system("bash -c \"./test\"")
 
getch()