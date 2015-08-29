# Runtime and Lifecycle

## Lifecycle

A typical lifecyle progresses like this:

1. There is no container or running application
2. A user tells the runtime to [create](#create) a container
3. The runtime creates the container
4. Manage applications:
    1. A user tells the runtime to [start](#start-process) an application
    2. The runtime executes any [pre-start hooks](runtime.md#pre-start)
    3. The runtime executes the application
    4. The application is running
    5. A user tells the runtime to [stop](#stop) an application
    6. The runtime sends a termination signal to the application
    7. The application exits
    8. The runtime executes any [post-stop hooks](runtime.md#post-stop)
5. A user tells the runtime to [destroy](#destroy) the container
6. The runtime removes the container

With steps v and vi, the user is explicitly stopping the application
(via the runtime), but it's also possible that the application could
exit for other reasons.  In that case we skip directly from iv to vii.

You can also repeat the application steps in 4 as many times as you
want, to launch multiple applications inside the same container.

### Create

Creates the container: file system, namespaces, cgroups, capabilities.

### Start (process)

Runs a process in a container. Can be invoked several times.

### Stop (process)

Not sure we need that from runc cli. Process is killed from the outside.

This event needs to be captured by runc to run onstop event handlers.

### Destroy

Remove the container: unmount file systems, remove namespaces, etc.
This is the inverse of [create](#create).
