BEGIN {}
{
	for (i = 1; i <= NF; ++i) {
		if($i != "\\"){
			printf "%s ", $i
		}
	}
}
END {printf "\n"}
