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
  list_t head;
  
  //nie bylo inicjalizacji listy
  SLIST_INIT(&head);
  for (int i = 0; i < N; i++)
  {
  	//SLIST_INSERT_HEAD(&head,new_node(i),link);
  	
  	node_t *ptr= new_node(i);
  	//wskaźnik na nowy węzeł należy przekazać jako wartość, a nie wywołanie, bo to jest makro
    SLIST_INSERT_HEAD(&head,ptr,link);
   	 
  }
  
  //to jest zle, bo zwalniam tylko naglowek listy, a nie jej elementy
  //free(SLIST_FIRST(&head));
  
  //tak powinno byc ok, zwalniam cala liste
  while (!SLIST_EMPTY(&head)) {
  
  	
  	node_t *elem = SLIST_FIRST(&head);
  	
  	SLIST_REMOVE_HEAD(&head, link);
  	
  	free(elem);
  }
  
  
  return 0;
}
