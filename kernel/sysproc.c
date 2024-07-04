#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_map_shared_pages(void)
{
  // map_shared_pages(struct proc* src_proc, struct proc* dst_proc, uint64 src_va, uint64 size)
  int pid_src;
  int pid_dst;
  uint64 src_va;
  int size;

  struct proc* src_proc;
  struct proc* dst_proc;

  argint(0, &pid_src);
  argint(1, &pid_dst);

  src_proc = find_proc(pid_src);
  dst_proc = find_proc(pid_dst);

  argaddr(2, &src_va);
  argint(3, &size);

  return map_shared_pages(src_proc, dst_proc, src_va, size);
}

uint64
sys_unmap_shared_pages(void)
{
  //unmap_shared_pages(struct proc* p, uint64 va, uint64 num_pages)
  int pid;
  uint64 va;
  uint64 num_pages;

  struct proc* p;

  argint(0, &pid);
  p = find_proc(pid);

  argaddr(1, &va);
  argaddr(2, &num_pages);

  return unmap_shared_pages(p, va, num_pages);
}

int
sys_memsize(void)
{
  return myproc()->sz;
}



