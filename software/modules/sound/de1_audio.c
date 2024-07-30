#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

#include <cobalt/kernel/rtdm/driver.h>

#define AUD_CORE_BASE   0xff280000
#define AUD_CORE_SIZE   0x10
#define AUD_CTRLS_SIZE  0xb0

#define AUD_CORE_CTRL   0x0
#define AUD_CORE_FIFO   0x4
#define AUD_CORE_LEFT   0x8
#define AUD_CORE_RIGHT  0xC

#define AUD_CORE_WR_LEFT_BITS  24
#define AUD_CORE_WR_RIGHT_BITS 16
#define AUD_CORE_RD_LEFT_BITS  8
#define AUD_CORE_RD_RIGHT_BITS 0

#define BUF_WORD_CAPACITY   128UL

struct snd_data {
    void __iomem *reg;
    u16           sndbuf_out[BUF_WORD_CAPACITY*2]; // Interleaved left and right data, output
    u16           sndbuf_in[BUF_WORD_CAPACITY*2]; // Interleaved left and right data, input
};

static struct snd_data data;

ssize_t snd_read(struct rtdm_fd *fd, void __user *buf, size_t size)
{
    u32 fifospace;
    u32 fifo_left;
    u32 fifo_right;
    unsigned long num_stereo_samples; // num of stereo pairs to copy
    unsigned long not_copied;
    ssize_t read = 0;
    size_t i;

    fifospace = ioread32(data.reg + AUD_CORE_FIFO);
    fifo_left = (fifospace >> AUD_CORE_RD_LEFT_BITS) & 0xFF;
    fifo_right = (fifospace >> AUD_CORE_RD_RIGHT_BITS) & 0xFF;

    num_stereo_samples = min(size / (sizeof(u16) * 2), (size_t)BUF_WORD_CAPACITY);
    /* Let's suppose fifo left and fifo right are always equal */
    num_stereo_samples = min((unsigned long)fifo_left, num_stereo_samples);

    for (i = 0; i < num_stereo_samples; i++) {
        u32 left_sample     = ioread32(data.reg + AUD_CORE_LEFT);
        u32 right_sample    = ioread32(data.reg + AUD_CORE_RIGHT);

        data.sndbuf_in[i*2 + 0] = left_sample;
        data.sndbuf_in[i*2 + 1] = right_sample;

        read += (sizeof(u16) * 2);
    } 

    not_copied = copy_to_user(buf, data.sndbuf_in, read);
    if (not_copied > 0)
        return -EFAULT;

    return read;
}

ssize_t snd_write(struct rtdm_fd *fd, const void __user *buf, size_t size)
{
    u32 fifospace;
    u32 fifo_left;
    u32 fifo_right;
    u32 datas; // number of stereo samples
    unsigned long to_copy;
    unsigned long not_copied;
    ssize_t written = 0;
    size_t i;

    to_copy = min(sizeof(data.sndbuf_out), size);
    not_copied = copy_from_user(data.sndbuf_out, buf, to_copy);
    if (not_copied > 0)
        return -EFAULT;

    datas = to_copy / (sizeof(u16) * 2);

    fifospace = ioread32(data.reg + AUD_CORE_FIFO);
    fifo_left = (fifospace >> AUD_CORE_WR_LEFT_BITS) & 0xFF;
    fifo_right = (fifospace >> AUD_CORE_WR_RIGHT_BITS) & 0xFF;

    /* Write left and right channels */
    for (i = 0; i < min(datas, fifo_right)*2; i += 2) {
        u32 left_sample = (u32)(data.sndbuf_out[i]);
        u32 right_sample = (u32)(data.sndbuf_out[i+1]);

        iowrite32(left_sample,  data.reg + AUD_CORE_LEFT);
        iowrite32(right_sample, data.reg + AUD_CORE_RIGHT);

        written += 2; // In stereo, two samples are written at once
    }

    return written * sizeof(u16);
}

static const struct vm_operations_struct snd_physical_vm_ops = {
#ifdef CONFIG_HAVE_IOREMAP_PROT
    .access = generic_access_phys,
#endif
};

static int
user_io_mmap(struct rtdm_fd *fd, struct vm_area_struct *vma)
{
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, AUD_CORE_BASE >> PAGE_SHIFT,
                        AUD_CTRLS_SIZE, vma->vm_page_prot)) {
        return -EAGAIN;
    }

    return 0;
}

static struct rtdm_driver snd_driver = {
    .profile_info = RTDM_PROFILE_INFO(snd,
                                      RTDM_CLASS_MISC,
                                      1, // subclass id
                                      1),
    .device_flags = RTDM_NAMED_DEVICE,
    .device_count = 1,
    .ops          = {
        .read_rt    = snd_read,
        .write_rt   = snd_write,
        .mmap       = user_io_mmap,
    }
};

static struct rtdm_device snd_device = {
    .driver = &snd_driver,
    .label  = "snd",
    //.device_data = data,
};

static __init int snd_init(void)
{
    int ret = -1;

    data.reg = ioremap_nocache(AUD_CORE_BASE, AUD_CTRLS_SIZE);
    if (!data.reg) {
        pr_err("ioremap failed");
        goto fail;
    }

    ret = rtdm_dev_register(&snd_device);
    if (ret) {
        pr_err("rtdm_dev_register failed");
        goto fail;
    }

    pr_info("Registered snd device");

    return 0;

fail:
    return ret;
}

static __exit void snd_exit(void)
{
    rtdm_dev_unregister(&snd_device);
    iounmap(data.reg);

    pr_info("Unregistered snd device");
}

module_init(snd_init);
module_exit(snd_exit);

MODULE_VERSION("0.1");
MODULE_AUTHOR("Sydney Hauke, REDS Institute");
MODULE_DESCRIPTION("RT sound driver for the DE1-SoC");
MODULE_LICENSE("GPL");
