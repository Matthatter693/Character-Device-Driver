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
sudo cp udev-rules/99-cbufdev.rules /etc/udev/rules.d/
```

V
