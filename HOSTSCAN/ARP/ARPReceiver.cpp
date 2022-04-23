// #include "ARPReceiver.h"

// ARPReceiver::ARPReceiver()
// {
//     if (epoll_fd = epoll_create1(0) < 0)
//     {
//         perror("epoll_create1 failed");
//         exit(EXIT_FAILURE);
//     }
// }

// bool ARPReceiver::Prepare(int sock_fd)
// {
//     epoll_event ev;
//     ev.events = EPOLLIN;
//     ev.data.fd = sock_fd;
//     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
//     {
//         perror("epoll_ctl failed");
//         return false;
//     }
// }

// bool ARPReceiver::Receive(int sock_fd)
// {
// }