// list.h
#ifndef LIST_f064fdac_a5b9_4a80_9e04_5a62c5d3bd19_H
#define LIST_f064fdac_a5b9_4a80_9e04_5a62c5d3bd19_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct List {
  unsigned long width;
  unsigned int length;
  unsigned int size;
  void *head;
} List;
typedef struct LL {
  List *l;
} LL;
List *List_new(unsigned long bytes);
void List_prettyPrint(const List *list);
void List_set(List *l, unsigned int i, const void *element);
void *List_gst(List *l, unsigned int i);
void List_resize(List *l, unsigned int newSize);
void List_append(List *l, void *element);
void List_fromArr(List *l, void *source, unsigned int i);
void List_appendFromArr(List *l, void *source, unsigned int i);
int  List_search(List *l, void *value);
void List_insert(List *l, unsigned int i, void *element);
void List_remove(List *l, unsigned int i);
void List_zeroOut(List *l);
void List_delete(List *l);
void List_print_as_string(List *l);
int List_filter(List *l, int (*function)(void *));
#define $List_forEach(list, type, item, scope)                                 \
  {                                                                            \
    type item;                                                                 \
    for (unsigned int _i = 0; _i < (list)->length; _i++) {                     \
      item = *((type *)List_gst((list), _i));                                  \
      scope                                                                    \
    }                                                                          \
  }

#define $List(type) List_new(sizeof(type))

#ifdef __cplusplus
}
#endif

#endif
