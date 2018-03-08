# <a name="VirtualMachineSpecificContainerConfiguration" /> Virtual-machine-specific Container Configuration

This section describes the schema for the [virtual-machine-specific section](config.md#platform-specific-configuration) of the [container configuration](config.md).
The virtual-machine container specification provides additional configuration for the hypervisor, kernel, and image.

## <a name="HypervisorObject" /> Hypervisor Object

**`hypervisor`** (object, OPTIONAL) configures the hypervisor process.
The schema is a subset of the [`process`](config.md#process) schema with the terminal properties `terminal` and `consoleSize` removed.

* **`cwd`** (string, REQUIRED) is the working directory that will be set for the executable.
    This value MUST be an absolute path.
* **`env`** (array of strings, OPTIONAL) with the same semantics as [IEEE Std 1003.1-2008's `environ`][ieee-1003.1-2008-xbd-c8.1_2].
* **`args`** (array of strings, REQUIRED) with similar semantics to [IEEE Std 1003.1-2008 `execvp`'s *argv*][ieee-1003.1-2008-functions-exec_2].
    This specification extends the IEEE standard in that at least one entry is REQUIRED, and that entry is used with the same semantics as `execvp`'s *file*.

### <a name="HypervisorPOSIXProcess" />POSIX process

For systems that support POSIX rlimits (for example Linux and Solaris), the `hypervisor` object supports the following process-specific properties:

* **`rlimits`** (array of objects, OPTIONAL) allows setting resource limits for the process.
    Each entry has the following structure:

    * **`type`** (string, REQUIRED) the platform resource being limited.
        * Linux: valid values are defined in the [`getrlimit(2)`][getrlimit.2_2] man page, such as `RLIMIT_MSGQUEUE`.
        * Solaris: valid values are defined in the [`getrlimit(3)`][getrlimit.3_2] man page, such as `RLIMIT_CORE`.

        The runtime MUST [generate an error](runtime.md#errors) for any values which cannot be mapped to a relevant kernel interface.
        For each entry in `rlimits`, a [`getrlimit(3)`][getrlimit.3_2] on `type` MUST succeed.
        For the following properties, `rlim` refers to the status returned by the `getrlimit(3)` call.

    * **`soft`** (uint64, REQUIRED) the value of the limit enforced for the corresponding resource.
        `rlim.rlim_cur` MUST match the configured value.
    * **`hard`** (uint64, REQUIRED) the ceiling for the soft limit that could be set by an unprivileged process.
        `rlim.rlim_max` MUST match the configured value.
        Only a privileged process (e.g. one with the `CAP_SYS_RESOURCE` capability) can raise a hard limit.

    If `rlimits` contains duplicated entries with same `type`, the runtime MUST [generate an error](runtime.md#errors).

### <a name="HypervisorLinuxProcess" />Linux Process

For Linux-based systems, the `hypervisor` object supports the following process-specific properties.

* **`apparmorProfile`** (string, OPTIONAL) specifies the name of the AppArmor profile for the process.
    For more information about AppArmor, see [AppArmor documentation][apparmor_2].
* **`capabilities`** (object, OPTIONAL) is an object containing arrays that specifies the sets of capabilities for the process.
    Valid values are defined in the [capabilities(7)][capabilities.7_2] man page, such as `CAP_CHOWN`.
    Any value which cannot be mapped to a relevant kernel interface MUST cause an error.
    `capabilities` contains the following properties:

    * **`effective`** (array of strings, OPTIONAL) the `effective` field is an array of effective capabilities that are kept for the process.
    * **`bounding`** (array of strings, OPTIONAL) the `bounding` field is an array of bounding capabilities that are kept for the process.
    * **`inheritable`** (array of strings, OPTIONAL) the `inheritable` field is an array of inheritable capabilities that are kept for the process.
    * **`permitted`** (array of strings, OPTIONAL) the `permitted` field is an array of permitted capabilities that are kept for the process.
    * **`ambient`** (array of strings, OPTIONAL) the `ambient` field is an array of ambient capabilities that are kept for the process.
* **`noNewPrivileges`** (bool, OPTIONAL) setting `noNewPrivileges` to true prevents the process from gaining additional privileges.
    As an example, the [`no_new_privs`][no-new-privs_2] article in the kernel documentation has information on how this is achieved using a `prctl` system call on Linux.
* **`oomScoreAdj`** *(int, OPTIONAL)* adjusts the oom-killer score in `[pid]/oom_score_adj` for the process's `[pid]` in a [proc pseudo-filesystem][proc_3].
    If `oomScoreAdj` is set, the runtime MUST set `oom_score_adj` to the given value.
    If `oomScoreAdj` is not set, the runtime MUST NOT change the value of `oom_score_adj`.
* **`selinuxLabel`** (string, OPTIONAL) specifies the SELinux label for the process.
    For more information about SELinux, see  [SELinux documentation][selinux_2].

### <a name="HypervisorUser" />User

The user for the process is a platform-specific structure that allows specific control over which user the process runs as.

#### <a name="HypervisorPOSIXUser" />POSIX-platform User

For POSIX platforms the `user` structure has the following fields:

* **`uid`** (int, REQUIRED) specifies the user ID in the [container namespace](glossary.md#container-namespace).
* **`gid`** (int, REQUIRED) specifies the group ID in the [container namespace](glossary.md#container-namespace).
* **`additionalGids`** (array of ints, OPTIONAL) specifies additional group IDs in the [container namespace](glossary.md#container-namespace) to be added to the process.

_Note: symbolic name for uid and gid, such as uname and gname respectively, are left to upper levels to derive (i.e. `/etc/passwd` parsing, NSS, etc)_

#### <a name="HypervisorWindowsUser" />Windows User

For Windows based systems the user structure has the following fields:

* **`username`** (string, OPTIONAL) specifies the user name for the process.

### Example

```json
    "hypervisor": {
        "args": ["/path/to/vmm", "opts1=foo", "opts2=bar"]
    }
```

## <a name="KernelObject" /> Kernel Object

**`kernel`** (object, REQUIRED) specifies details of the kernel to boot the container virtual machine with.
* **`path`** (string, REQUIRED) path to the kernel used to boot the container virtual machine.
    This value MUST be an absolute path in the [runtime mount namespace](glossary.md#runtime-namespace).
* **`parameters`** (array of strings, OPTIONAL) specifies an array of parameters to pass to the kernel.
* **`initrd`** (string, OPTIONAL) path to an initial ramdisk to be used by the container virtual machine.
    This value MUST be an absolute path in the [runtime mount namespace](glossary.md#runtime-namespace).

### Example

```json
    "kernel": {
        "path": "/path/to/vmlinuz",
        "parameters": ["foo=bar", "hello world"],
        "initrd": "/path/to/initrd.img"
    }
```

## <a name="ImageObject" /> Image Object

**`image`** (object, OPTIONAL) specifies details of the image that contains the root filesystem for the container virtual machine.
* **`path`** (string, REQUIRED) path to the container virtual machine root image.
    This value MUST be an absolute path in the [runtime mount namespace](glossary.md#runtime-namespace).
* **`format`** (string, REQUIRED) format of the container virtual machine root image. Commonly supported formats are:
    * **`raw`** [raw disk image format][raw-image-format]. Unset values for `format` will default to that format.
    * **`qcow2`** [QEMU image format][qcow2-image-format].
    * **`vdi`** [VirtualBox 1.1 compatible image format][vdi-image-format].
    * **`vmdk`** [VMware compatible image format][vmdk-image-format].
    * **`vhd`** [Virtual Hard Disk image format][vhd-image-format].

This image contains the root filesystem that the virtual machine **`kernel`** will boot into, not to be confused with the container root filesystem itself. The latter, as specified by **`path`** from the [Root Configuration](config.md#Root-Configuration) section, will be mounted inside the virtual machine at a location chosen by the virtual-machine-based runtime.

### Example

```json
    "image": {
        "path": "/path/to/vm/rootfs.img",
	"format": "raw"
    }
```

[apparmor_2]: https://wiki.ubuntu.com/AppArmor
[ieee-1003.1-2008-functions-exec_2]: http://pubs.opengroup.org/onlinepubs/9699919799/fu
[ieee-1003.1-2008-xbd-c8.1_2]: http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html#tag_08_01
[no-new-privs_2]: https://www.kernel.org/doc/Documentation/prctl/no_new_privs.txt
[proc_3]: https://www.kernel.org/doc/Documentation/filesystems/proc.txt
[qcow2-image-format]: https://git.qemu.org/?p=qemu.git;a=blob_plain;f=docs/interop/qcow2.txt;hb=HEAD
[raw-image-format]: https://en.wikipedia.org/wiki/IMG_(file_format)
[selinux_2]:http://selinuxproject.org/page/Main_Page
[vdi-image-format]: https://forensicswiki.org/wiki/Virtual_Disk_Image_(VDI)
[vhd-image-format]: https://github.com/libyal/libvhdi/blob/master/documentation/Virtual%20Hard%20Disk%20(VHD)%20image%20format.asciidoc
[vmdk-image-format]: http://www.vmware.com/app/vmdk/?src=vmdk

[capabilities.7_2]: http://man7.org/linux/man-pages/man7/capabilities.7.html
[getrlimit.2_2]: http://man7.org/linux/man-pages/man2/getrlimit.2.html
[getrlimit.3_2]: http://pubs.opengroup.org/onlinepubs/9699919799/functions/getrlimit.html
