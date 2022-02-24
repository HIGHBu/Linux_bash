#!/bin/bash
# 文件名: mv100K.sh
# 作者: 高晨熙 3190103302

for i in $( ls ) # 循环依次判断每⼀个⽂件
do
    if test -f $i # 判断是否为普通⽂件
    then
        let num=`stat -c %s $i `
        # echo "$num"
        if ((num > 102400))
        then
            mv $i ~/tmp
        fi
    fi
done