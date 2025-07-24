# KERNEL LOG DEVICE USING RING BUFFER

Here is a implementation of a kernel events logger based on the ring buffer. This is character device driver ran and tested in the linux kernel version **[6.8.0-64-generic]** with it's headers.
It is a loadable kernel module that can be loaded at run-time.

---

## STRUCTURE

The file structure of the repository as follows:

```
├── driver
│   └── cbufdev.c
├── include
│   └── kernhead.h
├── LICENSE
├── Makefile
├── README.md
├── udev-rules
│   └── 99-cbufdev.rules
└── user_prog
    └── main.c
```

---

## INSTRUCTIONS

- Run anyone of the following snippet of your choice to clone the repo from the github

```
$ git clone https://github.com/Matthatter693/Circular-Buffer-Device-Driver.git //Through HTTPS
$ git clone git@github.com:Matthatter693/Circular-Buffer-Device-Driver.git     //Through SSH
$ gh repo clone Matthatter693/Circular-Buffer-Device-Driver                    //Through Github-Cli
```

### KERNEL SIDE

- On kernel side to compile the driver run the following make command

```
$ make all
```

**NOTE**: This following makefile compiles with the headers of the host's kernel version. If you are compiling it for the out-of-tree deployment make sure to adjust the path in the makefile accordingly.

- After running you will be finding the object files and module files of the corresponding driver,verify with
  
```
$ ls driver/
  cbufdev.c  cbufdev.ko  cbufdev.mod  cbufdev.mod.c  cbufdev.mod.o  cbufdev.o
```

- Then run this snippet to apply the udev rules

```
$ sudo cp udev-rules/99-cbufdev.rules /etc/udev/rules.d/
```

- Change into driver directory and load the kernel driver into the kernel

```
$ cd drivers
$ sudo insmod cbufdev.ko
```

- Later verify whether the module is loaded with appropriate permissions

```
$lsmod | grep -i "cbufdev"     
cbufdev                12288  0

$ udevadm info -a -p /sys/class/cbuf/cbuf-0      

Udevadm info starts with the device specified by the devpath and then
walks up the chain of parent devices. It prints for every device
found, all possible attributes in the udev rules key format.
A rule to match, can be composed by the attributes of the device
and the attributes from one single parent device.

  looking at device '/devices/virtual/cbuf/cbuf-0':
    KERNEL=="cbuf-0"
    SUBSYSTEM=="cbuf"
    DRIVER==""
    ATTR{power/async}=="disabled"
    ATTR{power/control}=="auto"
    ATTR{power/runtime_active_kids}=="0"
    ATTR{power/runtime_active_time}=="0"
    ATTR{power/runtime_enabled}=="disabled"
    ATTR{power/runtime_status}=="unsupported"
    ATTR{power/runtime_suspended_time}=="0"
    ATTR{power/runtime_usage}=="0"

$ ls -lrta /dev/cbuf-0
crw-rw-rw- 1 root root 510, 0 Jul 24 21:21 /dev/cbuf-0
```

- Additionally use dmesg to look at the kernel's debug log

```
[31696.605262] Entering: cdev_release
[31704.484815] Entering: cdev_open
[31713.171044] Entering: cdev_release
[31766.870587] Entering:cleanup_module
[31864.173802] Entering: init_module
[31864.173810] device major number is:510
[31880.838509] Entering: cdev_open
[31880.838520] Entering cdev_write
[31880.838524] copied 12 bytes from the user
[31880.838526] Receive data from user:Hiii buddy!
[31880.838534] Entering cdev_write
[31880.838537] copied 13 bytes from the user
[31880.838539] Receive data from user:how are you?
[31880.838544] Entering: cdev_read
[31880.838548] User data len: 11
[31880.838553] Entering: cdev_read
[31880.838555] User data len: 12
[31880.838561] Entering: cdev_release
[31886.336635] Entering: cdev_release
[33100.442866] Entering:cleanup_module

```
It concludes that the module is loaded successfully and you can now successfully run any program that can use this driver to store log messages.

---

### USER SPACE SIDE

- For this compile the main program present in the user_prog
    
```
gcc main.c -o main
```

- This program writes and reads the string twice so the output will be more like

```
$ ./main
str1:Hiii buddy!
str2:how are you?
message sent
message sent
Read from device:Hiii buddy!
Read from device:how are you?
```
---
## CLEANUP

- After every work is done you can unload the module with rmmod command

```
sudo rmmod cbufdev.ko
```
