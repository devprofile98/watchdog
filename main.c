#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.

// -------------------------------
// -------------------------------

long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds =
      te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
  //   printf("milliseconds: %lld\n", milliseconds);
  return milliseconds;
}
// -------------------------------
// -------------------------------

typedef struct taskListEntry {
  struct taskListEntry *next;
  struct taskListEntry *prev;
  long long task_id;
  long long last_run;
  unsigned int timeout;
} TaskListEntry;

// -------------------------------
// -------------------------------

typedef struct taskList {

  TaskListEntry *head;
  TaskListEntry *tail;

} TaskList;

TaskListEntry head;
TaskListEntry tail;

// -------------------------------
// -------------------------------

long long last_run;

// -------------------------------
// -------------------------------

void feed_the_dog(pthread_t task_id) {
  last_run = current_timestamp();
  TaskListEntry *temp = &head;
  while (temp != &tail) {
    if (temp->task_id == task_id) {
      temp->last_run = current_timestamp();
      printf("TASK %ld fed the Dog!\n", task_id);
    }
    temp = temp->next;
  }
}

// -------------------------------
// -------------------------------

void get_watched_by_dog(pthread_t task_id, unsigned int timeout) {
  TaskListEntry *new_entry = (TaskListEntry *)malloc(sizeof(TaskListEntry));
  // printf("%x %x %xclear", new_entry, &head, &tail);
  new_entry->last_run = current_timestamp();
  new_entry->next = &tail;
  new_entry->timeout = timeout;
  new_entry->prev = tail.prev;
  new_entry->task_id = task_id;
  tail.prev->next = new_entry;
  tail.prev = new_entry;
}

// -------------------------------
// -------------------------------

void *myThreadFunc(void *arg) {
  //   sleep(15);
  while (1) {
    sleep(5);
    feed_the_dog(pthread_self());
  }

  return NULL; // what a SAAAAM
}
// -------------------------------
// -------------------------------

void *watchDog(void *args) {
  long long temp_time;
  while (1) {
    TaskListEntry *temp = head.next;
    while (temp != &tail) {
      temp_time = current_timestamp();
      printf("here! %lld\n", temp_time - temp->last_run);
      if (temp_time - temp->last_run > temp->timeout) {
        printf("WATCH DOG DIDN'T FEED GRACEFULLY!\n");
        exit(-1);
      }
      temp->last_run = temp_time;
      if (temp->next != &tail) {
        temp = temp->next;
        break;
      }
    }
    sleep(10);
  }
  return NULL;
}

// -------------------------------
// -------------------------------

int main() {
  last_run = current_timestamp();
  pthread_t thread_id, thread2_id, wdt_id;
  TaskList t_list;
  head.next = &tail;
  tail.prev = &head;
  t_list.head = &head;
  t_list.tail = &tail;
  TaskListEntry a;
  a.next = NULL;
  a.prev = NULL;
  a.last_run = current_timestamp();
  a.task_id = wdt_id;
  // sleep(1);
  head.prev = NULL;
  head.last_run = current_timestamp();
  head.task_id = 0;

  tail.next = NULL;
  tail.last_run = current_timestamp();
  tail.task_id = 0;

  tail.prev = &head;
  head.next = &tail;

  pthread_create(&thread_id, NULL, myThreadFunc, NULL);
  pthread_create(&thread2_id, NULL, myThreadFunc, NULL);

  pthread_create(&wdt_id, NULL, watchDog, NULL);

  get_watched_by_dog(thread_id, 4000);
  get_watched_by_dog(thread2_id, 10000);

  //   pthread_join(thread_id, NULL);
  pthread_join(thread_id, NULL);
  printf("After Thread\n");

  exit(0);
}