/*
 * Copyright (c) 2020 Matthew Martin <phy1729@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int
main(int argc, char *argv[]) {
	struct sockaddr_in addr = {0};
	size_t arglen;
	int fd;

	if (pledge("stdio inet", NULL) == -1) {
		err(1, "pledge");
	}

	if (argc != 2) {
		errx(1, "pidentd ident");
	}

	arglen = strlen(argv[1]);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		err(1, "socket");
	}

	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(113);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		err(1, "bind");
	}

	if (listen(fd, 1) == -1) {
		err(1, "listen");
	}

	while (1) {
		char buf[256];
		int conn;
		size_t size;
		socklen_t addrlen;

		addrlen = sizeof(addr);
		conn = accept(fd, (struct sockaddr *)&addr, &addrlen);
		if (conn == -1) {
			warn("accept");
		}
		size = read(conn, buf, sizeof(buf));
		if (size == -1) {
			warn("read");
			continue;
		} else if (size > sizeof(buf)) {
			warn("ident request too large");
			continue;
		}

		write(conn, buf, size);
		write(conn, " : USERID : UNIX : ", 19);
		write(conn, argv[1], arglen);
	}

	return 0;
}
