#!/bin/bash
# 文件名: printfile.sh
# 作者: 高晨熙 3190103302

# 参数个数判断
if (( $# != 1)) 
then 
    echo "要求程序的参数个数是1个"
    exit 1
fi
if test -f $1 # 判断是否为普通⽂件
    then
        echo "文件所有者：`stat -c %U $1 `"
        echo "最后的修改日期：`stat -c %y $1`"
fi