/*
 * Minimal driver for ALVA Nanoface USB audio interface. This driver does not
 * support USB audio, but enables other audio I/O connections on the device.
 *
 * Copyright (C) 2014 Lauri Niskanen (ape@ape3000.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

static unsigned char init_setup[] = {0x01, 0x0b, 0x00, 0x00,
				     0x01, 0x00, 0x00, 0x00};
static unsigned char init_data[] = {};

static void init_complete_callback(struct urb *request)
{
	printk(KERN_INFO "ALVA Nanoface initialized\n");
	usb_free_urb(request);
}

static int nanoface_probe(struct usb_interface *interface,
			  const struct usb_device_id *id)
{
	int status;
	struct usb_device *dev;
	struct urb *init_request;

	init_request = usb_alloc_urb(0, 0);
	if (init_request == 0)
	{
		printk(KERN_INFO "ALVA Nanoface initialization failed: Cannot allocate memory for URB request\n");
	}
	else
	{
		printk(KERN_INFO "ALVA Nanoface (%04X:%04X) connected\n",
		       id->idVendor, id->idProduct);

		dev = interface_to_usbdev(interface);
		usb_fill_control_urb(init_request, dev,
				     usb_sndctrlpipe(dev, 0), init_setup,
				     init_data, sizeof(init_data),
				     init_complete_callback, 0);

		status = usb_submit_urb(init_request, 0);
		if (status != 0)
		{
			printk(KERN_INFO "ALVA Nanoface initialization failed: Error %d when submitting URB\n",
			       status);
			return status;
		}
	}

	// Do not manage the device
	return -ENODEV;
}

static void nanoface_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "ALVA Nanoface disconnected\n");
}
 
static struct usb_device_id nanoface_table[] =
{
	{ USB_DEVICE(0x0a4a, 0xaffe) },
	{ }
};

MODULE_DEVICE_TABLE (usb, nanoface_table);
 
static struct usb_driver nanoface_driver =
{
	.name = "nanoface",
	.id_table = nanoface_table,
	.probe = nanoface_probe,
	.disconnect = nanoface_disconnect,
};
 
static int __init nanoface_init(void)
{
	return usb_register(&nanoface_driver);
}
 
static void __exit nanoface_exit(void)
{
	usb_deregister(&nanoface_driver);
}
 
module_init(nanoface_init);
module_exit(nanoface_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lauri Niskanen <ape@ape3000.com>");
MODULE_DESCRIPTION("ALVA Nanoface USB driver");
