#ifndef UTILS_MLIST_H
#define UTILS_MLIST_H

#include <stdlib.h>
#include <pthread.h>
#include <commons/collections/list.h>

typedef struct
{
   t_list *list;
   pthread_mutex_t mutex;
} t_mutext_list;

/**
 * @brief
 *
 * @return t_mutext_list*
 */
t_mutext_list *mlist_create(void);

/**
 * @brief
 *
 * @param lista_mutex
 * @return `u_int32_t`
 */
u_int32_t mlist_size(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @return `int8_t` : 1 si la lista está vacía, 0 en caso contrario.
 */
int8_t mlist_is_empty(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param elemento
 */
void mlist_add(t_mutext_list *lista_mutex, void *elemento);

/**
 * @brief
 *
 * @param lista_mutex
 * @param otra_lista_mutex
 */
void mlist_add_all(t_mutext_list *lista_mutex, t_mutext_list *otra_lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param index
 * @return `void*`
 */
void *mlist_get(t_mutext_list *lista_mutex, u_int32_t index);

/**
 * @brief
 *
 * @param lista_mutex
 * @return `void*`
 */
void *mlist_get_last(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @return `void*`
 */
void *mlist_peek(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param elemento
 */
void mlist_push_as_queue(t_mutext_list *lista_mutex, void *elemento);

/**
 * @brief
 *
 * @param lista_mutex
 * @return `void*`
 */
void *mlist_pop_as_queue(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param criterio
 * @return `void*`
 */
void *mlist_find(t_mutext_list *lista_mutex, int32_t (*criterio)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @param conversor
 * @return `t_mutext_list*`
 */
t_mutext_list *mlist_map(t_mutext_list *lista_mutex, void *(*conversor)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @return t_mutext_list*
 */
t_mutext_list *mlist_duplicate(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @return t_list*
 */
t_list *mlist_to_list(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param closure
 */
void mlist_iterate(t_mutext_list *lista_mutex, void (*closure)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @param criterio
 * @return `u_int32_t`
 */
int32_t mlist_index_of(t_mutext_list *lista_mutex, int32_t (*criterio)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @param index
 * @return `void*`
 */
void *mlist_remove(t_mutext_list *lista_mutex, u_int32_t index);

/**
 * @brief
 *
 * @param lista_mutex
 */
void *mlist_remove_last(t_mutext_list *lista_mutex);

/**
 * @brief
 *
 * @param lista_mutex
 * @param condicion
 * @return `void*`
 */
void *mlist_remove_by_condition(t_mutext_list *lista_mutex, int32_t (*condicion)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @param condicion
 * @param destructor
 */
void mlist_remove_and_destroy_by_condition(t_mutext_list *lista_mutex, int32_t (*condicion)(void *), void (*destructor)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 * @param destructor
 */
void mlist_clean(t_mutext_list *lista_mutex, void (*destructor)(void *));

/**
 * @brief
 *
 * @param lista_mutex
 */
void mlist_destroy(t_mutext_list *lista_mutex);

#endif // UTILS_MLIST_H