/* Frag test script
 * CS444 Spring2018
 * ----------------
 * Name: Zachary Thomas
 * Date: 6/2/2018
 * ----------------
 * This simple script simply
 * uses our SLOB system call.
 */

#include <stdio.h>
#include <unistd.h>

int main()
{
	syscall(359);
	return 0;
}
