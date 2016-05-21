DESCRIPTION

Yet another lab2.


HOW TO BUILD
<pre>
$ make
</pre>


HOW TO RUN
<pre>
$ ./manager --execute "CMD ARGS" [--logfile LF] [--multiplex M | --select | --async]
</pre>


HOW TO CLEAN
<pre>
$ make clean
or
$ make clear
</pre>


EXAPLES
<pre>
./manager --execute "ls -a"
./manager --execute "cat -n /home/pestov_ev/.ssh/id_dsa"
cat -n /home/pestov_ev/.ssh/id_dsa | ./manager --execute "tail -n 1"
cat README.md | ./manager --execute "sort"
find /* 2>/dev/null | ./manager --execute "grep pestov"
</pre>
