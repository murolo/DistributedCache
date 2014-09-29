mail()
{
    mail_title=$1
    mail_content="RT"
    mail_to="yinchunxiang@360.cn"
    echo -e "$mail_content" | mailx -s "${mail_title}" "${mail_to}"
}


check_disk()
{
    directory=$1
    percent=$(df -hP | grep ${directory} | grep -v grep | awk '{print $5}' | awk -F '%' '{print $1}')
    if (( percent > 60 )); then
        time_stamp=$(date +"%Y/%m/%d_%H:%M:%S")
        title="$(hostname) ${directory} used over ${percent}% @${time_stamp}"
        echo ${title}
        mail "${title}"
    fi
}

rotate_log()
{
    file_path=$(dirname $0)
    cd ${file_path}/../logs
    # delete file older than 10 days
    file_to_delete=$(find . -name "server.log.*" -mtime +10) 
    rm -f ${file_to_delete}

}

rotate_log 
check_disk " /data "

