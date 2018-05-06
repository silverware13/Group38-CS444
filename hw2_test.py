# HW2 test
# CS444 Spring 2018
# -----------------
# Name: Zachary Thomas
# Date: 5/4/2018
# ------------------------------------
# Generates a ton of I/O on the kernel 
# to test our I/O solution.
# ------------------------------------
# Cited refrences:
# repurposed my own code from my CS344 python assignment
# https://github.com/silverware13/Basic-Py/blob/master/mypython.py

#import modules
from __future__ import print_function
import random
import string
import os, glob

print("Starting I/O test.\n")

#set our string to all lowercase letters
string.ascii_letters
'abcdefghijklmnopqrstuvwxyz'

#generate a strings with random letters
s1 = ""
for x in range(0, 1000000):
	s1 += random.choice(string.ascii_lowercase)

#write 100 files
for lp in range(5):

	#write the string to a file
	file_name = "TEST_SCATTER_%d.txt" % lp
	t1 = open(file_name, "w+")
	t1.write(s1 + "\n")

	#close the file
	t1.close()

	#open the file and then read
	t1 = open(file_name, "r+")
	t1.read()
	
	#close the file
	t1.close()
	
	#write more to a file
	t1 = open(file_name, "a+")
	t1.write(s1 + "\n")
	
	#close the file
	t1.close()

#delete all test files
for filename in glob.glob("./TEST_SCATTER*"):
	os.remove(filename)

print("I/O test complete.\n")
