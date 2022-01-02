#include "../include/socket_can.hpp"

namespace socket_can
{
SocketCAN::SocketCAN(const char * ifname) :
  ifname_(ifname),
  connected_(false),
  timeout_(1000l)
{
  init();
}

SocketCAN::SocketCAN(const char * ifname, long timeout) :
  ifname_(ifname),
  connected_(false),
  timeout_(timeout)
{
  init();
}

SocketCAN::~SocketCAN()
{
  if (close(socket_) < 0) {
    perror("Closing: ");
    printf("Error: %d", errno);
  }
}

void SocketCAN::init()
{
  if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("Error while opening socket");
    return;
  }

  struct ifreq ifr{};
  strcpy(ifr.ifr_name, ifname_);
  ioctl(socket_, SIOCGIFINDEX, &ifr);

  struct sockaddr_can addr{};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  //printf("%s at index %d\n", ifname_, ifr.ifr_ifindex);

  if (bind(socket_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    perror("Error in socket bind");
    return;
  }

  int error = 0;
  socklen_t len = sizeof (error);
  int retval = getsockopt (socket_, SOL_SOCKET, SO_ERROR, &error, &len);
  if (retval != 0) {
    /* there was a problem getting the error code */
    printf("Error getting socket error code: %s\n", strerror(retval));
    return;
  }

  if (error != 0) {
    /* socket has a non zero error status */
    printf("Socket error: %s\n", strerror(error));
    return;
  }

  struct timeval timeout{};
  timeout.tv_sec = (timeout_ / 1000);
  timeout.tv_usec = (timeout_ % 1000) * 1000;

  if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *) & timeout, sizeof(timeout)) < 0) {
    perror("Setting timeout failed");
  }

  connected_ = true;
}

bool SocketCAN::is_connected()
{
  return connected_;
}

bool SocketCAN::write(uint32_t can_id, uint8_t dlc, uint8_t * data)
{
  struct can_frame frame{};

  frame.can_id = can_id;
  frame.can_dlc = dlc;
  memcpy(frame.data, data, dlc * sizeof(uint8_t));

  auto num_bytes = ::write(socket_, &frame, sizeof(struct can_frame));

  return num_bytes > 0;
}

bool SocketCAN::read(uint32_t * can_id, uint8_t * dlc, uint8_t * data)
{
  struct can_frame frame{};
  auto num_bytes = ::read(socket_, &frame, sizeof(struct can_frame));
  if (num_bytes != sizeof(struct can_frame)) {
    return false;
  }

  (* can_id) = frame.can_id;
  (* dlc) = frame.can_dlc;
  memcpy(data, frame.data, sizeof(frame.data));

  return true;
}
}