#!/bin/bash

if [ $# -lt 8 ]; then
        echo "USAGE=$0 -MESSAGE MSG -TOKEN TOKEN -TEL1 TELEPHONE_NUMBER1 -TEL2 TELEPHONE_NUMBER2"
        exit 1
fi

while [ $# -gt 0 ]; do
        COMMAND=$1
        case $COMMAND in
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
        *)
                break
                ;;
        esac
done

echo "{\"Message\":\"$MSG\",\"Address\":[\"tel:$TEL1\",\"tel:$TEL2\"]}" > smscontent

curl -i -X POST "https://api.att.com/rest/sms/2/messaging/outbox" \
    --header "Content-Type: application/json" \
    --header "Authorization: Bearer $TOKEN" \
    --header "Accept: application/json" \
    --data @smscontent
