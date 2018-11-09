#include <stdio.h>    // Standard input/output definitions
#include <stdlib.h>
#include <string.h>   // String function definitions
#include <unistd.h>   // for usleep()
#include <getopt.h>
#include <fcntl.h>
#include <termios.h>
#include "arduino-serial-lib.h"
#include <pthread.h>
#include <ctype.h>

#define NUM_THREADS 12
int pressed = 0;

//pthread_mutex_t mutexA;

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

struct thread_data{
    int thread_id;
    const char* port;
    int buf_max;
    char *buf;
};

void *read_port(void* threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    int taskid;
    taskid = my_data->thread_id;
    const char* port;
    port  = my_data->port;
    int buf_max;
    buf_max = my_data->buf_max;
    char *buf_pt;
    buf_pt = my_data->buf;
    char buf[buf_max];
    for (int i = 0; i < buf_max; i++) {
        buf[i] = buf_pt[i];
    }
    int fd = -1;
    int timeout = 2000;
    int baud = 57600;
    char eolchar = '*';
    char filename[13] = "raw/rawxx.txt";
    
    if (taskid == 10) {
	filename[7] = '1';
	filename[8] = '0';
    }
    else if (taskid > 9) {
        char integer = taskid%10 + '0';
        filename[7] = integer;
        integer = taskid/10 + '0';
        filename[8] = integer;
    }
    else{
        char integer = taskid+'0';
        filename[7] = '0';
        filename[8] = integer;
    }
    printf("%s\n",filename);
    FILE *f = fopen(filename, "w");
    fd = serialport_init(port, baud);
    while(1) {
        memset(buf, 0, buf_max);
        printf("%d", serialport_read_until(fd, buf, eolchar, buf_max, timeout));
        printf("cycle complete for thread # %d\n", taskid);
        for (int i=0; i<buf_max; i++){
            char this = *(buf+i);
            if (isalpha(this) || isdigit(this) || this == ' ' || this == '\n' || this == ':')
                fprintf(f,"%c",this);
                //printf("%c",this);
        }
        //printf("\n");
        //pthread_mutex_lock(&mutexA);
        if (pressed == 1) {
                printf("breaking..\n");
                break;
        }
        //pthread_mutex_unlock(&mutexA);
    }
    fclose(f);

    pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
    int fd = -1;
    int buf_max = 5000;
    char buf[buf_max];
    char buf2[buf_max];
    char eolchar = '*';
    const char* port = "/dev/ttyUSB2";
    int baud = 57600;
    int timeout = 2000;
    int rc;

    pthread_t threads[NUM_THREADS];

    struct thread_data thread_data_array[NUM_THREADS];
    char* ports[NUM_THREADS] = {"/dev/ttyUSB0", "/dev/ttyUSB1",
         "/dev/ttyACM0",
         "/dev/ttyACM1",
         "/dev/ttyACM2",
         "/dev/ttyACM3",
         "/dev/ttyACM4",
         "/dev/ttyACM5",
         "/dev/ttyACM6",
         "/dev/ttyACM7",
         "/dev/ttyACM8",
         "/dev/ttyACM9",
        };
    char* buffers[NUM_THREADS];
    for (int t = 0; t < NUM_THREADS; t++) {
        char buf[buf_max];
        buffers[t] = buf;
    }

    // if (pthread_mutex_init(&mutexA, NULL)) {
    //     printf("ERROR; return code from pthread_create() is %d\n", rc);
    //     exit(-1);
    // }

    for (int t = 0; t < NUM_THREADS; t++) {
        thread_data_array[t].thread_id = t;
        thread_data_array[t].port = ports[t];
        thread_data_array[t].buf_max = buf_max;
        thread_data_array[t].buf = buffers[t];
        rc = pthread_create(&threads[t], NULL, read_port, (void*)&thread_data_array[t]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

    }

    sleep(1);
    while(1) {
        if (kbhit()) {
            pressed = 1;
            break;
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        if (pthread_join(threads[t],NULL)){
            printf("\n ERROR on join\n");
            exit(19);
        }
    }
}
