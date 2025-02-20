#include "mlist.h"

t_mutext_list *mlist_create(void)
{
   t_mutext_list *lista_mutex = malloc(sizeof(t_mutext_list));
   lista_mutex->list = list_create();
   pthread_mutex_init(&(lista_mutex->mutex), NULL);

   return lista_mutex;
}

u_int32_t mlist_size(t_mutext_list *lista_mutex)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   u_int32_t size = list_size(lista_mutex->list);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return size;
}

int8_t mlist_is_empty(t_mutext_list *lista_mutex)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   int8_t vacia = list_is_empty(lista_mutex->list);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return vacia;
}

void mlist_add(t_mutext_list *lista_mutex, void *elemento)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   list_add(lista_mutex->list, elemento);
   pthread_mutex_unlock(&(lista_mutex->mutex));
}

void mlist_add_all(t_mutext_list *lista_mutex, t_mutext_list *otra_lista_mutex)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   list_add_all(lista_mutex->list, otra_lista_mutex->list);
   pthread_mutex_unlock(&(lista_mutex->mutex));
}

void *mlist_get(t_mutext_list *lista_mutex, u_int32_t index)
{
   if (mlist_is_empty(lista_mutex))
      return NULL;

   pthread_mutex_lock(&(lista_mutex->mutex));
   void *elemento = list_get(lista_mutex->list, index);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return elemento;
}

void *mlist_get_last(t_mutext_list *lista_mutex)
{
   return mlist_get(lista_mutex, mlist_size(lista_mutex) - 1);
}

void *mlist_peek(t_mutext_list *lista_mutex)
{
   return mlist_get(lista_mutex, 0);
}

void mlist_push_as_queue(t_mutext_list *lista_mutex, void *elemento)
{
   mlist_add(lista_mutex, elemento);
}

void *mlist_pop_as_queue(t_mutext_list *lista_mutex)
{
   return mlist_remove(lista_mutex, 0);
}

void *mlist_find(t_mutext_list *lista_mutex, int32_t (*criterio)(void *))
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   void *encontrado = list_find(lista_mutex->list, (void *)criterio);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return encontrado;
}

t_mutext_list *mlist_map(t_mutext_list *lista_mutex, void *(*conversor)(void *))
{
   t_mutext_list *nuevo_mlist = malloc(sizeof(t_mutext_list));
   pthread_mutex_init(&(nuevo_mlist->mutex), NULL);

   pthread_mutex_lock(&(lista_mutex->mutex));
   nuevo_mlist->list = list_map(lista_mutex->list, conversor);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return nuevo_mlist;
}

t_mutext_list *mlist_duplicate(t_mutext_list *lista_mutex)
{
   t_mutext_list *nuevo_mlist = malloc(sizeof(t_mutext_list));

   pthread_mutex_lock(&(lista_mutex->mutex));
   t_list *lista = list_duplicate(lista_mutex->list);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   nuevo_mlist->list = lista;
   pthread_mutex_init(&(nuevo_mlist->mutex), NULL);
   return nuevo_mlist;
}

t_list *mlist_to_list(t_mutext_list *lista_mutex)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   t_list *lista = list_duplicate(lista_mutex->list);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return lista;
}

void mlist_iterate(t_mutext_list *lista_mutex, void (*closure)(void *))
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   list_iterate(lista_mutex->list, closure);
   pthread_mutex_unlock(&(lista_mutex->mutex));
}

int32_t mlist_index_of(t_mutext_list *lista_mutex, int32_t (*criterio)(void *))
{
   int32_t index = -1;

   pthread_mutex_lock(&(lista_mutex->mutex));
   t_list_iterator *iterator = list_iterator_create(lista_mutex->list);
   while (list_iterator_has_next(iterator))
   {
      void *elemento = list_iterator_next(iterator);
      if (!criterio(elemento))
         continue;

      index = list_iterator_index(iterator);
      break;
   }
   list_iterator_destroy(iterator);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return index;
}

void *mlist_remove(t_mutext_list *lista_mutex, u_int32_t index)
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   void *elemento = list_remove(lista_mutex->list, index);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return elemento;
}

void *mlist_remove_last(t_mutext_list *lista_mutex)
{
   return mlist_remove(lista_mutex, mlist_size(lista_mutex) - 1);
}

void *mlist_remove_by_condition(t_mutext_list *lista_mutex, int32_t (*condicion)(void *))
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   void *elemento = list_remove_by_condition(lista_mutex->list, (void *)condicion);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   return elemento;
}

void mlist_remove_and_destroy_by_condition(t_mutext_list *lista_mutex,
                                           int32_t (*condicion)(void *),
                                           void (*destructor)(void *))
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   void *elemento = list_remove_by_condition(lista_mutex->list, (void *)condicion);
   pthread_mutex_unlock(&(lista_mutex->mutex));

   destructor(elemento);
}

void mlist_clean(t_mutext_list *lista_mutex, void (*destructor)(void *))
{
   pthread_mutex_lock(&(lista_mutex->mutex));
   list_clean_and_destroy_elements(lista_mutex->list, destructor);
   pthread_mutex_unlock(&(lista_mutex->mutex));
}

void mlist_destroy(t_mutext_list *lista_mutex)
{
   list_destroy(lista_mutex->list);
   pthread_mutex_destroy(&(lista_mutex->mutex));
   free(lista_mutex);
}
