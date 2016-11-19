set grp [group]
$grp assign 10
$grp initial
for {set i 0} {$i < 10} {incr i} {
	for {set j 0} {$j < 100} {incr j} {$grp train}
	$grp refresh
}

set ptn [$grp get 0]
set_ptn $ptn

#puts [[[$grp get 0] get 0 0] to_str]
