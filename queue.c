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
            while (cur->next && strcmp(list_entry(cur, element_t, list)->value, list_entry(cur->next, element_t, list)->value) == 0) {
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


// struct list_head* merge_two(struct list_head* left, struct list_head* right, bool descend) {
//     if (!left) return right;
//     if (!right) return left;

//     struct list_head dummy; // 虛擬頭節點
//     struct list_head* temp = &dummy;

//     while (left && right) {
//         if (descend
//                 ? (strcmp(list_entry(left, element_t, list)->value, list_entry(right, element_t, list)->value) >= 0)
//                 : (strcmp(list_entry(left, element_t, list)->value, list_entry(right, element_t, list)->value) <= 0)) {
//             temp->next = left;
//             left->prev = temp;
//             left = left->next;
//         } else {
//             temp->next = right;
//             right->prev = temp;
//             right = right->next;
//         }
//         temp = temp->next;
//     }

//     temp->next = left ? left : right;
//     if (temp->next) {
//         temp->next->prev = temp;
//     }

//     return dummy.next;
// }
// struct list_head* sort_helper(struct list_head *head, bool descend) {
//     if (!head || !head->next || head->next == head)
//         return head;

//     // 使用快慢指標找中點
//     struct list_head *slow = head, *fast = head->next;
//     while (fast != head && fast->next != head) {
//         slow = slow->next;
//         fast = fast->next->next;
//     }

//     struct list_head *right = slow->next;
//     slow->next = head;       // 左邊結束，回到 head
//     head->prev = slow;       // 修正左邊尾節點的 prev

//     right->prev = head;      // 右邊頭節點修正 prev
//     head->next = right;      // 右邊鏈結起點

//     struct list_head *left_sorted = sort_helper(head, descend);
//     struct list_head *right_sorted = sort_helper(right, descend);

//     return merge_two(left_sorted, right_sorted, descend);
// }


// void q_sort(struct list_head *head, bool descend) {
//     if (!head || list_empty(head) || list_is_singular(head))
//         return;

//     // 分割鏈結串列，然後排序
//     struct list_head *sorted = sort_helper(head->next, descend);

//     // 修正 head 與尾部鏈結，保持循環
//     head->next = sorted;
//     sorted->prev = head;

//     struct list_head *tail = sorted;
//     while (tail->next != head) {
//         tail = tail->next;
//     }
//     head->prev = tail;
//     tail->next = head;
// }

void swap(element_t* a, element_t* b) {
    char *temp = a->value;
    a->value = b->value;
    b->value = temp;
}

void q_sort(struct list_head *head, bool descend) {
    if (list_empty(head) || list_is_singular(head)) return; // Empty or single node list
    
    int swapped;
    struct list_head *current = NULL;

    do {
        swapped = 0;
        current = head->next; // Start from the first node

        while (current->next != head) { // Loop until the last node
            element_t* curelem = list_entry(current, element_t, list);
            element_t* nextelem = list_entry(current->next, element_t, list);
            if (strcmp(curelem->value, nextelem->value) > 0) {
                swap(curelem, nextelem);
                swapped = 1;
            }
            current = current->next;
        }
        //last = current; // Update the last node
    } while (swapped);

}

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


// element_t *mergeq(element_t *list1, element_t *list2, bool descend) {
//     if (!list1) return list2;
//     if (!list2) return list1;

//     element_t *result;

//     if ((descend ? strcmp(list1->value, list2->value) >= 0
//                  : strcmp(list1->value, list2->value) <= 0)) {
//         result = list1;
//         result->list.next = &mergeq(list_entry(list1->list.next, element_t, list), list2, descend)->list;
//     } else {
//         result = list2;
//         result->list.next = &mergeq(list1, list_entry(list2->list.next, element_t, list), descend)->list;
//     }
//     return result;
// }

// // https://leetcode.com/problems/merge-k-sorted-lists/
// int q_merge(struct list_head *head, bool descend) {
//     if (!head || list_empty(head) || list_is_singular(head))
//         return 0;

//     struct list_head *node;
//     struct list_head *temp;

//     // Traverse the chain of queues and merge them pair by pair
//     struct list_head *first_queue = NULL;
//     struct list_head *second_queue = NULL;

//     list_for_each_safe(node, temp, head) {
//         queue_contex_t *context1 = list_entry(node, queue_contex_t, chain);

//         if (!first_queue) {
//             first_queue = context1->q;
//         } else {
//             second_queue = context1->q;

//             // Merge two queues
//             element_t *merged_head = NULL;
//             element_t *e1, *e2;

//             e1 = list_first_entry(first_queue, element_t, list);
//             e2 = list_first_entry(second_queue, element_t, list);

//             merged_head = mergeq(e1, e2, descend);

//             INIT_LIST_HEAD(first_queue);
//             struct list_head *merged_list = &merged_head->list;

//             // Add merged nodes back into the first queue
//             struct list_head *current = merged_list;
//             while (current) {
//                 element_t *entry = list_entry(current, element_t, list);
//                 list_add_tail(&entry->list, first_queue);
//                 current = current->next;
//             }

//             INIT_LIST_HEAD(second_queue);
//             second_queue = NULL;
//         }
//     }

//     return q_size(first_queue);
// }

struct list_head* mergehelper (struct list_head* list1, struct list_head* list2) {
    struct list_head* result;
    struct list_head* temp;
    
    if (strcmp(list_entry(list1, element_t, list)->value, list_entry(list2, element_t, list)->value) <= 0) {
        result = list1;
        temp = mergehelper(list1->next, list2);
        result->next = temp;
        temp->prev = result;
    } 
    else {
        result = list2;
        temp = mergehelper(list1, list2->next);
        result->next = temp; 
        temp->prev = result;  
    }
    return result;
}

struct list_head* merge (struct list_head* list1, struct list_head* list2) {
    if (list_empty(list1)) return list2;
    if (list_empty(list2)) return list1;
    

    struct list_head* list1next = mergehelper(list1->next, list2->next);
    list1->next = list1next;
    list1next->prev = list1;
    
    struct list_head* temp = list1;
    while (temp->next != head) {
        temp = temp->next;
    }
    temp->next = list1;
    list1->prev = temp;
    // struct list_head* result;
    // if (list1->val <= list2->val) {
    //     result = list1;
    //     result->next = merge(list1->next, list2);
    // } 
    // else {
    //     result = list2;
    //     result->next = merge(list1, list2->next);   
    // }
    // return result;
    return list1;
}

int q_merge(struct list_head *head, bool descend) {
    int size = q_size(head);
    if (size == 0) return 0;
    if (size == 1) return q_size(list_entry(head->next, queue_contex_t, chain)->q); 
    
    struct list_head *last = head->prev;
    struct list_head *start = head;
    struct list_head *temp = last;
    // int last = size;
    while (last != head){
        start = head;
        temp = last;
        while (start->next != temp && start != temp) {
            start = merge(list_entry(start, queue_contex_t, chain)->q, list_entry(head->next, queue_contex_t, chain)->q);
            start = start->next;
            temp = temp->prev;
            if ( start == temp || start->prev == temp) {
                last = temp;
            }
        }
    }
    
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}
