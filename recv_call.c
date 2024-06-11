/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <assert.h>


#define PORT 9999

static char* in_buffer = (char *)malloc(4092);
static char* out_buffer = (char *)malloc(4092);

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

void audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    MA_ASSERT(pDevice->capture.format == pDevice->playback.format);
    MA_ASSERT(pDevice->capture.channels == pDevice->playback.channels);

    /* In this example the format and channel count are the same for both input and output which means we can just memcpy(). */
    // in_buffer has the received data, send it to the speaker.
    MA_COPY_MEMORY(pOutput, in_buffer, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
    // out_buffer has the microphone data.
    MA_COPY_MEMORY(pOutput, pInput, frameCount * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
}

void *server_loop(void *arg){

  pthread_t socket_listener, audio_reader;

  int sockfd, newsockfd, portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;

  // Create Socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

  // Bind
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }
  // Listen
  printf("Waiting for a call\n");
  listen(sockfd,5);

  // Accept
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd,
                     (struct sockaddr *) &cli_addr,
                     &clilen);
  printf("Call received, connection established\n");
  if (newsockfd < 0) {
    error("ERROR on accept");
  }

  assert(pthread_create(&socket_listener, NULL, &server_loop, NULL) == 0);
  assert(pthread_join(server_thread, NULL) == 0);

  while(1) {
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) {
      break;
    }
  }
  close(newsockfd);
  close(sockfd);

}

int main(int argc, char *argv[])
{
  pthread_t server_thread, audio_thread;

  printf("Spawning Server thread\n");
  assert(pthread_create(&server_thread, NULL, &server_loop, NULL) == 0);
  assert(pthread_join(server_thread, NULL) == 0);

  return 0;
}
