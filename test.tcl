set grp [group]
# $grp assign 2
# $grp initial

# for {set i 0} {$i < 10} {incr i} {
	# for {set j 0} {$j < 1000} {incr j} {$grp train}
	# set ptn [$grp get 0]
# }

$grp load "python/pattern_100.dat" 1 0 100 1

set ptn [$grp get 0]
set_ptn $ptn

set mthd_default $mthd_ptn

#puts [[[$grp get 0] get2 0 0] to_str]
#play $mthd_ptn $true 7 -1; play $mthd_ab $false 7 -1;