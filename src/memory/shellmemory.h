// Compile flag fallbacks
#ifndef FRAME_SIZE
// We use a frame size of 3 rather than a power of 2 to test interesting cases
// without having to implement more scheduling policies
#define FRAME_SIZE 3
#endif

#ifndef FRAME_STORE_SIZE
#define FRAME_STORE_SIZE 300
#endif

#ifndef VAR_STORE_SIZE
#define VAR_STORE_SIZE 10
#endif

// forward declaration -- signal existene of pcb struct type
struct PCB;

void mem_init();
int find_available_frame();
void mem_set_value(char* value_in, int address);
char* mem_get_value(int address);
void load_page_into_frame(struct PCB* pcb, int page, int frame);
void free_memory_frame(int frame);
void set_var_value(char* var_in, char* value_in);
char* get_var_value(char* var_in);
