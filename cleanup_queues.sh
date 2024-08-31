echo "Running clean up queues"
echo "Queues:"
ipcs -q

QUEUES=$(ipcs -q | awk '{ print  }'  | tail -n +4 )

echo $QUEUES

echo "Deleting above"

for id in $QUEUES; do sudo ipcrm -q $id && echo "Deleted queue with ID: $id" || echo "Failed to delete queue with ID: $id"; done

