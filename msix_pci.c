#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>

static char ids[1024] __initdata;
module_param_string(ids, ids, sizeof(ids), 0);
MODULE_PARM_DESC(ids, "Initial pci ids to add to the msix driver, format is \"vendor:device[:subvendor[:subdevice[:class[:class_mask]]]]\" and multiple comma separated entries can be specified");

struct msix_pci_device {
    struct pci_dev* 		    pdev;
    int 			            num_ctx;
    int                         msix_bar;
    int                         msix_size;
    int                         msix_offset;
};

static int msix_pci_probe(struct pci_dev* pdev, const struct pci_device_id* id) {
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
    printk("msix pci aer err handler\n");
    struct msix_pci_device* vdev;
    return PCI_ERS_RESULT_CAN_RECOVER;
}

static const struct pci_error_handlers msix_err_handlers = {
    .error_detected = msix_pci_aer_err_detected,
};

// pci driver
static struct pci_driver msix_pci_driver = {
    .name = "msix-pci",
    .id_table = NULL,
    .probe = msix_pci_probe,
    .remove = msix_pci_remove,
    .err_handler = &msix_err_handlers,
};

static void __init msix_pci_fill_ids(void) {
    char* p;
    char* id;
    int rc;
    if (ids[0] == '\0') {
        return;
    }
    p = ids;
    while ((id = strsep(&p, ","))) {
        unsigned int vendor, device, subvendor = PCI_ANY_ID,
            subdevice = PCI_ANY_ID, class = 0, class_mask = 0;
        int fields;
        if (!strlen(id)) continue;

        fields = sscanf(id, "%x:%x:%x:%x:%x:%x",
                        &vendor, &device, &subvendor, &subdevice,
                        &class, &class_mask);

        if (fields < 2) {
            printk("msix invalid id string %s\n", id);
            continue;
        }

        rc = pci_add_dynid(&msix_pci_driver, vendor, device, subvendor, subdevice, class, class_mask, 0);
        if (rc) {
            printk("msix failed to add dynamic id [%04x:%04x[%04x:%04x]] class %#08x/%08x (%d)\n",
                vendor, device, subvendor, subdevice, class, class_mask, rc);
        } else {
            printk("msix add [%04x:%04x[%04x:%04x]] class %#08x/%08x\n",
                vendor, device, subvendor, subdevice, class, class_mask);
        }

    }
}

static int __init msix_pci_init(void) {
    int ret = pci_register_driver(&msix_pci_driver);
    if (ret) {
        printk("msix register pci driver failed %d\n", ret);
        return ret;
    }
    msix_pci_fill_ids();
    return 0;
}

static void __exit msix_pci_exit(void) {
    printk("msix driver exit\n");
    pci_unregister_driver(&msix_pci_driver);
}

module_init(msix_pci_init);
module_exit(msix_pci_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("msix pci driver kernel module");