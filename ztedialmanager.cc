
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <signal.h>
#include <libudev.h>
#include <sys/wait.h>

#include "dispatchmanager.h"
#include "signalistener.h"
#include "log.h"
#include "ztedial.h"
#include "ztedialmanager.h"
#include "ztenetlink.h"

ZteDialManager::ZteDialManager()
{
    DispatchManager::Instantiate();

    scan_device();
}

ZteDialManager::~ZteDialManager()
{
    DispatchManager::Dispose();
}

void ZteDialManager::scan_device()
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev, *parent, *interface;

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        Log::Error("Can't create udev");
        return;
    }

    /* Create a list of the devices in the 'tty' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    /* For each item enumrated, match the ZTE 4G LTE modem */
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        path = udev_list_entry_get_name(dev_list_entry);

        dev = udev_device_new_from_syspath(udev, path);

        parent = udev_device_get_parent_with_subsystem_devtype(
                dev,
                "usb",
                "usb_device");

        if (parent) {
            /* get idVendor/idProduct */
            int idVendor = strtoul(udev_device_get_sysattr_value(parent, "idVendor"), 0, 16);
            int idProduct = strtoul(udev_device_get_sysattr_value(parent, "idProduct"), 0, 16);

            if (idVendor != 0x19d2 && idProduct != 0x0199) {
                udev_device_unref(dev);
                continue;
            }
        }

        interface = udev_device_get_parent_with_subsystem_devtype(
                    dev,
                    "usb",
                    "usb_interface");

        if (interface) {
            const char *intf =
                udev_device_get_sysattr_value(interface, "interface");
            if (!strncmp(intf, "USB-AT", 6)) {
                char message[64];
                snprintf(message, sizeof(message),
                        "ACTION=add, DEVNODE=%s",
                        udev_device_get_devnode(dev));
                Log::Info("%s", message);
                DispatchManager::Post(DispatchManager::main_id, message);
            }
        }

        udev_device_unref(dev);
    }

    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);
}

int ZteDialManager::main_loop_run()
{
    SignalListener* signaler;
    ZteNetlink* netlink;

    signaler = new SignalListener();
    signaler->watch(SIGTERM);
    signaler->watch(SIGINT);
    signaler->watch(SIGHUP);
    signaler->watch(SIGCHLD);

    netlink = ZteNetlink::open();
    netlink->start();

    for (;;) {
        char message[1024];
        int n = DispatchManager::Get(DispatchManager::main_id, message, sizeof(message));
        if (n < 0)
            break;
        if (!n)
            continue;
        char *p = message;
        if (!strncmp(p, "ACTION=", 7)) {
            p += 7;
            if (!strncmp(p, "exit", 4))
                break;
            else if (!strncmp(p, "add", 3)) {
                p = strstr(p + 3, "DEVNODE=");
                if (!p) continue;
                p += 8;
                if (fork() == 0) {
                    // reset the signal table
                    signaler->clean();

                    // reinitialize dispatchmanager context
                    DispatchManager::Dispose();
                    DispatchManager::Instantiate();

                    // reinitialize signal handler
                    signaler->watch(SIGTERM);
                    signaler->watch(SIGINT);
                    signaler->watch(SIGHUP);

                    // now run the dialer
                    ZteDial::dial(p);
                    break;
                }
            } else if (!strncmp(p, "waitpid", 7)) {
                int status;
                while (waitpid(-1, &status, WNOHANG) > 0);
            }
        }
    }

    delete signaler;
    delete netlink;

    return 0;
}

