# EOF of stdio PIPE checking is broken running Windows executable with WSL2

First, I placed all files to explain this issue at https://github.com/LumaKernel/issue-WSL2-pipe .

`a.c` is small program enough to reproduce this issue. ( Simple program to read lines and print unless it finds EOF. )

`a.exe` is compiled with `gcc.exe a.c`.
`a.linux.out` is compiled with `gcc a.c`.

I found the same issue with Go and Rust programs (including `clip.exe` ( [one shipped from Microsoft](https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/clip) ;) ), [`win32yank`](https://github.com/equalsraf/win32yank), [my small Go program](https://github.com/LumaKernel/inspect-cmd) and `powershell.exe` (see below) ).

This issue doesn't happen with Linux Programs.

## Environment

```
$ # All below commands are run with bash.
$ # using WSL2
$ echo $SHELL
/bin/bash
$ uname -a
Linux MyComputer 4.19.104-microsoft-standard #1 SMP Wed Feb 19 06:37:35 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
$ cat /proc/version
Linux version 4.19.104-microsoft-standard (oe-user@oe-host) (gcc version 8.2.0 (GCC)) #1 SMP Wed Feb 19 06:37:35 UTC 2020
$ cat /etc/os-release
NAME="Ubuntu"
VERSION="18.04.4 LTS (Bionic Beaver)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 18.04.4 LTS"
VERSION_ID="18.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=bionic
UBUNTU_CODENAME=bionic
$ which gcc.exe  # This is shipped from `scoop install gcc` ( actually mingw-w64's one )
/mnt/c/Users/luma/scoop/apps/gcc/current/bin/gcc.exe
$ which gcc
/home/linuxbrew/.linuxbrew/bin/gcc
```

`winver.exe` is saying...

![image](https://user-images.githubusercontent.com/29811106/89128291-b2502f00-d52f-11ea-864b-7598618a3792.png)

`ver` and `wsl -l -v` in cmd is saying...

```bat
C:\Users\luma>ver

Microsoft Windows [Version 10.0.20180.1000]

C:\Users\luma>wsl -l -v
  NAME                   STATE           VERSION
* Ubuntu                 Running         2
  docker-desktop         Running         2
  Ubuntu-18.04           Stopped         1
  docker-desktop-data    Running         2
  Alpine                 Running         2
```

## Problem

The piping is broken if checking EOF in program.
Windows executable program will wait user input.

```bash
$ ./a.exe < sample.txt  # works well with file input
61 0a    |a.|
62 0a    |b.|
FOUND EOF!!
$ echo a | ./a.exe  # not working well
61 0a    |a.|
^C$
$ # After at least 3 seconds, but not reaching EOF
$ # and I hit CTRL-C to stop a program.
$ # same
$ cat sample.txt | ./a.exe
61 0a    |a.|
62 0a    |b.|
^C$
```

Below is using linux native executable ( and this is the expected behavior. )

```bash
$ ./a.linux.out < sample.txt
61 0a    |a.|
62 0a    |b.|
FOUND EOF!!
$ cat sample.txt | ./a.linux.out
61 0a    |a.|
62 0a    |b.|
FOUND EOF!!
```


This may have happened after installing update of WSL2 Linux Kernel from [here](https://docs.microsoft.com/en-us/windows/wsl/wsl2-kernel). It was working well before installing this.


## Q. Works with single line read ?

A. Yes.

```bash
$ echo -ne "a\x0a" | powershell.exe -c "Read-Host"
a
a

$
```


## Q. Works good with CRLF ?

A. No. 

```bash
$ echo -en "a\x0d\x0ab\x0d\x0a\x0d\x0a" | ./a.exe
61 0a    |a.|
62 0a    |b.|
0a     |.|
^C$
```
