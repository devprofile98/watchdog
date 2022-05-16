#ifndef RS_WATCHDOG_H
#define RS_WATCHDOG_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.

#define RS_SLEEP(x) sleep(x)

// function to receive current timestamp
// -------------------------------------
long long current_timestamp();

// represents each entry in TaskList
// -----------------------------------
typedef struct taskListEntry {
  struct taskListEntry *next;
  struct taskListEntry *prev;
  long long task_id;
  long long last_run;
  unsigned int timeout;
} TaskListEntry;

// List to store tasks that are watched by dog
// -----------------------------------
typedef struct taskList {
  TaskListEntry *head;
  TaskListEntry *tail;
} TaskList;

// represent the Dog
// -----------------------------------
typedef struct Dog {
  TaskList t_list;
  long long sleep_time;
} WatchDog;

static WatchDog watchdog;

void init_the_dog();

// feed the dog to prevent it's fireup
// -----------------------------------
void feed_the_dog(pthread_t task_id);

// subscribe to watchdog
// -----------------------------------
void add_to_watchlist(pthread_t task_id, unsigned int timeout);

// The watchgod Task
// -----------------------------------
void *watchDog(void *args);

#endif // RS_WATCHDOG_H