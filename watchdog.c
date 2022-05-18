#include "watchdog.h"

long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds =
      te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
  //   printf("milliseconds: %lld\n", milliseconds);
  return milliseconds;
}

void feed_the_dog(pthread_t task_id) {
  // last_run = current_timestamp();
  TaskListEntry *temp = watchdog.t_list.head;
  while (temp != watchdog.t_list.tail) {
    if (temp->task_id == task_id) {
      temp->last_run = current_timestamp();
      printf("TASK %ld fed the Dog!\n", task_id);
    }
    temp = temp->next;
  }
}

void add_to_watchlist(pthread_t task_id, unsigned int timeout,
                      WDT_CALLBACK cb) {
  if (is_exists(task_id)) {
    printf("DISCARDING THE TASK BECAUSE IT EXISTS ALREADY!");
    return;
  }
  TaskListEntry *new_entry = (TaskListEntry *)malloc(sizeof(TaskListEntry));
  new_entry->last_run = current_timestamp();
  new_entry->next = watchdog.t_list.tail;
  new_entry->timeout = timeout;
  new_entry->prev = watchdog.t_list.tail->prev;
  new_entry->task_id = task_id;
  new_entry->callback = cb;
  watchdog.t_list.tail->prev->next = new_entry;
  watchdog.t_list.tail->prev = new_entry;
}

void *watchDog(void *args) {
  long long temp_time;
  while (1) {
    TaskListEntry *temp = watchdog.t_list.head->next;
    while (temp != watchdog.t_list.tail) {
      temp_time = current_timestamp();
      printf("here! %lld\n", temp_time - temp->last_run);
      if (temp_time - temp->last_run > temp->timeout) {
        if (temp->callback != NULL)
          temp->callback(NULL);
        printf("the watchdog was not fed well!\n");
        fire_the_dog();
      }
      //   temp->last_run = temp_time;
      if (temp->next != watchdog.t_list.tail) {
        temp = temp->next;
      } else {
        break;
      }
    }
    RS_SLEEP(watchdog.sleep_time);
  }
  return NULL;
}

void fire_the_dog() {
  if (watchdog.callback != NULL)
    watchdog.callback(watchdog.callback_arg);
  exit(-1);
};

void init_the_dog(long long sleep_time, WDT_CALLBACK cb) {
  TaskListEntry *head = (TaskListEntry *)malloc(sizeof(TaskListEntry));
  TaskListEntry *tail = (TaskListEntry *)malloc(sizeof(TaskListEntry));
  head->next = tail;
  tail->prev = head;
  watchdog.t_list.head = head;
  watchdog.t_list.tail = tail;
  watchdog.sleep_time = sleep_time;
  watchdog.callback = cb;

  head->prev = NULL;
  head->last_run = current_timestamp();
  head->task_id = 0;

  tail->next = NULL;
  tail->last_run = current_timestamp();
  tail->task_id = 0;

  tail->prev = head;
  head->next = tail;
}

int is_exists(long long task_id) {
  int found = 0;
  TaskListEntry *temp = watchdog.t_list.head->next;
  while (temp != watchdog.t_list.tail) {
    if (temp->task_id == task_id) {
      found = 1;
      break;
    }

    if (temp->next != watchdog.t_list.tail) {
      temp = temp->next;
    } else {
      break;
    }
  }
  return found;
}
