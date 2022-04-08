#include <kernelGlobal.h>

stivale2_tag *Kernel::stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (stivale2_tag *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == nullptr) {
            return nullptr;
        }

        if (current_tag->identifier == id) {
            return current_tag;
        }

        current_tag = (stivale2_tag *)current_tag->next;
    }
}
