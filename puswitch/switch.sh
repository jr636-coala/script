#!/bin/bash
pactl set-default-sink "$1"
pactl list short sink-inputs|while read stream; do
    streamId=$(echo $stream|cut '-d ' -f1)
    pactl move-sink-input "$streamId" "$2"
done
