all:
	$(MAKE) -C ipv6_multicast_send
	$(MAKE) -C ipv6_multicast_recv

clean:
	$(MAKE) clean -C ipv6_multicast_send
	$(MAKE) clean -C ipv6_multicast_recv
