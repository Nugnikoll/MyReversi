set grp [group]
$grp assign 2
$grp initial
for {set i 0} {$i < 10000} {incr i} {$grp train}
$grp refresh

set ptn [$grp get 0]
set_ptn $ptn

#puts [[[$grp get 0] get 0 0] to_str]
#play $mthd_ptn $true 7 -1; play $mthd_ab $false 7 -1;