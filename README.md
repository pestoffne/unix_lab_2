DESCRIPTION

Yet another lab2.


HOW TO BUILD
<pre>
$ mkdir build
$ cd build
$ cmake ..
$ make
</pre>


HOW TO RUN
<pre>
$ ./a.out --execute "CMD ARGS" [--logfile LF] [--multiplex M | --select | --async]
</pre>


EXAMPLES
<pre>
./a.out --execute "ls -a"
cat README.md | ./a.out --execute "sort"
find /* 2>/dev/null | ./a.out --execute "grep pestov"
./a.out --execute 'ipython3 --no-banner --no-confirm-exit' --async
</pre>
