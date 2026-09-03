TOTAL_LINES=$(wc -l < access.log)
echo "LINES: $TOTAL_LINES"

echo "STATUS_COUNTS:"
# Added a space after awk, pointed it to the log, and fixed the sort pipeline
awk '{print $2}' access.log | sort | uniq -c | sort -rn
