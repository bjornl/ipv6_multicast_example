/*	IPv6 multicast example - ipv6_multicast_send.c
	2012 - Bjorn Lindgren <nr@c64.org>
	https://github.com/bjornl/ipv6_multicast_example
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	struct sockaddr_in6 saddr;
	struct ipv6_mreq mreq;
	char buf[1400];
	ssize_t len;
	int sd, fd, on = 1, ifidx = 0;

	if (argc < 3) {
		printf("\nUsage: %s <address> <port>\n\nExample: %s ff02::5:6 12345\n\n", argv[0], argv[0]);
		return 1;
	}

	sd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
		perror("setsockopt");
		return 1;
	}

	if (setsockopt(sd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifidx, sizeof(ifidx))) {
		perror("setsockopt");
		return 1;
	}

	memset(&saddr, 0, sizeof(struct sockaddr_in6));
	saddr.sin6_family = AF_INET6;
	saddr.sin6_port = htons(atoi(argv[2]));
	inet_pton(AF_INET6, argv[1], &saddr.sin6_addr);

	memcpy(&mreq.ipv6mr_multiaddr, &saddr.sin6_addr, sizeof(mreq.ipv6mr_multiaddr));
	mreq.ipv6mr_interface = ifidx;

	if (setsockopt(sd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq))) {
		perror("setsockopt");
		return 1;
	}

	fd = open("/dev/stdin", O_RDONLY, NULL);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	while (len) {
		len = read(fd, buf, 1400);
		/* printf("read %zd bytes from fd\n", len); */
		if (!len) {
			break;
		} else if (len < 0) {
			perror("read");
			return 1;
		} else {
			len = sendto(sd, buf, len, 0, (const struct sockaddr *) &saddr, sizeof(saddr));
			/* printf("sent %zd bytes to sd\n", len); */
		}
	}

	close(sd);
	close(fd);

	return 0;
}
