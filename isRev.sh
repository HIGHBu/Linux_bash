#!/bin/bash
# 文件名: isRev.sh
# 作者: 高晨熙 3190103302

read -p "请输一个要判断的字符串：" STR # 读入一个字符串
str=""
RES=""
for ((a=0; a < ${#STR}; ++a));
do

	if echo ${STR:$a:1} | grep "[a-zA-Z]" >/dev/null
	then
		str=$str${STR:$a:1}
	fi
done
# echo "str为：$str"
for ((a=${#str};a>=0; --a));
do
	RES=$RES${str:$a:1}
done
# echo "RES为：$RES"

if test $str = $RES # 如果反转后的字符串和源字符串相同，则说明是回文，否则不是
then
    echo "yes"
else
    echo "no"
fi
