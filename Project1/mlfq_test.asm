
_mlfq_test:     file format elf32-i386


Disassembly of section .text:

00000000 <main>:
    exit();
  while (wait() != -1);
}

int main(int argc, char *argv[])
{
   0:	8d 4c 24 04          	lea    0x4(%esp),%ecx
   4:	83 e4 f0             	and    $0xfffffff0,%esp
   7:	ff 71 fc             	push   -0x4(%ecx)
   a:	55                   	push   %ebp
   b:	89 e5                	mov    %esp,%ebp
   d:	56                   	push   %esi
   e:	53                   	push   %ebx
   f:	51                   	push   %ecx
  10:	83 ec 2c             	sub    $0x2c,%esp
  int i, pid;
  int count[MAX_LEVEL] = {0};
  13:	c7 45 d4 00 00 00 00 	movl   $0x0,-0x2c(%ebp)
  1a:	c7 45 d8 00 00 00 00 	movl   $0x0,-0x28(%ebp)
  21:	c7 45 dc 00 00 00 00 	movl   $0x0,-0x24(%ebp)
  28:	c7 45 e0 00 00 00 00 	movl   $0x0,-0x20(%ebp)
  2f:	c7 45 e4 00 00 00 00 	movl   $0x0,-0x1c(%ebp)
//  int child;

  parent = getpid();
  36:	e8 c8 04 00 00       	call   503 <getpid>

  printf(1, "MLFQ test start\n");
  3b:	83 ec 08             	sub    $0x8,%esp
  3e:	68 51 09 00 00       	push   $0x951
  43:	6a 01                	push   $0x1
  parent = getpid();
  45:	a3 74 0d 00 00       	mov    %eax,0xd74
  printf(1, "MLFQ test start\n");
  4a:	e8 c1 05 00 00       	call   610 <printf>

  printf(1, "[Test 1] default\n");
  4f:	5e                   	pop    %esi
  50:	58                   	pop    %eax
  51:	68 62 09 00 00       	push   $0x962
  56:	6a 01                	push   $0x1
  58:	e8 b3 05 00 00       	call   610 <printf>
  pid = fork_children();
  5d:	e8 9e 00 00 00       	call   100 <fork_children>

  if (pid != parent)
  62:	83 c4 10             	add    $0x10,%esp
  65:	39 05 74 0d 00 00    	cmp    %eax,0xd74
  6b:	74 6b                	je     d8 <main+0xd8>
  6d:	89 c6                	mov    %eax,%esi
  6f:	bb a0 86 01 00       	mov    $0x186a0,%ebx
  74:	eb 14                	jmp    8a <main+0x8a>
  76:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  7d:	8d 76 00             	lea    0x0(%esi),%esi
      if (x < 0 || x > 4)
      {
        printf(1, "Wrong level: %d\n", x);
        exit();
      }
      count[x]++;
  80:	83 44 85 d4 01       	addl   $0x1,-0x2c(%ebp,%eax,4)
    for (i = 0; i < NUM_LOOP; i++)
  85:	83 eb 01             	sub    $0x1,%ebx
  88:	74 1f                	je     a9 <main+0xa9>
      int x = getLevel();
  8a:	e8 a4 04 00 00       	call   533 <getLevel>
      if (x < 0 || x > 4)
  8f:	83 f8 04             	cmp    $0x4,%eax
  92:	76 ec                	jbe    80 <main+0x80>
        printf(1, "Wrong level: %d\n", x);
  94:	83 ec 04             	sub    $0x4,%esp
  97:	50                   	push   %eax
  98:	68 74 09 00 00       	push   $0x974
  9d:	6a 01                	push   $0x1
  9f:	e8 6c 05 00 00       	call   610 <printf>
        exit();
  a4:	e8 da 03 00 00       	call   483 <exit>
    }
    printf(1, "Process %d\n", pid);
  a9:	53                   	push   %ebx
    for (i = 0; i < MAX_LEVEL; i++)
  aa:	31 db                	xor    %ebx,%ebx
    printf(1, "Process %d\n", pid);
  ac:	56                   	push   %esi
  ad:	68 85 09 00 00       	push   $0x985
  b2:	6a 01                	push   $0x1
  b4:	e8 57 05 00 00       	call   610 <printf>
  b9:	83 c4 10             	add    $0x10,%esp
      printf(1, "L%d: %d\n", i, count[i]);
  bc:	ff 74 9d d4          	push   -0x2c(%ebp,%ebx,4)
  c0:	53                   	push   %ebx
    for (i = 0; i < MAX_LEVEL; i++)
  c1:	83 c3 01             	add    $0x1,%ebx
      printf(1, "L%d: %d\n", i, count[i]);
  c4:	68 91 09 00 00       	push   $0x991
  c9:	6a 01                	push   $0x1
  cb:	e8 40 05 00 00       	call   610 <printf>
    for (i = 0; i < MAX_LEVEL; i++)
  d0:	83 c4 10             	add    $0x10,%esp
  d3:	83 fb 05             	cmp    $0x5,%ebx
  d6:	75 e4                	jne    bc <main+0xbc>
  }
  exit_children();
  d8:	e8 23 01 00 00       	call   200 <exit_children>
  printf(1, "[Test 1] finished\n");
  dd:	50                   	push   %eax
  de:	50                   	push   %eax
  df:	68 9a 09 00 00       	push   $0x99a
  e4:	6a 01                	push   $0x1
  e6:	e8 25 05 00 00       	call   610 <printf>
  printf(1, "done\n");
  eb:	5a                   	pop    %edx
  ec:	59                   	pop    %ecx
  ed:	68 ad 09 00 00       	push   $0x9ad
  f2:	6a 01                	push   $0x1
  f4:	e8 17 05 00 00       	call   610 <printf>
  exit();
  f9:	e8 85 03 00 00       	call   483 <exit>
  fe:	66 90                	xchg   %ax,%ax

00000100 <fork_children>:
{
 100:	55                   	push   %ebp
 101:	89 e5                	mov    %esp,%ebp
 103:	53                   	push   %ebx
 104:	bb 04 00 00 00       	mov    $0x4,%ebx
 109:	83 ec 04             	sub    $0x4,%esp
    if ((p = fork()) == 0)
 10c:	e8 6a 03 00 00       	call   47b <fork>
 111:	85 c0                	test   %eax,%eax
 113:	74 13                	je     128 <fork_children+0x28>
  for (i = 0; i < NUM_THREAD; i++)
 115:	83 eb 01             	sub    $0x1,%ebx
 118:	75 f2                	jne    10c <fork_children+0xc>
}
 11a:	a1 74 0d 00 00       	mov    0xd74,%eax
 11f:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 122:	c9                   	leave  
 123:	c3                   	ret    
 124:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      sleep(10);
 128:	83 ec 0c             	sub    $0xc,%esp
 12b:	6a 0a                	push   $0xa
 12d:	e8 e1 03 00 00       	call   513 <sleep>
}
 132:	8b 5d fc             	mov    -0x4(%ebp),%ebx
      return getpid();
 135:	83 c4 10             	add    $0x10,%esp
}
 138:	c9                   	leave  
      return getpid();
 139:	e9 c5 03 00 00       	jmp    503 <getpid>
 13e:	66 90                	xchg   %ax,%ax

00000140 <fork_children2>:
{
 140:	55                   	push   %ebp
 141:	89 e5                	mov    %esp,%ebp
 143:	53                   	push   %ebx
  for (i = 0; i < NUM_THREAD; i++)
 144:	31 db                	xor    %ebx,%ebx
{
 146:	83 ec 04             	sub    $0x4,%esp
    if ((p = fork()) == 0)
 149:	e8 2d 03 00 00       	call   47b <fork>
 14e:	85 c0                	test   %eax,%eax
 150:	74 26                	je     178 <fork_children2+0x38>
      int r = setPriority(p, i);
 152:	83 ec 08             	sub    $0x8,%esp
 155:	53                   	push   %ebx
 156:	50                   	push   %eax
 157:	e8 c7 03 00 00       	call   523 <setPriority>
      if (r < 0)
 15c:	83 c4 10             	add    $0x10,%esp
 15f:	85 c0                	test   %eax,%eax
 161:	78 2e                	js     191 <fork_children2+0x51>
  for (i = 0; i < NUM_THREAD; i++)
 163:	83 c3 01             	add    $0x1,%ebx
 166:	83 fb 04             	cmp    $0x4,%ebx
 169:	75 de                	jne    149 <fork_children2+0x9>
}
 16b:	a1 74 0d 00 00       	mov    0xd74,%eax
 170:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 173:	c9                   	leave  
 174:	c3                   	ret    
 175:	8d 76 00             	lea    0x0(%esi),%esi
      sleep(300);
 178:	83 ec 0c             	sub    $0xc,%esp
 17b:	68 2c 01 00 00       	push   $0x12c
 180:	e8 8e 03 00 00       	call   513 <sleep>
}
 185:	8b 5d fc             	mov    -0x4(%ebp),%ebx
      return getpid();
 188:	83 c4 10             	add    $0x10,%esp
}
 18b:	c9                   	leave  
      return getpid();
 18c:	e9 72 03 00 00       	jmp    503 <getpid>
        printf(1, "setpriority returned %d\n", r);
 191:	83 ec 04             	sub    $0x4,%esp
 194:	50                   	push   %eax
 195:	68 38 09 00 00       	push   $0x938
 19a:	6a 01                	push   $0x1
 19c:	e8 6f 04 00 00       	call   610 <printf>
        exit();
 1a1:	e8 dd 02 00 00       	call   483 <exit>
 1a6:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 1ad:	8d 76 00             	lea    0x0(%esi),%esi

000001b0 <fork_children3>:
{
 1b0:	55                   	push   %ebp
 1b1:	89 e5                	mov    %esp,%ebp
 1b3:	53                   	push   %ebx
  for (i = 0; i < NUM_THREAD; i++)
 1b4:	31 db                	xor    %ebx,%ebx
{
 1b6:	83 ec 04             	sub    $0x4,%esp
    if ((p = fork()) == 0)
 1b9:	e8 bd 02 00 00       	call   47b <fork>
 1be:	85 c0                	test   %eax,%eax
 1c0:	74 16                	je     1d8 <fork_children3+0x28>
  for (i = 0; i < NUM_THREAD; i++)
 1c2:	83 c3 01             	add    $0x1,%ebx
 1c5:	83 fb 04             	cmp    $0x4,%ebx
 1c8:	75 ef                	jne    1b9 <fork_children3+0x9>
}
 1ca:	a1 74 0d 00 00       	mov    0xd74,%eax
 1cf:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 1d2:	c9                   	leave  
 1d3:	c3                   	ret    
 1d4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
      sleep(10);
 1d8:	83 ec 0c             	sub    $0xc,%esp
 1db:	6a 0a                	push   $0xa
 1dd:	e8 31 03 00 00       	call   513 <sleep>
      max_level = i;
 1e2:	89 1d 70 0d 00 00    	mov    %ebx,0xd70
      return getpid();
 1e8:	83 c4 10             	add    $0x10,%esp
}
 1eb:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 1ee:	c9                   	leave  
      return getpid();
 1ef:	e9 0f 03 00 00       	jmp    503 <getpid>
 1f4:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 1fb:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 1ff:	90                   	nop

00000200 <exit_children>:
{
 200:	55                   	push   %ebp
 201:	89 e5                	mov    %esp,%ebp
 203:	83 ec 08             	sub    $0x8,%esp
  if (getpid() != parent)
 206:	e8 f8 02 00 00       	call   503 <getpid>
 20b:	3b 05 74 0d 00 00    	cmp    0xd74,%eax
 211:	75 11                	jne    224 <exit_children+0x24>
 213:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 217:	90                   	nop
  while (wait() != -1);
 218:	e8 6e 02 00 00       	call   48b <wait>
 21d:	83 f8 ff             	cmp    $0xffffffff,%eax
 220:	75 f6                	jne    218 <exit_children+0x18>
}
 222:	c9                   	leave  
 223:	c3                   	ret    
    exit();
 224:	e8 5a 02 00 00       	call   483 <exit>
 229:	66 90                	xchg   %ax,%ax
 22b:	66 90                	xchg   %ax,%ax
 22d:	66 90                	xchg   %ax,%ax
 22f:	90                   	nop

00000230 <strcpy>:
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
 230:	55                   	push   %ebp
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 231:	31 c0                	xor    %eax,%eax
{
 233:	89 e5                	mov    %esp,%ebp
 235:	53                   	push   %ebx
 236:	8b 4d 08             	mov    0x8(%ebp),%ecx
 239:	8b 5d 0c             	mov    0xc(%ebp),%ebx
 23c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  while((*s++ = *t++) != 0)
 240:	0f b6 14 03          	movzbl (%ebx,%eax,1),%edx
 244:	88 14 01             	mov    %dl,(%ecx,%eax,1)
 247:	83 c0 01             	add    $0x1,%eax
 24a:	84 d2                	test   %dl,%dl
 24c:	75 f2                	jne    240 <strcpy+0x10>
    ;
  return os;
}
 24e:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 251:	89 c8                	mov    %ecx,%eax
 253:	c9                   	leave  
 254:	c3                   	ret    
 255:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 25c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000260 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 260:	55                   	push   %ebp
 261:	89 e5                	mov    %esp,%ebp
 263:	53                   	push   %ebx
 264:	8b 55 08             	mov    0x8(%ebp),%edx
 267:	8b 4d 0c             	mov    0xc(%ebp),%ecx
  while(*p && *p == *q)
 26a:	0f b6 02             	movzbl (%edx),%eax
 26d:	84 c0                	test   %al,%al
 26f:	75 17                	jne    288 <strcmp+0x28>
 271:	eb 3a                	jmp    2ad <strcmp+0x4d>
 273:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 277:	90                   	nop
 278:	0f b6 42 01          	movzbl 0x1(%edx),%eax
    p++, q++;
 27c:	83 c2 01             	add    $0x1,%edx
 27f:	8d 59 01             	lea    0x1(%ecx),%ebx
  while(*p && *p == *q)
 282:	84 c0                	test   %al,%al
 284:	74 1a                	je     2a0 <strcmp+0x40>
    p++, q++;
 286:	89 d9                	mov    %ebx,%ecx
  while(*p && *p == *q)
 288:	0f b6 19             	movzbl (%ecx),%ebx
 28b:	38 c3                	cmp    %al,%bl
 28d:	74 e9                	je     278 <strcmp+0x18>
  return (uchar)*p - (uchar)*q;
 28f:	29 d8                	sub    %ebx,%eax
}
 291:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 294:	c9                   	leave  
 295:	c3                   	ret    
 296:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 29d:	8d 76 00             	lea    0x0(%esi),%esi
  return (uchar)*p - (uchar)*q;
 2a0:	0f b6 59 01          	movzbl 0x1(%ecx),%ebx
 2a4:	31 c0                	xor    %eax,%eax
 2a6:	29 d8                	sub    %ebx,%eax
}
 2a8:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 2ab:	c9                   	leave  
 2ac:	c3                   	ret    
  return (uchar)*p - (uchar)*q;
 2ad:	0f b6 19             	movzbl (%ecx),%ebx
 2b0:	31 c0                	xor    %eax,%eax
 2b2:	eb db                	jmp    28f <strcmp+0x2f>
 2b4:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 2bb:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 2bf:	90                   	nop

000002c0 <strlen>:

uint
strlen(const char *s)
{
 2c0:	55                   	push   %ebp
 2c1:	89 e5                	mov    %esp,%ebp
 2c3:	8b 55 08             	mov    0x8(%ebp),%edx
  int n;

  for(n = 0; s[n]; n++)
 2c6:	80 3a 00             	cmpb   $0x0,(%edx)
 2c9:	74 15                	je     2e0 <strlen+0x20>
 2cb:	31 c0                	xor    %eax,%eax
 2cd:	8d 76 00             	lea    0x0(%esi),%esi
 2d0:	83 c0 01             	add    $0x1,%eax
 2d3:	80 3c 02 00          	cmpb   $0x0,(%edx,%eax,1)
 2d7:	89 c1                	mov    %eax,%ecx
 2d9:	75 f5                	jne    2d0 <strlen+0x10>
    ;
  return n;
}
 2db:	89 c8                	mov    %ecx,%eax
 2dd:	5d                   	pop    %ebp
 2de:	c3                   	ret    
 2df:	90                   	nop
  for(n = 0; s[n]; n++)
 2e0:	31 c9                	xor    %ecx,%ecx
}
 2e2:	5d                   	pop    %ebp
 2e3:	89 c8                	mov    %ecx,%eax
 2e5:	c3                   	ret    
 2e6:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 2ed:	8d 76 00             	lea    0x0(%esi),%esi

000002f0 <memset>:

void*
memset(void *dst, int c, uint n)
{
 2f0:	55                   	push   %ebp
 2f1:	89 e5                	mov    %esp,%ebp
 2f3:	57                   	push   %edi
 2f4:	8b 55 08             	mov    0x8(%ebp),%edx
}

static inline void
stosb(void *addr, int data, int cnt)
{
  asm volatile("cld; rep stosb" :
 2f7:	8b 4d 10             	mov    0x10(%ebp),%ecx
 2fa:	8b 45 0c             	mov    0xc(%ebp),%eax
 2fd:	89 d7                	mov    %edx,%edi
 2ff:	fc                   	cld    
 300:	f3 aa                	rep stos %al,%es:(%edi)
  stosb(dst, c, n);
  return dst;
}
 302:	8b 7d fc             	mov    -0x4(%ebp),%edi
 305:	89 d0                	mov    %edx,%eax
 307:	c9                   	leave  
 308:	c3                   	ret    
 309:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

00000310 <strchr>:

char*
strchr(const char *s, char c)
{
 310:	55                   	push   %ebp
 311:	89 e5                	mov    %esp,%ebp
 313:	8b 45 08             	mov    0x8(%ebp),%eax
 316:	0f b6 4d 0c          	movzbl 0xc(%ebp),%ecx
  for(; *s; s++)
 31a:	0f b6 10             	movzbl (%eax),%edx
 31d:	84 d2                	test   %dl,%dl
 31f:	75 12                	jne    333 <strchr+0x23>
 321:	eb 1d                	jmp    340 <strchr+0x30>
 323:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 327:	90                   	nop
 328:	0f b6 50 01          	movzbl 0x1(%eax),%edx
 32c:	83 c0 01             	add    $0x1,%eax
 32f:	84 d2                	test   %dl,%dl
 331:	74 0d                	je     340 <strchr+0x30>
    if(*s == c)
 333:	38 d1                	cmp    %dl,%cl
 335:	75 f1                	jne    328 <strchr+0x18>
      return (char*)s;
  return 0;
}
 337:	5d                   	pop    %ebp
 338:	c3                   	ret    
 339:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
  return 0;
 340:	31 c0                	xor    %eax,%eax
}
 342:	5d                   	pop    %ebp
 343:	c3                   	ret    
 344:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 34b:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 34f:	90                   	nop

00000350 <gets>:

char*
gets(char *buf, int max)
{
 350:	55                   	push   %ebp
 351:	89 e5                	mov    %esp,%ebp
 353:	57                   	push   %edi
 354:	56                   	push   %esi
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
 355:	8d 7d e7             	lea    -0x19(%ebp),%edi
{
 358:	53                   	push   %ebx
  for(i=0; i+1 < max; ){
 359:	31 db                	xor    %ebx,%ebx
{
 35b:	83 ec 1c             	sub    $0x1c,%esp
  for(i=0; i+1 < max; ){
 35e:	eb 27                	jmp    387 <gets+0x37>
    cc = read(0, &c, 1);
 360:	83 ec 04             	sub    $0x4,%esp
 363:	6a 01                	push   $0x1
 365:	57                   	push   %edi
 366:	6a 00                	push   $0x0
 368:	e8 2e 01 00 00       	call   49b <read>
    if(cc < 1)
 36d:	83 c4 10             	add    $0x10,%esp
 370:	85 c0                	test   %eax,%eax
 372:	7e 1d                	jle    391 <gets+0x41>
      break;
    buf[i++] = c;
 374:	0f b6 45 e7          	movzbl -0x19(%ebp),%eax
 378:	8b 55 08             	mov    0x8(%ebp),%edx
 37b:	88 44 1a ff          	mov    %al,-0x1(%edx,%ebx,1)
    if(c == '\n' || c == '\r')
 37f:	3c 0a                	cmp    $0xa,%al
 381:	74 1d                	je     3a0 <gets+0x50>
 383:	3c 0d                	cmp    $0xd,%al
 385:	74 19                	je     3a0 <gets+0x50>
  for(i=0; i+1 < max; ){
 387:	89 de                	mov    %ebx,%esi
 389:	83 c3 01             	add    $0x1,%ebx
 38c:	3b 5d 0c             	cmp    0xc(%ebp),%ebx
 38f:	7c cf                	jl     360 <gets+0x10>
      break;
  }
  buf[i] = '\0';
 391:	8b 45 08             	mov    0x8(%ebp),%eax
 394:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
  return buf;
}
 398:	8d 65 f4             	lea    -0xc(%ebp),%esp
 39b:	5b                   	pop    %ebx
 39c:	5e                   	pop    %esi
 39d:	5f                   	pop    %edi
 39e:	5d                   	pop    %ebp
 39f:	c3                   	ret    
  buf[i] = '\0';
 3a0:	8b 45 08             	mov    0x8(%ebp),%eax
 3a3:	89 de                	mov    %ebx,%esi
 3a5:	c6 04 30 00          	movb   $0x0,(%eax,%esi,1)
}
 3a9:	8d 65 f4             	lea    -0xc(%ebp),%esp
 3ac:	5b                   	pop    %ebx
 3ad:	5e                   	pop    %esi
 3ae:	5f                   	pop    %edi
 3af:	5d                   	pop    %ebp
 3b0:	c3                   	ret    
 3b1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 3b8:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 3bf:	90                   	nop

000003c0 <stat>:

int
stat(const char *n, struct stat *st)
{
 3c0:	55                   	push   %ebp
 3c1:	89 e5                	mov    %esp,%ebp
 3c3:	56                   	push   %esi
 3c4:	53                   	push   %ebx
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 3c5:	83 ec 08             	sub    $0x8,%esp
 3c8:	6a 00                	push   $0x0
 3ca:	ff 75 08             	push   0x8(%ebp)
 3cd:	e8 f1 00 00 00       	call   4c3 <open>
  if(fd < 0)
 3d2:	83 c4 10             	add    $0x10,%esp
 3d5:	85 c0                	test   %eax,%eax
 3d7:	78 27                	js     400 <stat+0x40>
    return -1;
  r = fstat(fd, st);
 3d9:	83 ec 08             	sub    $0x8,%esp
 3dc:	ff 75 0c             	push   0xc(%ebp)
 3df:	89 c3                	mov    %eax,%ebx
 3e1:	50                   	push   %eax
 3e2:	e8 f4 00 00 00       	call   4db <fstat>
  close(fd);
 3e7:	89 1c 24             	mov    %ebx,(%esp)
  r = fstat(fd, st);
 3ea:	89 c6                	mov    %eax,%esi
  close(fd);
 3ec:	e8 ba 00 00 00       	call   4ab <close>
  return r;
 3f1:	83 c4 10             	add    $0x10,%esp
}
 3f4:	8d 65 f8             	lea    -0x8(%ebp),%esp
 3f7:	89 f0                	mov    %esi,%eax
 3f9:	5b                   	pop    %ebx
 3fa:	5e                   	pop    %esi
 3fb:	5d                   	pop    %ebp
 3fc:	c3                   	ret    
 3fd:	8d 76 00             	lea    0x0(%esi),%esi
    return -1;
 400:	be ff ff ff ff       	mov    $0xffffffff,%esi
 405:	eb ed                	jmp    3f4 <stat+0x34>
 407:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 40e:	66 90                	xchg   %ax,%ax

00000410 <atoi>:

int
atoi(const char *s)
{
 410:	55                   	push   %ebp
 411:	89 e5                	mov    %esp,%ebp
 413:	53                   	push   %ebx
 414:	8b 55 08             	mov    0x8(%ebp),%edx
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
 417:	0f be 02             	movsbl (%edx),%eax
 41a:	8d 48 d0             	lea    -0x30(%eax),%ecx
 41d:	80 f9 09             	cmp    $0x9,%cl
  n = 0;
 420:	b9 00 00 00 00       	mov    $0x0,%ecx
  while('0' <= *s && *s <= '9')
 425:	77 1e                	ja     445 <atoi+0x35>
 427:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 42e:	66 90                	xchg   %ax,%ax
    n = n*10 + *s++ - '0';
 430:	83 c2 01             	add    $0x1,%edx
 433:	8d 0c 89             	lea    (%ecx,%ecx,4),%ecx
 436:	8d 4c 48 d0          	lea    -0x30(%eax,%ecx,2),%ecx
  while('0' <= *s && *s <= '9')
 43a:	0f be 02             	movsbl (%edx),%eax
 43d:	8d 58 d0             	lea    -0x30(%eax),%ebx
 440:	80 fb 09             	cmp    $0x9,%bl
 443:	76 eb                	jbe    430 <atoi+0x20>
  return n;
}
 445:	8b 5d fc             	mov    -0x4(%ebp),%ebx
 448:	89 c8                	mov    %ecx,%eax
 44a:	c9                   	leave  
 44b:	c3                   	ret    
 44c:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi

00000450 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 450:	55                   	push   %ebp
 451:	89 e5                	mov    %esp,%ebp
 453:	57                   	push   %edi
 454:	8b 45 10             	mov    0x10(%ebp),%eax
 457:	8b 55 08             	mov    0x8(%ebp),%edx
 45a:	56                   	push   %esi
 45b:	8b 75 0c             	mov    0xc(%ebp),%esi
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
 45e:	85 c0                	test   %eax,%eax
 460:	7e 13                	jle    475 <memmove+0x25>
 462:	01 d0                	add    %edx,%eax
  dst = vdst;
 464:	89 d7                	mov    %edx,%edi
 466:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 46d:	8d 76 00             	lea    0x0(%esi),%esi
    *dst++ = *src++;
 470:	a4                   	movsb  %ds:(%esi),%es:(%edi)
  while(n-- > 0)
 471:	39 f8                	cmp    %edi,%eax
 473:	75 fb                	jne    470 <memmove+0x20>
  return vdst;
}
 475:	5e                   	pop    %esi
 476:	89 d0                	mov    %edx,%eax
 478:	5f                   	pop    %edi
 479:	5d                   	pop    %ebp
 47a:	c3                   	ret    

0000047b <fork>:
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
 47b:	b8 01 00 00 00       	mov    $0x1,%eax
 480:	cd 40                	int    $0x40
 482:	c3                   	ret    

00000483 <exit>:
SYSCALL(exit)
 483:	b8 02 00 00 00       	mov    $0x2,%eax
 488:	cd 40                	int    $0x40
 48a:	c3                   	ret    

0000048b <wait>:
SYSCALL(wait)
 48b:	b8 03 00 00 00       	mov    $0x3,%eax
 490:	cd 40                	int    $0x40
 492:	c3                   	ret    

00000493 <pipe>:
SYSCALL(pipe)
 493:	b8 04 00 00 00       	mov    $0x4,%eax
 498:	cd 40                	int    $0x40
 49a:	c3                   	ret    

0000049b <read>:
SYSCALL(read)
 49b:	b8 05 00 00 00       	mov    $0x5,%eax
 4a0:	cd 40                	int    $0x40
 4a2:	c3                   	ret    

000004a3 <write>:
SYSCALL(write)
 4a3:	b8 10 00 00 00       	mov    $0x10,%eax
 4a8:	cd 40                	int    $0x40
 4aa:	c3                   	ret    

000004ab <close>:
SYSCALL(close)
 4ab:	b8 15 00 00 00       	mov    $0x15,%eax
 4b0:	cd 40                	int    $0x40
 4b2:	c3                   	ret    

000004b3 <kill>:
SYSCALL(kill)
 4b3:	b8 06 00 00 00       	mov    $0x6,%eax
 4b8:	cd 40                	int    $0x40
 4ba:	c3                   	ret    

000004bb <exec>:
SYSCALL(exec)
 4bb:	b8 07 00 00 00       	mov    $0x7,%eax
 4c0:	cd 40                	int    $0x40
 4c2:	c3                   	ret    

000004c3 <open>:
SYSCALL(open)
 4c3:	b8 0f 00 00 00       	mov    $0xf,%eax
 4c8:	cd 40                	int    $0x40
 4ca:	c3                   	ret    

000004cb <mknod>:
SYSCALL(mknod)
 4cb:	b8 11 00 00 00       	mov    $0x11,%eax
 4d0:	cd 40                	int    $0x40
 4d2:	c3                   	ret    

000004d3 <unlink>:
SYSCALL(unlink)
 4d3:	b8 12 00 00 00       	mov    $0x12,%eax
 4d8:	cd 40                	int    $0x40
 4da:	c3                   	ret    

000004db <fstat>:
SYSCALL(fstat)
 4db:	b8 08 00 00 00       	mov    $0x8,%eax
 4e0:	cd 40                	int    $0x40
 4e2:	c3                   	ret    

000004e3 <link>:
SYSCALL(link)
 4e3:	b8 13 00 00 00       	mov    $0x13,%eax
 4e8:	cd 40                	int    $0x40
 4ea:	c3                   	ret    

000004eb <mkdir>:
SYSCALL(mkdir)
 4eb:	b8 14 00 00 00       	mov    $0x14,%eax
 4f0:	cd 40                	int    $0x40
 4f2:	c3                   	ret    

000004f3 <chdir>:
SYSCALL(chdir)
 4f3:	b8 09 00 00 00       	mov    $0x9,%eax
 4f8:	cd 40                	int    $0x40
 4fa:	c3                   	ret    

000004fb <dup>:
SYSCALL(dup)
 4fb:	b8 0a 00 00 00       	mov    $0xa,%eax
 500:	cd 40                	int    $0x40
 502:	c3                   	ret    

00000503 <getpid>:
SYSCALL(getpid)
 503:	b8 0b 00 00 00       	mov    $0xb,%eax
 508:	cd 40                	int    $0x40
 50a:	c3                   	ret    

0000050b <sbrk>:
SYSCALL(sbrk)
 50b:	b8 0c 00 00 00       	mov    $0xc,%eax
 510:	cd 40                	int    $0x40
 512:	c3                   	ret    

00000513 <sleep>:
SYSCALL(sleep)
 513:	b8 0d 00 00 00       	mov    $0xd,%eax
 518:	cd 40                	int    $0x40
 51a:	c3                   	ret    

0000051b <uptime>:
SYSCALL(uptime)
 51b:	b8 0e 00 00 00       	mov    $0xe,%eax
 520:	cd 40                	int    $0x40
 522:	c3                   	ret    

00000523 <setPriority>:
SYSCALL(setPriority)
 523:	b8 18 00 00 00       	mov    $0x18,%eax
 528:	cd 40                	int    $0x40
 52a:	c3                   	ret    

0000052b <yield>:
SYSCALL(yield)
 52b:	b8 19 00 00 00       	mov    $0x19,%eax
 530:	cd 40                	int    $0x40
 532:	c3                   	ret    

00000533 <getLevel>:
SYSCALL(getLevel)
 533:	b8 1a 00 00 00       	mov    $0x1a,%eax
 538:	cd 40                	int    $0x40
 53a:	c3                   	ret    

0000053b <schedulerLock>:
SYSCALL(schedulerLock)
 53b:	b8 1b 00 00 00       	mov    $0x1b,%eax
 540:	cd 40                	int    $0x40
 542:	c3                   	ret    

00000543 <schedulerUnlock>:
SYSCALL(schedulerUnlock)
 543:	b8 1c 00 00 00       	mov    $0x1c,%eax
 548:	cd 40                	int    $0x40
 54a:	c3                   	ret    

0000054b <tickinfo>:
 54b:	b8 1d 00 00 00       	mov    $0x1d,%eax
 550:	cd 40                	int    $0x40
 552:	c3                   	ret    
 553:	66 90                	xchg   %ax,%ax
 555:	66 90                	xchg   %ax,%ax
 557:	66 90                	xchg   %ax,%ax
 559:	66 90                	xchg   %ax,%ax
 55b:	66 90                	xchg   %ax,%ax
 55d:	66 90                	xchg   %ax,%ax
 55f:	90                   	nop

00000560 <printint>:
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
 560:	55                   	push   %ebp
 561:	89 e5                	mov    %esp,%ebp
 563:	57                   	push   %edi
 564:	56                   	push   %esi
 565:	53                   	push   %ebx
 566:	83 ec 3c             	sub    $0x3c,%esp
 569:	89 4d c4             	mov    %ecx,-0x3c(%ebp)
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
 56c:	89 d1                	mov    %edx,%ecx
{
 56e:	89 45 b8             	mov    %eax,-0x48(%ebp)
  if(sgn && xx < 0){
 571:	85 d2                	test   %edx,%edx
 573:	0f 89 7f 00 00 00    	jns    5f8 <printint+0x98>
 579:	f6 45 08 01          	testb  $0x1,0x8(%ebp)
 57d:	74 79                	je     5f8 <printint+0x98>
    neg = 1;
 57f:	c7 45 bc 01 00 00 00 	movl   $0x1,-0x44(%ebp)
    x = -xx;
 586:	f7 d9                	neg    %ecx
  } else {
    x = xx;
  }

  i = 0;
 588:	31 db                	xor    %ebx,%ebx
 58a:	8d 75 d7             	lea    -0x29(%ebp),%esi
 58d:	8d 76 00             	lea    0x0(%esi),%esi
  do{
    buf[i++] = digits[x % base];
 590:	89 c8                	mov    %ecx,%eax
 592:	31 d2                	xor    %edx,%edx
 594:	89 cf                	mov    %ecx,%edi
 596:	f7 75 c4             	divl   -0x3c(%ebp)
 599:	0f b6 92 14 0a 00 00 	movzbl 0xa14(%edx),%edx
 5a0:	89 45 c0             	mov    %eax,-0x40(%ebp)
 5a3:	89 d8                	mov    %ebx,%eax
 5a5:	8d 5b 01             	lea    0x1(%ebx),%ebx
  }while((x /= base) != 0);
 5a8:	8b 4d c0             	mov    -0x40(%ebp),%ecx
    buf[i++] = digits[x % base];
 5ab:	88 14 1e             	mov    %dl,(%esi,%ebx,1)
  }while((x /= base) != 0);
 5ae:	39 7d c4             	cmp    %edi,-0x3c(%ebp)
 5b1:	76 dd                	jbe    590 <printint+0x30>
  if(neg)
 5b3:	8b 4d bc             	mov    -0x44(%ebp),%ecx
 5b6:	85 c9                	test   %ecx,%ecx
 5b8:	74 0c                	je     5c6 <printint+0x66>
    buf[i++] = '-';
 5ba:	c6 44 1d d8 2d       	movb   $0x2d,-0x28(%ebp,%ebx,1)
    buf[i++] = digits[x % base];
 5bf:	89 d8                	mov    %ebx,%eax
    buf[i++] = '-';
 5c1:	ba 2d 00 00 00       	mov    $0x2d,%edx

  while(--i >= 0)
 5c6:	8b 7d b8             	mov    -0x48(%ebp),%edi
 5c9:	8d 5c 05 d7          	lea    -0x29(%ebp,%eax,1),%ebx
 5cd:	eb 07                	jmp    5d6 <printint+0x76>
 5cf:	90                   	nop
    putc(fd, buf[i]);
 5d0:	0f b6 13             	movzbl (%ebx),%edx
 5d3:	83 eb 01             	sub    $0x1,%ebx
  write(fd, &c, 1);
 5d6:	83 ec 04             	sub    $0x4,%esp
 5d9:	88 55 d7             	mov    %dl,-0x29(%ebp)
 5dc:	6a 01                	push   $0x1
 5de:	56                   	push   %esi
 5df:	57                   	push   %edi
 5e0:	e8 be fe ff ff       	call   4a3 <write>
  while(--i >= 0)
 5e5:	83 c4 10             	add    $0x10,%esp
 5e8:	39 de                	cmp    %ebx,%esi
 5ea:	75 e4                	jne    5d0 <printint+0x70>
}
 5ec:	8d 65 f4             	lea    -0xc(%ebp),%esp
 5ef:	5b                   	pop    %ebx
 5f0:	5e                   	pop    %esi
 5f1:	5f                   	pop    %edi
 5f2:	5d                   	pop    %ebp
 5f3:	c3                   	ret    
 5f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
  neg = 0;
 5f8:	c7 45 bc 00 00 00 00 	movl   $0x0,-0x44(%ebp)
 5ff:	eb 87                	jmp    588 <printint+0x28>
 601:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 608:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 60f:	90                   	nop

00000610 <printf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int fd, const char *fmt, ...)
{
 610:	55                   	push   %ebp
 611:	89 e5                	mov    %esp,%ebp
 613:	57                   	push   %edi
 614:	56                   	push   %esi
 615:	53                   	push   %ebx
 616:	83 ec 2c             	sub    $0x2c,%esp
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
 619:	8b 5d 0c             	mov    0xc(%ebp),%ebx
{
 61c:	8b 75 08             	mov    0x8(%ebp),%esi
  for(i = 0; fmt[i]; i++){
 61f:	0f b6 13             	movzbl (%ebx),%edx
 622:	84 d2                	test   %dl,%dl
 624:	74 6a                	je     690 <printf+0x80>
  ap = (uint*)(void*)&fmt + 1;
 626:	8d 45 10             	lea    0x10(%ebp),%eax
 629:	83 c3 01             	add    $0x1,%ebx
  write(fd, &c, 1);
 62c:	8d 7d e7             	lea    -0x19(%ebp),%edi
  state = 0;
 62f:	31 c9                	xor    %ecx,%ecx
  ap = (uint*)(void*)&fmt + 1;
 631:	89 45 d0             	mov    %eax,-0x30(%ebp)
 634:	eb 36                	jmp    66c <printf+0x5c>
 636:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 63d:	8d 76 00             	lea    0x0(%esi),%esi
 640:	89 4d d4             	mov    %ecx,-0x2c(%ebp)
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
 643:	b9 25 00 00 00       	mov    $0x25,%ecx
      if(c == '%'){
 648:	83 f8 25             	cmp    $0x25,%eax
 64b:	74 15                	je     662 <printf+0x52>
  write(fd, &c, 1);
 64d:	83 ec 04             	sub    $0x4,%esp
 650:	88 55 e7             	mov    %dl,-0x19(%ebp)
 653:	6a 01                	push   $0x1
 655:	57                   	push   %edi
 656:	56                   	push   %esi
 657:	e8 47 fe ff ff       	call   4a3 <write>
 65c:	8b 4d d4             	mov    -0x2c(%ebp),%ecx
      } else {
        putc(fd, c);
 65f:	83 c4 10             	add    $0x10,%esp
  for(i = 0; fmt[i]; i++){
 662:	0f b6 13             	movzbl (%ebx),%edx
 665:	83 c3 01             	add    $0x1,%ebx
 668:	84 d2                	test   %dl,%dl
 66a:	74 24                	je     690 <printf+0x80>
    c = fmt[i] & 0xff;
 66c:	0f b6 c2             	movzbl %dl,%eax
    if(state == 0){
 66f:	85 c9                	test   %ecx,%ecx
 671:	74 cd                	je     640 <printf+0x30>
      }
    } else if(state == '%'){
 673:	83 f9 25             	cmp    $0x25,%ecx
 676:	75 ea                	jne    662 <printf+0x52>
      if(c == 'd'){
 678:	83 f8 25             	cmp    $0x25,%eax
 67b:	0f 84 07 01 00 00    	je     788 <printf+0x178>
 681:	83 e8 63             	sub    $0x63,%eax
 684:	83 f8 15             	cmp    $0x15,%eax
 687:	77 17                	ja     6a0 <printf+0x90>
 689:	ff 24 85 bc 09 00 00 	jmp    *0x9bc(,%eax,4)
        putc(fd, c);
      }
      state = 0;
    }
  }
}
 690:	8d 65 f4             	lea    -0xc(%ebp),%esp
 693:	5b                   	pop    %ebx
 694:	5e                   	pop    %esi
 695:	5f                   	pop    %edi
 696:	5d                   	pop    %ebp
 697:	c3                   	ret    
 698:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 69f:	90                   	nop
  write(fd, &c, 1);
 6a0:	83 ec 04             	sub    $0x4,%esp
 6a3:	88 55 d4             	mov    %dl,-0x2c(%ebp)
 6a6:	6a 01                	push   $0x1
 6a8:	57                   	push   %edi
 6a9:	56                   	push   %esi
 6aa:	c6 45 e7 25          	movb   $0x25,-0x19(%ebp)
 6ae:	e8 f0 fd ff ff       	call   4a3 <write>
        putc(fd, c);
 6b3:	0f b6 55 d4          	movzbl -0x2c(%ebp),%edx
  write(fd, &c, 1);
 6b7:	83 c4 0c             	add    $0xc,%esp
 6ba:	88 55 e7             	mov    %dl,-0x19(%ebp)
 6bd:	6a 01                	push   $0x1
 6bf:	57                   	push   %edi
 6c0:	56                   	push   %esi
 6c1:	e8 dd fd ff ff       	call   4a3 <write>
        putc(fd, c);
 6c6:	83 c4 10             	add    $0x10,%esp
      state = 0;
 6c9:	31 c9                	xor    %ecx,%ecx
 6cb:	eb 95                	jmp    662 <printf+0x52>
 6cd:	8d 76 00             	lea    0x0(%esi),%esi
        printint(fd, *ap, 16, 0);
 6d0:	83 ec 0c             	sub    $0xc,%esp
 6d3:	b9 10 00 00 00       	mov    $0x10,%ecx
 6d8:	6a 00                	push   $0x0
 6da:	8b 45 d0             	mov    -0x30(%ebp),%eax
 6dd:	8b 10                	mov    (%eax),%edx
 6df:	89 f0                	mov    %esi,%eax
 6e1:	e8 7a fe ff ff       	call   560 <printint>
        ap++;
 6e6:	83 45 d0 04          	addl   $0x4,-0x30(%ebp)
 6ea:	83 c4 10             	add    $0x10,%esp
      state = 0;
 6ed:	31 c9                	xor    %ecx,%ecx
 6ef:	e9 6e ff ff ff       	jmp    662 <printf+0x52>
 6f4:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
        s = (char*)*ap;
 6f8:	8b 45 d0             	mov    -0x30(%ebp),%eax
 6fb:	8b 10                	mov    (%eax),%edx
        ap++;
 6fd:	83 c0 04             	add    $0x4,%eax
 700:	89 45 d0             	mov    %eax,-0x30(%ebp)
        if(s == 0)
 703:	85 d2                	test   %edx,%edx
 705:	0f 84 8d 00 00 00    	je     798 <printf+0x188>
        while(*s != 0){
 70b:	0f b6 02             	movzbl (%edx),%eax
      state = 0;
 70e:	31 c9                	xor    %ecx,%ecx
        while(*s != 0){
 710:	84 c0                	test   %al,%al
 712:	0f 84 4a ff ff ff    	je     662 <printf+0x52>
 718:	89 5d d4             	mov    %ebx,-0x2c(%ebp)
 71b:	89 d3                	mov    %edx,%ebx
 71d:	8d 76 00             	lea    0x0(%esi),%esi
  write(fd, &c, 1);
 720:	83 ec 04             	sub    $0x4,%esp
          s++;
 723:	83 c3 01             	add    $0x1,%ebx
 726:	88 45 e7             	mov    %al,-0x19(%ebp)
  write(fd, &c, 1);
 729:	6a 01                	push   $0x1
 72b:	57                   	push   %edi
 72c:	56                   	push   %esi
 72d:	e8 71 fd ff ff       	call   4a3 <write>
        while(*s != 0){
 732:	0f b6 03             	movzbl (%ebx),%eax
 735:	83 c4 10             	add    $0x10,%esp
 738:	84 c0                	test   %al,%al
 73a:	75 e4                	jne    720 <printf+0x110>
      state = 0;
 73c:	8b 5d d4             	mov    -0x2c(%ebp),%ebx
 73f:	31 c9                	xor    %ecx,%ecx
 741:	e9 1c ff ff ff       	jmp    662 <printf+0x52>
 746:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 74d:	8d 76 00             	lea    0x0(%esi),%esi
        printint(fd, *ap, 10, 1);
 750:	83 ec 0c             	sub    $0xc,%esp
 753:	b9 0a 00 00 00       	mov    $0xa,%ecx
 758:	6a 01                	push   $0x1
 75a:	e9 7b ff ff ff       	jmp    6da <printf+0xca>
 75f:	90                   	nop
        putc(fd, *ap);
 760:	8b 45 d0             	mov    -0x30(%ebp),%eax
  write(fd, &c, 1);
 763:	83 ec 04             	sub    $0x4,%esp
        putc(fd, *ap);
 766:	8b 00                	mov    (%eax),%eax
  write(fd, &c, 1);
 768:	6a 01                	push   $0x1
 76a:	57                   	push   %edi
 76b:	56                   	push   %esi
        putc(fd, *ap);
 76c:	88 45 e7             	mov    %al,-0x19(%ebp)
  write(fd, &c, 1);
 76f:	e8 2f fd ff ff       	call   4a3 <write>
        ap++;
 774:	83 45 d0 04          	addl   $0x4,-0x30(%ebp)
 778:	83 c4 10             	add    $0x10,%esp
      state = 0;
 77b:	31 c9                	xor    %ecx,%ecx
 77d:	e9 e0 fe ff ff       	jmp    662 <printf+0x52>
 782:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        putc(fd, c);
 788:	88 55 e7             	mov    %dl,-0x19(%ebp)
  write(fd, &c, 1);
 78b:	83 ec 04             	sub    $0x4,%esp
 78e:	e9 2a ff ff ff       	jmp    6bd <printf+0xad>
 793:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
 797:	90                   	nop
          s = "(null)";
 798:	ba b3 09 00 00       	mov    $0x9b3,%edx
        while(*s != 0){
 79d:	89 5d d4             	mov    %ebx,-0x2c(%ebp)
 7a0:	b8 28 00 00 00       	mov    $0x28,%eax
 7a5:	89 d3                	mov    %edx,%ebx
 7a7:	e9 74 ff ff ff       	jmp    720 <printf+0x110>
 7ac:	66 90                	xchg   %ax,%ax
 7ae:	66 90                	xchg   %ax,%ax

000007b0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 7b0:	55                   	push   %ebp
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 7b1:	a1 78 0d 00 00       	mov    0xd78,%eax
{
 7b6:	89 e5                	mov    %esp,%ebp
 7b8:	57                   	push   %edi
 7b9:	56                   	push   %esi
 7ba:	53                   	push   %ebx
 7bb:	8b 5d 08             	mov    0x8(%ebp),%ebx
  bp = (Header*)ap - 1;
 7be:	8d 4b f8             	lea    -0x8(%ebx),%ecx
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 7c1:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi
 7c8:	89 c2                	mov    %eax,%edx
 7ca:	8b 00                	mov    (%eax),%eax
 7cc:	39 ca                	cmp    %ecx,%edx
 7ce:	73 30                	jae    800 <free+0x50>
 7d0:	39 c1                	cmp    %eax,%ecx
 7d2:	72 04                	jb     7d8 <free+0x28>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 7d4:	39 c2                	cmp    %eax,%edx
 7d6:	72 f0                	jb     7c8 <free+0x18>
      break;
  if(bp + bp->s.size == p->s.ptr){
 7d8:	8b 73 fc             	mov    -0x4(%ebx),%esi
 7db:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 7de:	39 f8                	cmp    %edi,%eax
 7e0:	74 30                	je     812 <free+0x62>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
 7e2:	89 43 f8             	mov    %eax,-0x8(%ebx)
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
 7e5:	8b 42 04             	mov    0x4(%edx),%eax
 7e8:	8d 34 c2             	lea    (%edx,%eax,8),%esi
 7eb:	39 f1                	cmp    %esi,%ecx
 7ed:	74 3a                	je     829 <free+0x79>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
 7ef:	89 0a                	mov    %ecx,(%edx)
  } else
    p->s.ptr = bp;
  freep = p;
}
 7f1:	5b                   	pop    %ebx
  freep = p;
 7f2:	89 15 78 0d 00 00    	mov    %edx,0xd78
}
 7f8:	5e                   	pop    %esi
 7f9:	5f                   	pop    %edi
 7fa:	5d                   	pop    %ebp
 7fb:	c3                   	ret    
 7fc:	8d 74 26 00          	lea    0x0(%esi,%eiz,1),%esi
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 800:	39 c2                	cmp    %eax,%edx
 802:	72 c4                	jb     7c8 <free+0x18>
 804:	39 c1                	cmp    %eax,%ecx
 806:	73 c0                	jae    7c8 <free+0x18>
  if(bp + bp->s.size == p->s.ptr){
 808:	8b 73 fc             	mov    -0x4(%ebx),%esi
 80b:	8d 3c f1             	lea    (%ecx,%esi,8),%edi
 80e:	39 f8                	cmp    %edi,%eax
 810:	75 d0                	jne    7e2 <free+0x32>
    bp->s.size += p->s.ptr->s.size;
 812:	03 70 04             	add    0x4(%eax),%esi
 815:	89 73 fc             	mov    %esi,-0x4(%ebx)
    bp->s.ptr = p->s.ptr->s.ptr;
 818:	8b 02                	mov    (%edx),%eax
 81a:	8b 00                	mov    (%eax),%eax
 81c:	89 43 f8             	mov    %eax,-0x8(%ebx)
  if(p + p->s.size == bp){
 81f:	8b 42 04             	mov    0x4(%edx),%eax
 822:	8d 34 c2             	lea    (%edx,%eax,8),%esi
 825:	39 f1                	cmp    %esi,%ecx
 827:	75 c6                	jne    7ef <free+0x3f>
    p->s.size += bp->s.size;
 829:	03 43 fc             	add    -0x4(%ebx),%eax
  freep = p;
 82c:	89 15 78 0d 00 00    	mov    %edx,0xd78
    p->s.size += bp->s.size;
 832:	89 42 04             	mov    %eax,0x4(%edx)
    p->s.ptr = bp->s.ptr;
 835:	8b 4b f8             	mov    -0x8(%ebx),%ecx
 838:	89 0a                	mov    %ecx,(%edx)
}
 83a:	5b                   	pop    %ebx
 83b:	5e                   	pop    %esi
 83c:	5f                   	pop    %edi
 83d:	5d                   	pop    %ebp
 83e:	c3                   	ret    
 83f:	90                   	nop

00000840 <malloc>:
  return freep;
}

void*
malloc(uint nbytes)
{
 840:	55                   	push   %ebp
 841:	89 e5                	mov    %esp,%ebp
 843:	57                   	push   %edi
 844:	56                   	push   %esi
 845:	53                   	push   %ebx
 846:	83 ec 1c             	sub    $0x1c,%esp
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 849:	8b 45 08             	mov    0x8(%ebp),%eax
  if((prevp = freep) == 0){
 84c:	8b 3d 78 0d 00 00    	mov    0xd78,%edi
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 852:	8d 70 07             	lea    0x7(%eax),%esi
 855:	c1 ee 03             	shr    $0x3,%esi
 858:	83 c6 01             	add    $0x1,%esi
  if((prevp = freep) == 0){
 85b:	85 ff                	test   %edi,%edi
 85d:	0f 84 9d 00 00 00    	je     900 <malloc+0xc0>
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 863:	8b 17                	mov    (%edi),%edx
    if(p->s.size >= nunits){
 865:	8b 4a 04             	mov    0x4(%edx),%ecx
 868:	39 f1                	cmp    %esi,%ecx
 86a:	73 6a                	jae    8d6 <malloc+0x96>
 86c:	bb 00 10 00 00       	mov    $0x1000,%ebx
 871:	39 de                	cmp    %ebx,%esi
 873:	0f 43 de             	cmovae %esi,%ebx
  p = sbrk(nu * sizeof(Header));
 876:	8d 04 dd 00 00 00 00 	lea    0x0(,%ebx,8),%eax
 87d:	89 45 e4             	mov    %eax,-0x1c(%ebp)
 880:	eb 17                	jmp    899 <malloc+0x59>
 882:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 888:	8b 02                	mov    (%edx),%eax
    if(p->s.size >= nunits){
 88a:	8b 48 04             	mov    0x4(%eax),%ecx
 88d:	39 f1                	cmp    %esi,%ecx
 88f:	73 4f                	jae    8e0 <malloc+0xa0>
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
 891:	8b 3d 78 0d 00 00    	mov    0xd78,%edi
 897:	89 c2                	mov    %eax,%edx
 899:	39 d7                	cmp    %edx,%edi
 89b:	75 eb                	jne    888 <malloc+0x48>
  p = sbrk(nu * sizeof(Header));
 89d:	83 ec 0c             	sub    $0xc,%esp
 8a0:	ff 75 e4             	push   -0x1c(%ebp)
 8a3:	e8 63 fc ff ff       	call   50b <sbrk>
  if(p == (char*)-1)
 8a8:	83 c4 10             	add    $0x10,%esp
 8ab:	83 f8 ff             	cmp    $0xffffffff,%eax
 8ae:	74 1c                	je     8cc <malloc+0x8c>
  hp->s.size = nu;
 8b0:	89 58 04             	mov    %ebx,0x4(%eax)
  free((void*)(hp + 1));
 8b3:	83 ec 0c             	sub    $0xc,%esp
 8b6:	83 c0 08             	add    $0x8,%eax
 8b9:	50                   	push   %eax
 8ba:	e8 f1 fe ff ff       	call   7b0 <free>
  return freep;
 8bf:	8b 15 78 0d 00 00    	mov    0xd78,%edx
      if((p = morecore(nunits)) == 0)
 8c5:	83 c4 10             	add    $0x10,%esp
 8c8:	85 d2                	test   %edx,%edx
 8ca:	75 bc                	jne    888 <malloc+0x48>
        return 0;
  }
}
 8cc:	8d 65 f4             	lea    -0xc(%ebp),%esp
        return 0;
 8cf:	31 c0                	xor    %eax,%eax
}
 8d1:	5b                   	pop    %ebx
 8d2:	5e                   	pop    %esi
 8d3:	5f                   	pop    %edi
 8d4:	5d                   	pop    %ebp
 8d5:	c3                   	ret    
    if(p->s.size >= nunits){
 8d6:	89 d0                	mov    %edx,%eax
 8d8:	89 fa                	mov    %edi,%edx
 8da:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
      if(p->s.size == nunits)
 8e0:	39 ce                	cmp    %ecx,%esi
 8e2:	74 4c                	je     930 <malloc+0xf0>
        p->s.size -= nunits;
 8e4:	29 f1                	sub    %esi,%ecx
 8e6:	89 48 04             	mov    %ecx,0x4(%eax)
        p += p->s.size;
 8e9:	8d 04 c8             	lea    (%eax,%ecx,8),%eax
        p->s.size = nunits;
 8ec:	89 70 04             	mov    %esi,0x4(%eax)
      freep = prevp;
 8ef:	89 15 78 0d 00 00    	mov    %edx,0xd78
}
 8f5:	8d 65 f4             	lea    -0xc(%ebp),%esp
      return (void*)(p + 1);
 8f8:	83 c0 08             	add    $0x8,%eax
}
 8fb:	5b                   	pop    %ebx
 8fc:	5e                   	pop    %esi
 8fd:	5f                   	pop    %edi
 8fe:	5d                   	pop    %ebp
 8ff:	c3                   	ret    
    base.s.ptr = freep = prevp = &base;
 900:	c7 05 78 0d 00 00 7c 	movl   $0xd7c,0xd78
 907:	0d 00 00 
    base.s.size = 0;
 90a:	bf 7c 0d 00 00       	mov    $0xd7c,%edi
    base.s.ptr = freep = prevp = &base;
 90f:	c7 05 7c 0d 00 00 7c 	movl   $0xd7c,0xd7c
 916:	0d 00 00 
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 919:	89 fa                	mov    %edi,%edx
    base.s.size = 0;
 91b:	c7 05 80 0d 00 00 00 	movl   $0x0,0xd80
 922:	00 00 00 
    if(p->s.size >= nunits){
 925:	e9 42 ff ff ff       	jmp    86c <malloc+0x2c>
 92a:	8d b6 00 00 00 00    	lea    0x0(%esi),%esi
        prevp->s.ptr = p->s.ptr;
 930:	8b 08                	mov    (%eax),%ecx
 932:	89 0a                	mov    %ecx,(%edx)
 934:	eb b9                	jmp    8ef <malloc+0xaf>
