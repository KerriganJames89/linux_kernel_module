#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>

long (*STUB_start_elevator)(void) = NULL;
EXPORT_SYMBOL(STUB_start_elevator);
SYSCALL_DEFINE0(start_elevator) {
    printk(KERN_DEBUG "inside start_elevator syscall\n");
    if(STUB_start_elevator) {
        return STUB_start_elevator();
    } else {
        return -ENOSYS;
    }
}

long (*STUB_issue_request)(int, int, int) = NULL;
EXPORT_SYMBOL(STUB_issue_request);
SYSCALL_DEFINE3(issue_request, int, start_floor, int, destination_floor, int, type) {
    printk(KERN_DEBUG "inside issue_request syscall\n");
    if(STUB_issue_request) {
        return STUB_issue_request(start_floor, destination_floor, type);
    } else {
        return -ENOSYS;
    }
}

long (*STUB_stop_elevator)(void) = NULL;
EXPORT_SYMBOL(STUB_stop_elevator);
SYSCALL_DEFINE0(stop_elevator) {
    printk(KERN_DEBUG "inside stop_elevator syscall\n");
    if(STUB_stop_elevator) {
        return STUB_stop_elevator();
    } else {
        return -ENOSYS;
    }
}
