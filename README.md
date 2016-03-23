DESCRIPTION

Yet another lab1.


HOW TO BUILD
<pre>
$ make
</pre>


HOW TO RUN
<pre>
$ ./bin/lab --mode [=] {posix --amount=N} |
                       {kill --signo=S --pid=P} |
                       {child} |
                       {std} |
                       {pipe}
</pre>


HOW TO USE WITH --mode std
<pre>
Step 1: $ ./bin/lab --mode std
Step 2: Copy PID.
Step 3: Cause from another terminal:
        $ kill -s SIGNAL PID
Step 4: Process will print PID, GID and finished.
</pre>


HOW TO CLEAN
<pre>
$ make clean
or
$ make clear
</pre>
