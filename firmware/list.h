// list.h
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned long width;
  unsigned int length;
  unsigned int size;
  void* head;
} List;

List* List_new(unsigned long bytes);
void List_prettyPrint(const List *list);
void List_set(List* l, unsigned int i,const void* element);
void* List_gst(List* l, unsigned int i);
void List_resize(List *l, unsigned int newSize);
void List_append(List*l,void* element);
void List_copyInto(List *l, void *source, unsigned int i);
void List_insert(List*l,unsigned int i,void*element);
void List_remove(List*l,unsigned int i);
void List_zeroOut(List *l);
void List_delete(List*l);
void List_print_as_string(List* l);
int List_filter(List *l, int (*function)(void *));
#define $List_forEach(list,type,item,scope) \
{\
  type item;\
    for (unsigned int _i = 0; _i < (list)->length ; _i++){ \
      item =*((type*) List_gst((list), _i));\
      scope\
    }\
}

List* List_new(unsigned long bytes);

#ifdef __cplusplus
}
#endif
