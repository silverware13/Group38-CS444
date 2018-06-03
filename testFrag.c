/* Frag test script
 * CS444 Spring2018
 * ----------------
 * Name: Zachary Thomas
 * Date: 6/2/2018
 * ----------------
 * This simple script shows a SLOBs
 * fragmentation sufferred by using
 * a system call.
 */

#include <stdio.h>
#include <unistd.h>

int main()
{
	int callVal = syscall(359);
	printf("The system call worked: %lu.\n", callVal);
	return 0;
}
