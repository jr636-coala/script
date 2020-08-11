#!/bin/sh

function wistia {
	[[ -z "$2" ]] && OUTPUT=`echo "$1" | grep -o "[A-Za-z0-9.-]*$"` || OUTPUT="$2"
	TOKEN=`curl $1 | grep -o "/embed/.*/[A-Za-Z0-9]*\"" | sed "s/.*\///; s/\"//"`
	LINK=`curl "https://fast.wistia.net/embed/iframe/$TOKEN" | grep -o "https://embed-ssl.wistia.com/deliveries/[A-Za-z0-9]*.bin" | head -1`
	curl "$LINK" > "$OUTPUT"
}

function dupe {
	s=`printf "%-${2}s" "$1"`
	[[ "$2" -eq "0" ]] && echo "" || echo "${s// /$1}"
}

function bar {
	num=$((($1 * $3) / $2))
	echo "[$(dupe \# $num)$(dupe - $(($3 - $num)))]"
}

URLS=`cat $1`
NUM=`cat $1 | wc -l`
I=0
for URL in $URLS
do
s=`bar $I $NUM 30`
echo "$I/$NUM $s"
wistia $URL &> /dev/null
((++I))
done