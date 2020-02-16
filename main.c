
#include "tcpserver.h"

int main(int argc, char* argv[]) {

    int port = argc==2 ? argv[1] : 8090;
    printf("listening on port %d\n", port);
    init_server(port);
    work();
    return 0;
}
