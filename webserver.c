// 1. Доработать однопоточную версию сервера, доступную по адре- су:
// http://rosettacode.org/wiki/Hello_world/Web_server#C. Обработка каждого
// запроса выполняется в отдельном потоке: при получении запроса создается
// новый поток для его обработки, после отправки результата клиенту поток
// завершает свою работу. Соединение с клиентом закрывается сразу после
// обработки запроса. Подробную информацию о работе с сокетами можно найти в
// книге [2].
// 2. Оценить производительность сервера с помощью утилиты ab4 , входящей в
// комплект поставки веб-сервера Apache. Утилита создана для оценки
// производительности веб-сервера Apache, однако может быть использована и для
// оценки любого другого веб-сервера. Список оцениваемых параметров согласовать
// с преподавателем.
// 3. Оценить максимальное количество потоков, с которым может работать сервер,
// для различных размеров стека по умолчанию (2 Мбайт, 1 Мбайт, 512 Кбайт).
// 4. Добавить в обработчик запроса от клиента запуск простейшего PHP-скрипта,
// возвращающего версию PHP (<?php echo phpversion(); ?>). Вернуть номер версии
// клиенту. Оценить изменение производи- тельности сервера с помощью утилиты ab.

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

char header_templ[] = "HTTP/1.1 200 OK\r\n"
                      "Content-Length: %d\r\n"
                      "Content-Type: text/html; charset=UTF-8\r\n\r\n";

struct Request {
  int id;
  int fd;
};

void handle_request(struct Request *request) {
  // sleep(5);

  char body[100];
  sprintf(body, "Request number %d has been processed\r\n", request->id);

  char header[1000];
  sprintf(header, header_templ, strlen(body));

  char response[1000];
  strncpy(response, header, strlen(header));
  strncpy(response + strlen(header), body, strlen(body));

  write(request->fd, response, strlen(response));
  close(request->fd);
}

struct {
  struct Request *queue;
  int tail;
  int cap;
  int head;
  int max_size;
  pthread_mutex_t mutex;
} jobs;

void push_job(struct Request request) {
  struct Request *new_queue;
  int has_new_queue = 0;
  if (jobs.tail > jobs.cap - 1) {
    jobs.max_size = (jobs.max_size + 1) * 2;
    jobs.cap = jobs.max_size;
    new_queue = malloc(sizeof(struct Request) * jobs.max_size);
    for (int i = 0; i < jobs.tail - jobs.head; i++) {
      new_queue[i] = jobs.queue[i + jobs.head];
    }
    jobs.tail = jobs.tail - jobs.head;
    jobs.head = 0;
    has_new_queue = 1;
  }

  if (has_new_queue) {
    free(jobs.queue);
    jobs.queue = new_queue;
  }

  jobs.queue[jobs.tail] = request;
  jobs.tail++;
}

struct Request pop_job() {
  struct Request r = jobs.queue[jobs.head];
  jobs.head++;

  return r;
}

struct ThreadInfo {
  int id;
};

void *handle_jobs(void *arg) {
  struct ThreadInfo *thread_info = (struct ThreadInfo *)arg;

  while (1) {
    pthread_mutex_lock(&jobs.mutex);

    int has_request = 0;
    struct Request r;
    if (jobs.tail - jobs.head > 0) {
      r = pop_job();
      has_request = 1;
    }

    pthread_mutex_unlock(&jobs.mutex);

    if (has_request) {
      printf("Thread %d | Request ID %d\n", thread_info->id, r.id);
      handle_request(&r);
    }
  }

  return NULL;
}

int main() {
  int one = 1, client_fd;
  struct sockaddr_in svr_addr, cli_addr;
  socklen_t sin_len = sizeof(cli_addr);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    err(1, "can't open socket");
  }

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

  int port = 8080;
  svr_addr.sin_family = AF_INET;
  svr_addr.sin_addr.s_addr = INADDR_ANY;
  svr_addr.sin_port = htons(port);

  if (bind(sock, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1) {
    close(sock);
    err(1, "Can't bind");
  }

  listen(sock, 5);

  jobs.queue = malloc(0);

  int threads_num = 10;
  pthread_t threads[threads_num];
  struct ThreadInfo thread_info[threads_num];
  for (int i = 0; i < threads_num; i++) {
    thread_info[i].id = i;
    pthread_create(&threads[0], NULL, &handle_jobs, &thread_info[i]);
  }

  unsigned int request_id = 0;

  while (1) {
    client_fd = accept(sock, (struct sockaddr *)&cli_addr, &sin_len);
    printf("got connection\n");

    if (client_fd == -1) {
      perror("Can't accept");
      continue;
    }

    struct Request request;

    request.id = ++request_id;
    request.fd = client_fd;

    pthread_mutex_lock(&jobs.mutex);
    push_job(request);
    pthread_mutex_unlock(&jobs.mutex);
  }

  for (int i = 0; i < threads_num; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
