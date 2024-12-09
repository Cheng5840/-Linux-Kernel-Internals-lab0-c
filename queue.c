#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *elem = list_entry(node, element_t, list);
        q_release_element(elem);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!s) {
        return false;
    }
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem) {
        return false;
    }

    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }
    
    list_add(&new_elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!s)
        return false;

    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem) {
        return false;
    }

    new_elem->value = strdup(s);
    if (!new_elem->value) {
        free(new_elem);
        return false;
    }

    list_add_tail(&new_elem->list, head);
    return true;
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node = head->next;
    element_t *elem = list_entry(node, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(node);  // Unlink the node
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    struct list_head *node = head->prev;
    element_t *elem = list_entry(node, element_t, list);

    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    list_del(node);  // Unlink the node
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;
    int len = 0;
    struct list_head *node;
    list_for_each (node, head) {
        len++;
    }
    return len;
}

//* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    int len = 0;
    struct list_head *cur = head->next;
    while(cur!=head){
        len ++;
        cur = cur->next;
    }

    int middle = len/2;
    cur = head->next;

    for (int i=0; i<middle; i++){
        cur = cur->next;
    }

    list_del(cur);
    element_t *mid = list_entry(cur, element_t, list);
    q_release_element(mid);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (head->next->next == head)
        return true;
    // if(list_is_singular()) return true;

    struct list_head dummy;
    dummy.next = head->next;
    struct list_head *prev = &dummy;
    struct list_head *cur = head->next;

    while (cur) {
        element_t *cur_e = list_entry(cur, element_t, list);
        element_t *next_e = list_entry(cur->next, element_t, list);
        if (cur->next && strcmp(cur_e->value, next_e->value)) {
            while (cur->next && strcmp(cur_e->value, next_e->value)) {
                cur = cur->next;
            }
            prev->next = cur->next;

        } else {
            prev = cur;
        }

        cur = cur->next;
    }

    return true;
}
/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head->next == head || head->next->next == head) return; 

    struct list_head* newHead = head->next->next; 
    struct list_head* cur = head->next;
    struct list_head* prev = head;     

    while (cur!=head) {
        struct list_head* nextPair = cur->next->next; 
        struct list_head* second = cur->next;         

        second->next = cur;
        cur->prev = second;
        cur->next = nextPair;
        nextPair->prev = cur;

        if (prev != head) {
            prev->next = second;
            second->prev = prev;
        }

        prev = cur;
        cur = nextPair;
    }
    head->next = newHead;
    newHead->prev = head;
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) 
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *temp;
    list_for_each_safe(node, temp, head)
    {
        struct list_head *prev = node->prev;
        struct list_head *next = node->next;

        node->next = prev;
        node->prev = next;
    }

    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    
    if (!head || head->next->next == head || k == 1) return;
    
   
    struct list_head* prev= head;
    struct list_head* cur = head->next;

    int count = 0;
    //can replace by q_size()
    while (cur!=head){
        count ++;
        cur = cur->next;
    }

    while (count >= k) {
        cur = prev->next;
        struct list_head* next = cur->next;

        // reverse k nodes
        // move last node to first each round  
        for (int i=0; i<k-1; i++){
            cur->next = next->next;
            next->next->prev = cur;

            next->next = prev->next;
            prev->next->prev = next;

            prev->next = next;
            next->prev = prev;
            next = cur->next;
        }

        prev = cur;
        count -= k;
    }

}


/* use for q_sort (merge sort) */
struct list_head* getmid(struct list_head* head) {
    struct list_head* slow = head;
    struct list_head* fast = head->next;

    while (fast != head && fast->next != head){
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

struct list_head* merge(struct list_head* left, struct list_head* right, bool descend) {
    

    struct list_head *dummy = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(dummy);
    struct list_head* temp = dummy;

    while(!list_empty(left) && !list_empty(right)) {

        // ascending
        if (!descend){
            if (strcmp( list_entry(left, element_t, list)->value, list_entry(right, element_t, list)->value) <= 0) {
                left->prev = temp;
                temp->next = left;
                left = left->next;
                temp = temp->next;
            }
            else {
                right->prev = temp;
                temp->next = right;
                right = right->next;
                temp = temp->next;
            }
        }
        // descending
        else {
            if (strcmp( list_entry(left, element_t, list)->value, list_entry(right, element_t, list)->value) >= 0) {
                left->prev = temp;
                temp->next = left;
                left = left->next;
                temp = temp->next;
            }
            else {
                right->prev = temp;
                temp->next = right;
                right = right->next;
                temp = temp->next;
            }
        }
            
    }

    while(left) {
        left->prev = temp;
        temp->next = left;
        temp = temp->next;
        left = left->next;
    }
    while(right) {
        right->prev = temp;
        temp->next = right;
        temp = temp->next;
        right = right->next;
    }

    return dummy;
}
/* Sort elements of queue in ascending/descending order */


static struct list_head *merge_sort(struct list_head *head, bool descend) 
{
    if (list_empty(head) || list_is_singular(head))
        return head;

    struct list_head* mid = getmid(head);
    struct list_head* left = head->next;
    struct list_head* right = mid->next;
    
    mid->next = head;

    left = merge_sort(left, descend);
    right = merge_sort(right, descend);

    return merge(left, right, descend);
}


// merge sort;
void q_sort(struct list_head *head, bool descend) 
{
    if (list_empty(head) || list_is_singular(head))
        return ;

    struct list_head *sorted = merge_sort(head, descend);

    INIT_LIST_HEAD(head);
    list_splice_tail(sorted, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
 // https://leetcode.com/problems/remove-nodes-from-linked-list/
int q_ascend(struct list_head *head)
{
    //SO C90 forbids variable length array ‘stack’
    struct list_head* stack[2000];
    struct list_head dummy = {NULL, NULL};
    stack[0] = &dummy;
    int top = 0;
    struct list_head* cur = head;

    while (cur) {

        while (top > 0 && list_entry(cur, element_t, list)->value < list_entry(stack[top], element_t, list)->value ){
            top --;
        }
        cur->prev = stack[top];
        stack[top]->next = cur;
        top ++;
        stack[top] = cur;
        cur = cur->next;
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
 // https://leetcode.com/problems/remove-nodes-from-linked-list/
int q_descend(struct list_head *head)
{
    struct list_head* stack[2000];
    struct list_head dummy = {NULL, NULL};
    stack[0] = &dummy;
    int top = 0;
    struct list_head* cur = head;

    while (cur) {

        while (top > 0 && list_entry(cur, element_t, list)->value > list_entry(stack[top], element_t, list)->value ){
            top --;
        }
        cur->prev = stack[top];
        stack[top]->next = cur;
        top ++;
        stack[top] = cur;
        cur = cur->next;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */


element_t *mergeq(element_t *list1, element_t *list2, bool descend) {
    if (!list1) return list2;
    if (!list2) return list1;

    element_t *result;

    if ((descend ? strcmp(list1->value, list2->value) >= 0
                 : strcmp(list1->value, list2->value) <= 0)) {
        result = list1;
        result->list.next = &mergeq(list_entry(list1->list.next, element_t, list), list2, descend)->list;
    } else {
        result = list2;
        result->list.next = &mergeq(list1, list_entry(list2->list.next, element_t, list), descend)->list;
    }
    return result;
}

int q_merge(struct list_head *head, bool descend) {
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    struct list_head *node;
    struct list_head *temp;

    // Traverse the chain of queues and merge them pair by pair
    struct list_head *first_queue = NULL;
    struct list_head *second_queue = NULL;

    list_for_each_safe(node, temp, head) {
        queue_contex_t *context1 = list_entry(node, queue_contex_t, chain);

        if (!first_queue) {
            first_queue = context1->q;
        } else {
            second_queue = context1->q;

            // Merge two queues
            element_t *merged_head = NULL;
            element_t *e1, *e2;

            e1 = list_first_entry(first_queue, element_t, list);
            e2 = list_first_entry(second_queue, element_t, list);

            merged_head = mergeq(e1, e2, descend);

            INIT_LIST_HEAD(first_queue);
            struct list_head *merged_list = &merged_head->list;

            // Add merged nodes back into the first queue
            struct list_head *current = merged_list;
            while (current) {
                element_t *entry = list_entry(current, element_t, list);
                list_add_tail(&entry->list, first_queue);
                current = current->next;
            }

            INIT_LIST_HEAD(second_queue);
            second_queue = NULL;
        }
    }

    return q_size(first_queue);
}
