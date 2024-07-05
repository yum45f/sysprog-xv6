#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void main()
{
  if (cpuid() == 0)
  {
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");

    kinit(); // physical page allocator
    printf("kinit done\n");

    kvminit(); // create kernel page table
    printf("kvminit done\n");

    kvminithart(); // turn on paging
    printf("kvminithart done\n");

    procinit(); // process table
    printf("procinit done\n");

    trapinit(); // trap vectors
    printf("trapinit done\n");

    trapinithart(); // install kernel trap vector
    printf("trapinithart done\n");

    plicinit(); // set up interrupt controller
    printf("plicinit done\n");

    plicinithart(); // ask PLIC for device interrupts
    printf("plicinithart done\n");

    binit(); // buffer cache
    printf("binit done\n");

    iinit(); // inode table
    printf("iinit done\n");

    fileinit(); // file table
    printf("fileinit done\n");

    virtio_disk_init(); // emulated hard disk
    printf("virtio_disk_init done\n");

    userinit(); // first user process
    __sync_synchronize();
    started = 1;
    printf("CPU 0 initialization complete\n");
  }
  else
  {
    while (started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  scheduler();
}
