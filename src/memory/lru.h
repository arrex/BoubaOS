typedef struct LRUNode {
    int frame;
    struct LRUNode* prev;
    struct LRUNode* next;
} LRUNode;

void lru_init();
void lru_deinit();
int get_lru_frame();
void access_frame(int frame);
