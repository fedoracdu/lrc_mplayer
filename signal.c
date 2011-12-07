/*
 * =====================================================================================
 *
 *       Filename:  signal.c
 *
 *    Description:  deal with signal
 *
 *        Version:  1.0
 *        Created:  12/07/2011 02:40:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

#include "sig_deal.h"

void child_retrieve(int signum)
{
	wait(NULL);
	printf("retireve child inforamtion\n");
}

void sig_deal_child(void)
{
	signal(SIGCHLD, child_retrieve);
}
