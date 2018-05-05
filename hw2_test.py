# HW2 test.py
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

print("Starting I/O test.\n")

#write and read 10,000 times
for lp in range(10000):
	#set our string to all lowercase letters
	string.ascii_letters
	'abcdefghijklmnopqrstuvwxyz'

	#generate a strings with random letters
	s1 = ""
	for x in range(0, 1000):
		s1 += random.choice(string.ascii_lowercase)

	#write the string to a file
	t1 = open("textFile.txt", "w+")
	t1.write(s1 + "\n")

	#close the file
	t1.close()

	#open the file and then read
	t1 = open("textFile.txt", "r+")
	t1.read()
	
	#close the file
	t1.close()

print("I/O test complete.\n")
