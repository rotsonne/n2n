/*
 * test.cpp
 *
 *  Created on: Dec 2, 2014
 *      Author: emin
 */
#include <net/if.h>
#include <linux/if_tun.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ancillary.h"
#include <sys/socket.h>
#include <stdlib.h>


int tuntap_open(char *dev, char *device_mac, char *address_mode, char *device_ip, char* device_mask, int mtu)
{
	struct ifreq ifr;
	int fd, fd_sock;
	char *tuntap_device = "/dev/net/tun";

#define N2N_LINUX_SYSTEMCMD_SIZE 128

	char buf[N2N_LINUX_SYSTEMCMD_SIZE];
	int rc;

	fd = open(tuntap_device, O_RDWR);

	printf("fd: %d \n", fd);
	if(fd < 0) {
		printf("ERROR: ioctl() [%s][%d]\n", strerror(errno), errno);
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP|IFF_NO_PI; /* Want a TAP device for layer 2 frames. */
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	rc = ioctl(fd, TUNSETIFF, (void *)&ifr);

	if(rc < 0) {
		printf("error on ioctl\n");
		close(fd);
		return -1;
	}

	if ( device_mac && device_mac[0] != '\0' )
	  {
	      snprintf(buf, sizeof(buf), "/sbin/ifconfig %s hw ether %s",
	               ifr.ifr_name, device_mac );
	      system(buf);
	  }

	  if ( 0 == strncmp( "dhcp", address_mode, 5 ) )
	  {
	      snprintf(buf, sizeof(buf), "/sbin/ifconfig %s %s mtu %d up",
	               ifr.ifr_name, device_ip, mtu);
	  }
	  else
	  {
	      snprintf(buf, sizeof(buf), "/sbin/ifconfig %s %s netmask %s mtu %d up",
	               ifr.ifr_name, device_ip, device_mask, mtu);
	  }

	  system(buf);

	if ( (fd_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}
	
	if(!ancil_send_fd(fd_sock, fd))
	{
		printf("done");
	}
	else
	{
		printf("NOT done");	
	}
	
	return(fd);
}


int main()
{
	tuntap_open("edge0", "6a:c3:d5:a5:6e:4d", "static", "10.11.12.13", "255.255.255.0", 1500);
	while(1) {}
}



