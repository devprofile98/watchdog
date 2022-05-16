#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.

#include "watchdog.h"

void *myThreadFunc(void *arg) {
  while (1) {
    sleep(5);
    feed_the_dog(pthread_self());
  }
  return NULL;
}

int main() {
  pthread_t thread_id, thread2_id, wdt_id;

  TaskListEntry a;
  a.next = NULL;
  a.prev = NULL;
  a.last_run = current_timestamp();
  a.task_id = wdt_id;

  init_the_dog(1); // sleep time in second

  // create two thread for common task
  pthread_create(&thread_id, NULL, myThreadFunc, NULL);
  pthread_create(&thread2_id, NULL, myThreadFunc, NULL);
  // create the watchdog task
  pthread_create(&wdt_id, NULL, watchDog, NULL);

  // subscribe the two common task to dog watchlist
  add_to_watchlist(thread_id, 6000);   // timeout in milli seconds
  add_to_watchlist(thread2_id, 10000); // timeout in milli seconds

  // prevent the program from exiting!
  pthread_join(thread_id, NULL);

  exit(0);
}