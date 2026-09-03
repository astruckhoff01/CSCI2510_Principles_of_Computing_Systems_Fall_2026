TOTAL_LINES=$(wc -l < access.log)
echo "LINES: $TOTAL_LINES"

echo "STATUS_COUNTS:"
awk '{print $2}' access.log | sort | uniq -c | sort -rn
