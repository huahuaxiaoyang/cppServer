#! /bin/bash
rootPath1="./log4cpp_makefile"
rootPath="."

# 统计天数
startDate="20230227"
endDate=$(date "+%Y%m%d")
# 将日期转成秒
startDateTime=`date -d "$startDate" +%s`
endDateTime=`date -d "$endDate" +%s`
# 两个日期秒数差
diff=$(($endDateTime - $startDateTime))
# 计算天数（一天24*60*60=86400秒）
diffCount=$(($diff/86400))
# DATETIME=$(date "+%Y-%m-%d_%H:%M:%S")
DATETIME=$(date "+%m_%d")

FILENAME="$rootPath/day$diffCount"_"$DATETIME"
OBJ_DIR="$FILENAME/bin"
LOG_DIR="$FILENAME/log"

# 创建文件夹
if [ -d ${FILENAME} ];then
    echo "dir: $FILENAME exist"
else
    mkdir "$FILENAME"
fi
# 创建可执行程序存放目录
#if [ -d ${OBJ_DIR} ];then
 #   echo "dir: $OBJ_DIR exist"
#else
#    mkdir "$OBJ_DIR"
#fi

# 创建Log文件存放目录
if [ -d ${LOG_DIR} ];then
    echo "dir: $LOG_DIR exist"
else
    mkdir "$LOG_DIR"
fi

# 移动Makefile
cp $rootPath1/Makefile $FILENAME/Makefile
cp $rootPath1/Log4cpp.hpp $LOG_DIR/Log4cpp.hpp
cp $rootPath1/log4cpp.conf $LOG_DIR/log4cpp.conf
cp $rootPath1/log4cppConf.cc $FILENAME/log4cppConf.cc
# 切换路径


cd $FILENAME

touch test.cc
echo -e "#include \"./log/Log4cpp.hpp\"\n#include <iostream>\nusing namespace std;\nint main(int argc, char const *argv[]) \n{\n\t\n\n\treturn 0;\n} " > test.cc