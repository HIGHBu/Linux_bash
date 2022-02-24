#!/bin/bash
# 文件名: dirsync.sh
# 作者: 高晨熙 3190103302

function del_reduance(){
    for file in `ls $2`  #遍历dest中的文件
    do
        if [ -e "$1/$file" ]
        then 
            if [ -d "$1/$file" ]  #如果在source中存在且是目录
            then 
                del_reduance "$1/$file" "$2/$file" #进目录递归
            fi
        else #在source中不存在
            rm -rf "$2/$file"     #把它在dest中删了
        fi
    done
}

if (($#!=2))  #如果不是两个参数的话，报错
then
    echo 'We need two parameters! One for source and one for destination'
    echo 'use it like: ./dirsync.sh source dest'
    exit 1
fi
echo 'backup&sync'
cp -aTu $1 $2  #把source复制给dest（修改时间早的就不替换了）
#-a：在复制目录时使用，它保留链接、文件属性，并复制目录下的所有内容。
#-u: 根据修改时间更新
#-T: 把目标当成一个普通文件
del_reduance $1 $2
#保证dest里没有比source多的文件
cp -aTu $2 $1 #把dest里修改时间晚的复制给source
exit 0