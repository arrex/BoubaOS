#include "lru.h"

#include <stdlib.h>

#include "memory.h"

// get least recently used frame number
// add to lru list

static struct LRUNode* head;
static struct LRUNode* tail;

struct LRUNode* lru_node_init(int frame) {
    struct LRUNode* node = malloc(sizeof(struct LRUNode));

    node->frame = frame;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void lru_init() {
    // init head and tail as dummy nodes
    head = lru_node_init(-1);
    tail = lru_node_init(-1);

    struct LRUNode* prev = head;
    for (int frame = 0; frame < FRAME_STORE_SIZE / FRAME_SIZE; frame++) {
        struct LRUNode* node = lru_node_init(frame);

        // link bidirectionally
        prev->next = node;
        node->prev = prev;

        prev = node;
    }

    // link last node with tail
    prev->next = tail;
    tail->prev = prev;
}

void lru_deinit() {}

/*
 * Returns least recently used frame number
 */
int get_lru_frame() {
    struct LRUNode* lru = tail->prev;
    return lru->frame;
}

/*
 * Accesses a frame number, upating its position in the LRU queue
 */
void access_frame(int frame) {
    struct LRUNode* node = head;

    while (node->frame != frame) {
        node = node->next;
    }

    // get neighbours
    struct LRUNode* left = node->prev;
    struct LRUNode* right = node->next;

    // extract target and seal gap
    left->next = right;
    right->prev = left;

    // put node at head of queue
    left = head;
    right = head->next;

    left->next = node;
    node->next = right;
    right->prev = node;
    node->prev = head;
}
