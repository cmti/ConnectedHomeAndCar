#!/bin/bash

if [ $# -lt 12 ]; then
        echo "USAGE=$0 -URL URL -MESSAGE MSG -TOKEN TOKEN -TEL1 TELEPHONE_NUMBER1 -TEL2 TELEPHONE_NUMBER2 -POLLINTERVAL INTERVAL_SECONDS"
        exit 1
fi

while [ $# -gt 0 ]; do
        COMMAND=$1
        case $COMMAND in
        -URL)
                URL=$2
                shift 2
                ;;
        -MESSAGE)
                MSG=$2
                shift 2
                ;;
        -TOKEN)
                TOKEN=$2
                shift 2
                ;;
        -TEL1)
                TEL1=$2
                shift 2
                ;;
        -TEL2)
                TEL2=$2
                shift 2;
                ;;
        -POLLINTERVAL)
                INTERVAL_SECONDS=$2
                shift 2;
                ;;
        *) 
                break
                ;;
        esac 
done

((PAST_DURATION=10*INTERVAL_SECONDS))
((IDLE_DURATION=30*INTERVAL_SECONDS))

while [ true ]; do
curl -X GET ${URL} > /tmp/junk 2>/dev/null
LAST=`grep -i span /tmp/junk|grep -i "s ago" | cut -f2 -d\> | sed -e 's/s ago//g'|head -1`

DT=`date "+%Y-%m-%d-%H:%M:%S"`
if [ ! -z $LAST ]
then
        if [ $LAST -lt $PAST_DURATION ]
        then
                echo "$DT--send a SMS message to $TEL1 AND $TEL2"
                ./sendsms.sh -MESSAGE $MSG -TOKEN $TOKEN -TEL1 $TEL1 -TEL2 $TEL2
                sleep $IDLE_DURATION
        fi
fi
sleep $INTERVAL_SECONDS
echo "$DT ----nothing to do----"
done        
