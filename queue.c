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
    // printf("%s\n",new_elem->value);
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

    list_del(node);  
    return elem;
}


// why sp size need to be bufsize, cannot be strlen
// element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
// {
//     if (!head || list_empty(head))
//         return NULL;
//     struct list_head *node = head->next;
//     element_t *elem = list_entry(node, element_t, list);
//     size_t len = strlen(elem->value)+1;
//     if (sp && bufsize > 0 && len < bufsize) {
//         strncpy(sp, elem->value, len - 1);
//         sp[ len ] = '\0';
//     }

//     list_del(node);  
//     return elem;
// }


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

    if (!head || list_empty(head) || list_is_singular(head))
        return true;
    struct list_head *prev = head;
    struct list_head *cur = head->next;
    struct list_head *tmp;
    struct list_head *curnext;

    while (cur!=head) {
        if (cur->next != head && strcmp(list_entry(cur, element_t, list)->value, list_entry(cur->next, element_t, list)->value) == 0) {
            while (cur->next != head && strcmp(list_entry(cur, element_t, list)->value, list_entry(cur->next, element_t, list)->value) == 0) {
                // curnext = cur->next;
                tmp = cur;
                cur = cur->next;
                element_t *elem = list_entry(tmp, element_t, list);
                q_release_element(elem);
            }
            cur->next->prev = prev;
            prev->next = cur->next;
            curnext = cur->next;
            element_t *elem_cur = list_entry(cur, element_t, list);
            q_release_element(elem_cur);
            cur = curnext;

        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (head->next == head || head->next->next == head) return; 

    struct list_head* newHead = head->next->next; 
    struct list_head* cur = head->next;
    struct list_head* prev = head;     

    while (cur!=head && cur->next!=head) {
        struct list_head* nextPair = cur->next->next; 
        struct list_head* second = cur->next;         

        second->next = cur;
        cur->prev = second;
        cur->next = nextPair;
        nextPair->prev = cur;

        prev->next = second;
        second->prev = prev;

        prev = cur;
        cur = nextPair;
    }
    head->next = newHead;
    newHead->prev = head;
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



struct list_head* merge_two_lists (struct list_head* left, struct list_head* right, bool descend) {
    if (list_empty(left)) return right;
    if (list_empty(right)) return left;
    LIST_HEAD(temphead);

    while (!list_empty(left) && !list_empty(right)) {
        element_t *l = list_first_entry(left, element_t, list);
        element_t *r = list_first_entry(right, element_t, list);
        if ((descend && strcmp(l->value, r->value) > 0) ||
            (!descend && strcmp(l->value, r->value) <= 0))
            list_move_tail(&l->list, &temphead);
        else
            list_move_tail(&r->list, &temphead);
    }
    list_splice_tail_init(left, &temphead);
    list_splice_tail_init(right, &temphead);
    list_splice(&temphead, left);
    return left;
}

struct list_head* sort_helper(struct list_head *head, bool descend) {
    if (!head || list_empty(head) || list_is_singular(head))
        return head;

    // find mid
    LIST_HEAD(righthead);
    struct list_head *slow = head, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *right = slow->next;
    // printf("right str: %s\n", list_entry(right, element_t, list)->value);
    righthead.next = right;
    righthead.prev = head->prev;
    head->prev->next = &righthead;
    right->prev = &righthead;

    slow->next = head;
    head->prev = slow;

    struct list_head *left_sorted = sort_helper(head, descend);
    // printf("left_sorted str: %s\n", list_entry(left_sorted->next, element_t, list)->value);

    struct list_head *right_sorted = sort_helper(right->prev, descend);

    return merge_two_lists(left_sorted, right_sorted, descend);
}



void q_sort(struct list_head *head, bool descend) {
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *sorted = sort_helper(head, descend);
    head = sorted;
}
// }


// ##################### Bubble Sort ######################
// void swap(element_t* a, element_t* b) {
//     char *temp = a->value;
//     a->value = b->value;
//     b->value = temp;
// }

// void q_sort(struct list_head *head, bool descend) {
//     if (list_empty(head) || list_is_singular(head)) return; // Empty or single node list
    
//     int swapped;
//     struct list_head *current = NULL;

//     do {
//         swapped = 0;
//         current = head->next; // Start from the first node

//         while (current->next != head) { // Loop until the last node
//             element_t* curelem = list_entry(current, element_t, list);
//             element_t* nextelem = list_entry(current->next, element_t, list);
//             if (strcmp(curelem->value, nextelem->value) > 0) {
//                 swap(curelem, nextelem);
//                 swapped = 1;
//             }
//             current = current->next;
//         }
//         //last = current; // Update the last node
//     } while (swapped);

// }
// ###################### End of Bubble Sort ##################################

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
 // https://leetcode.com/problems/remove-nodes-from-linked-list/
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;
    //SO C90 forbids variable length array ‘stack’
    struct list_head* stack[2000];
    // struct list_head dummy = {NULL, NULL};
    // stack[0] = &dummy;
    stack[0] = head;
    int top = 0;
    struct list_head* cur = head->next;

    while (cur!=head) {

        while (top > 0 && strcmp(list_entry(cur, element_t, list)->value, list_entry(stack[top], element_t, list)->value)<0 ){
            element_t *elem = list_entry(stack[top], element_t, list);
            q_release_element(elem);
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
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;
    struct list_head* stack[1000];
    // struct list_head dummy = {NULL, NULL};
    // stack[0] = &dummy;
    stack[0] = head;
    int top = 0;
    struct list_head* cur = head->next;
    struct list_head* curnext;

    while (cur!=head) {

        while (top > 0 && strcmp(list_entry(cur, element_t, list)->value, list_entry(stack[top], element_t, list)->value)>0 ){
            element_t *elem = list_entry(stack[top], element_t, list);
            q_release_element(elem);
            top --;
        }
        curnext = cur->next; 
        cur->prev = stack[top];
        stack[top]->next = cur;
        top ++;
        stack[top] = cur;
        cur = curnext;
    }
    stack[top]->next = head;
    head->prev = stack[top];
    return q_size(head);
}





/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend) {
    int size = q_size(head);
    if (size == 0) return 0;
    if (size == 1) return q_size(list_entry(head->next, queue_contex_t, chain)->q); 
    
    struct list_head *last = head->prev;
    struct list_head *start = head->next;
    // struct list_head *startnext;
    // struct list_head *tempprev;
    struct list_head *temp = last;
    // int last = size;
    while (last != head->next){
        start = head->next;
        temp = last;
        queue_contex_t* startqueue = list_entry(start, queue_contex_t, chain);
        queue_contex_t* tempqueue = list_entry(temp, queue_contex_t, chain);


        while ( start != temp && start->prev!=temp) {
            startqueue->q = merge_two_lists(startqueue->q, tempqueue->q, descend);
            start = start->next;
            temp = temp->prev;
            if ( start == temp || start->prev == temp) {
                last = temp;
            }
        }
    }
    
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}


