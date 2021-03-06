#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>

#include "tome.h"

/* The \0 prefix is required to use the abstract namespace */
char *socket_path = "\0info.guardianproject.gpg.pinentry";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd, client, rc;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  unlink(socket_path);

  if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

    fprintf(stderr,"[<-] line opened - awaiting second party\n");
    if ( (client = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
      exit(1);
    }

    fprintf(stderr,"[<->] second party attached - connecting parties\n");

    rc = send_fd(client, STDIN_FILENO);
    if( rc == -1 ) {
        perror("sending stdin failed");
        exit(1);
    }
    rc = send_fd(client, STDOUT_FILENO);
    if( rc == -1 ) {
        perror("sending stdout failed");
        exit(1);
    }

    fprintf(stderr, "[+] success. my work here is done.\n");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);

  return 0;
}




