start
for {set x 0} {$x < 9} {incr x} { puts [play 1];puts [play 0] }

start
assign {8240 940572672 4108 103749688}

# bug
# 1886930006 2138996736 -2004567128 -2147483648
# 1937722448 1065254912 -2004831313 -1065353216

generate
for {set i 0} {$i < 1000} {incr i} {evolve}