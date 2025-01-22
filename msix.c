#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

struct msix_pci_device {
    struct pci_dev* 		pdev;
    int 			num_ctx;
    int                         msix_bar;
    int                         msix_size;
    int                         msix_offset;
};

static int msix_pci_probe(struct pci_dev* pdev, const struct pci_device_id* id) {
    printk("msix pci probe\n");
    struct msix_pci_device* vdev;
    if (pdev->hdr_type != PCI_HEADER_TYPE_NORMAL) {
        return -EINVAL;
    }
    vdev = kzalloc(sizeof(*vdev), GFP_KERNEL);
    if (!vdev) {
        return -ENOMEM;
    }
    vdev->pdev = pdev;
    return 0;
}

static void msix_pci_remove(struct pci_dev* pdev) {
    struct msix_pci_device* vdev;
}

static pci_ers_result_t msix_pci_aer_err_detected(struct pci_dev* pdev, pci_channel_state_t state) {
    struct msix_pci_device* vdev;
    return PCI_ERS_RESULT_CAN_RECOVER;
}

static const struct pci_error_handlers msix_err_handlers = {
    .error_detected = msix_pci_aer_err_detected,
};

static struct pci_driver msix_pci_driver = {
    .name = "msix-pci",
    .id_table = NULL,
    .probe = msix_pci_probe,
    .remove = msix_pci_remove,
    .err_handler = &msix_err_handlers,
};

static int __init msix_init(void) {
    int ret = pci_register_driver(&msix_pci_driver);
    if (ret) {
        printk("msix register pci driver failed %d\n", ret);
        return ret;
    }
    return 0;
}

static void __exit msix_exit(void) {
    pci_unregister_driver(&msix_pci_driver);
}

module_init(msix_init);
module_exit(msix_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("msix interrupt driver kernel module");
