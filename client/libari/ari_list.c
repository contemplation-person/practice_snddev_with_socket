#include "libari.h"
// 노드 구조체 정의
ari_node *new_node(char *key, void *value)
{
    ari_node *n = malloc(sizeof(ari_node));
    int key_len ;

    if (!key || !n)
        return NULL;
    key_len = strlen(key);

    n->key = malloc(key_len + 1);
    if (!n->key)
    {
        free(n);
        return NULL;
    }

    bzero(n->key, key_len + 1);
    strcpy(n->key, key);

    n->value = value;
    n->next = NULL;

    return n;
}

/**
 * @return key가 존재하면 존재하는 노드 반환,
 * key 가 존재하지 않으면 맨 마지막 노드를 반환
 * key 가 존재하지만 틀린 키라면 NULL 반환
*/
ari_node *search_node(ari_node *head, char *key)
{
    int key_len = strlen(key);

    if (!key)
    {
        while (head && head->next)
        {
            head = head->next;
        }
        return (head);
    }

    while (head && strncmp(head->key, key, key_len + 1))
    {
        head = head->next;
    }

    return head;
}

ari_node *insert_node(ari_node *head_node, ari_node *insert_node, char *key)
{
    ari_node *next_node;
    ari_node *temp_node;

    if (!head_node)
        return (insert_node);
    
    temp_node = search_node(head_node, key);
    if (!temp_node)
    {
        temp_node = search_node(head_node, NULL);
    }
    
    if (!temp_node)
    {
        return (NULL);
    }
    next_node = temp_node->next;
    temp_node->next = insert_node;
    insert_node->next = next_node;

    return head_node;
}

/**
 * @param key가 존재하지 않으면 모두 삭제
 * @param free_value value를 free 할건지 확인
 * return head_node;
 * 해당 키가 없으면 삭제하지 않음.
*/
ari_node *ari_del_node(ari_node *head_node, char *key, void *del_value(void *))
{
    int key_len = strlen(key);
    ari_node *prev_node = head_node;
    ari_node *next_node = head_node;


    if (!head_node)
        return (head_node);
    if (!key)
    {
        while (head_node)
        {
            prev_node = head_node;
            free(prev_node->key);
            if (del_value)
                del_value(prev_node->value);
            free(prev_node);
        }
        return NULL;
    }

    while (next_node && strncmp(next_node->key, key, key_len + 1))
    {
        prev_node = next_node;
        next_node = next_node->next;
    }

    if (!next_node)
        return NULL;
    
    if (prev_node)
    {
        prev_node->next = next_node->next;
    }
    else
    {
        head_node = next_node;
    }

    free(next_node->key);
    if (del_value)
        del_value(next_node->value);
    free(next_node);

    if (head_node == next_node)
        return NULL;

    return head_node;
}
