#!/bin/bash

. ~/.bash_profile

echo "Please input start date:"
read start_date

now_date=`date -d "0 day ago" +%Y%m%d`
echo "now_date: "$now_date

start_day=$(date -d "$start_date" +%s)
echo "start_day: "$start_day

end_day=$(date -d "$now_date" +%s)
echo "end_day: "$end_day

sum_day=$(((end_day - start_day)/86400))

echo "sum_day: "$sum_day

for ((i=sum_day; i>=0; i--))
do
    add_date=`date -d "$i day ago" +%Y%m%d`
    end_date=`date -d "$(($i-1)) day ago" +%Y%m%d`
    echo "add_date: "$add_date
    echo "end_date: "$end_date

    cd $APPDIR
    CALTSMINSTAMT $add_date

    sleep 1
    CALINSTDAYSETT $add_date $end_date
    sleep 1
    CALINSTMONSETT $add_date $end_date
done
