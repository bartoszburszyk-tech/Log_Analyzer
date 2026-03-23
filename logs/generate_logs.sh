#!/bin/bash

OUTPUT="${1:-sample_big.log}"
COUNT="${2:-10000}"

METHODS=("GET" "POST" "PUT" "DELETE" "PATCH")
ENDPOINTS=("/api/users" "/api/orders" "/api/payment" "/api/login" "/api/reports" "/admin" "/index.html" "/static/app.js" "/health" "/api/upload" "/api/users/99" "/api/orders/7")
STATUS_CODES=(200 200 200 200 301 304 400 401 403 404 404 500 500 503)
IPS=("192.168.1.1" "192.168.1.2" "10.0.0.5" "10.0.0.9" "172.16.0.1" "172.16.0.2" "8.8.8.8" "1.1.1.1")
UAS=("Mozilla/5.0" "curl/7.68" "Python/requests" "HealthChecker/1.0")

echo "[INFO] Generuję $COUNT wpisów do $OUTPUT..."

> "$OUTPUT"

for i in $(seq 1 "$COUNT"); do
    IP=${IPS[$RANDOM % ${#IPS[@]}]}
    METHOD=${METHODS[$RANDOM % ${#METHODS[@]}]}
    ENDPOINT=${ENDPOINTS[$RANDOM % ${#ENDPOINTS[@]}]}
    STATUS=${STATUS_CODES[$RANDOM % ${#STATUS_CODES[@]}]}
    SIZE=$((RANDOM % 9000 + 100))
    UA=${UAS[$RANDOM % ${#UAS[@]}]}

    # Losowa godzina
    HOUR=$(printf "%02d" $((RANDOM % 24)))
    MIN=$(printf "%02d" $((RANDOM % 60)))
    SEC=$(printf "%02d" $((RANDOM % 60)))

    echo "$IP - - [10/Oct/2024:$HOUR:$MIN:$SEC -0700] \"$METHOD $ENDPOINT HTTP/1.1\" $STATUS $SIZE \"-\" \"$UA\""
done >> "$OUTPUT"

echo "[INFO] Gotowe! Wygenerowano $COUNT wpisów w $OUTPUT"
