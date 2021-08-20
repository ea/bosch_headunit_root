for i in `cat message_queues.txt`
do
	ssh root@172.17.0.1 "/tmp/rtos_message_testing $i" >> logged_messages/$i.log &
	sleep 1
done

