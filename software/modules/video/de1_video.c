#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <cobalt/kernel/rtdm/driver.h>


#define PHYS_ADDR           0xFF200000
#define C_RAM_DEPTH         0x80000
#define C_NB_PIXEL_IMAGE    0x12C00

MODULE_LICENSE("GPL");
MODULE_AUTHOR("reds");
MODULE_DESCRIPTION("de1 video module");

static void __iomem *mapped_address;

static int __init video_init(void)
{
    int i = 0;

    pr_info("Hello module init\n");

    // Map the physical address to a virtual address
    mapped_address = ioremap(PHYS_ADDR, C_RAM_DEPTH);

    if (!mapped_address) {
        pr_err("Failed to ioremap\n");
        return -ENOMEM;
    }

    for(i = 0; i < C_NB_PIXEL_IMAGE; i++){
        *((int*)(mapped_address) + i) = 0xe82a1a;
    }

    return 0; // Non-zero return means that the module couldn't be loaded.
}

static void __exit video_exit(void)
{
    pr_info("Goodbye module exit\n");
}

module_init(video_init);
module_exit(video_exit);
