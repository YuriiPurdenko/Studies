#include "csapp.h"
#include "queue.h"

typedef struct node {
  SLIST_ENTRY(node) link;
  long value;
} node_t;

typedef SLIST_HEAD(, node) list_t;

node_t *new_node(long value) {
  node_t *node = malloc(sizeof(node_t));
  node->value = value;
  return node;
}

#define N 10

int main(void) {
  //list_t head;
  list_t head = SLIST_HEAD_INITIALIZER(head);
  
  for (int i = 0; i < N; i++){
    //SLIST_INSERT_HEAD(&head, new_node(i), link); //dwukrotnie zostanie wywolana funckja new_mode()
    node_t *new = new_node(i);
    SLIST_INSERT_HEAD(&head, new, link);
  }
  //free(SLIST_FIRST(&head)); //zwalniamy tylko pierwszy element z listy
  /*
  node_t *next = SLIST_FIRST(&head);
  while ( next != SLIST_END()){
    node_t *el = next;
    next = SLIST_NEXT(next,link);
    free(el);
  }
    */
  while(!SLIST_EMPTY(&head)){
    node_t *elem = SLIST_FIRST(&head);
    SLIST_REMOVE_HEAD(&head, link);
    free(elem);
  }
  return 0;
}
