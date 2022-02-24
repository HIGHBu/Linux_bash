#!/bin/bash
# ⽂件名: count.sh
# 作者: 高晨熙 3190103302
# 定义变量
let "total=0" # 普通⽂件字符总数
# 参数个数判断
if (( $# != 1)) 
then 
    echo "要求程序的参数个数是1个"
    exit 1
fi
# 进⼊指定的⽬录
if ! cd $1 
then
    echo "⽬录不存在或错误"
    exit 1
fi

for i in $( ls ) # 循环依次判断每⼀个⽂件
do
    if test -f $i # 判断是否为普通⽂件
    then
        let cnt=$(cat $i | wc -c) # 统计⽂件字符数
        let total+=cnt
    fi
done
echo "$1 ⽬录中包含："
echo "`ls -l $1 | grep  "^-" | wc -l ` 个普通⽂件，共$total 个字节" #统计普通文件数
echo "`ls -l $1 | grep  "^d" | wc -l`个⽬录⽂件 " #统计子目录数
echo "`ls  -F $1 | grep "*$" | wc -l`个可执⾏⽂件 " #统计可执行文件