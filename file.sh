#!/bin/bash
cd Downloads
touch details.txt
mkdir A
cd A
mkdir within-A
cp ../details.txt A
cp ../details.txt fake-details.txt
touch null-image.png
ls -l > TextFiles.txt
mv TextFiles.txt ~
cd ~
echo "IT WAS FUN :)" > goodbye.txt

# TASK 2
myvar="Hello"
echo $myvar myvar="Yes dear" 
echo $myvar myvar=7+5
echo $myvar
echo First argument : $1
echo Third argument : $3
#$ ./pshow one two three
echo Argument : $1
shift
echo Argument : $1
shift
echo Argument : $1

if [ $# -gt 0 ] ; 
then
echo " Your command l i n e contains $# arguments "
echo " All arguments displayed using  positio n a l parameter "

else 
echo " Your command l i n e contains no arguments "
fi

echo "Your f i r s t name please "
read fname
echo "Hello $fname , Lets be f r i e n d ! "

read first middle last
echo " Hello $first $middle $last "
echo "enter 100"; 
read count
if [ $count -eq 100 ] ; 
then
echo " Count is 100 "
fi



if [ $1 -gt 0 ]
then
echo " $1 number i s p o s i t i v e "
fi
if [ $1=hi ] ; then
echo " The first argument was hi "
fi

# S t r i ng Comparisons
string1=$1
if [ -z " $string1 " ] ; then
echo "NULL STRING";
fi
if [ -n " $string1 " ] ; then
echo "NOT NULL STRING"
fi

echo "Is it morning? Please answer yes or no ";
read timeofday
if [ $timeofday="yes" ] ; then 
echo "Good morning "
fi

count=99
if [ $count -eq 100 ]
then
echo " Count is 100 "
else
echo " Count is not 100 "
fi

if [ $# -eq 0 ]
then
echo " $0 : You must supply one i n t e g e r "
exit 1

fi
if test $1 -gt 0
then
echo "$1 i s p o s i t i v e number "
else
echo "$1 i s negative "
fi

echo "Is it morning? Please answer yes or no "
read timeofday
if [ $timeofday = "yes" ] ; then 
echo "Good morning "
elif [ $timeofday = "no" ] ; then 
echo " good afternoon "
else
echo "Sorry , $timeofday not recognized . Enter yes or no"

fi


echo "Is it morning? Please answer yes or no "
read timeofday
case "$timeofday" in 
yes) 
echo "Good Morning" ;; 
no)
echo "Good Afternoon " ;; 
y )
echo "Good Morning " ;;
n ) 
echo " Good Afternoon" ;;
∗ ) 
echo " Sorry , answer not recognized " ;;
esac

#clear
echo "h e l l o world " 
var1=10;
var2=20;
var3=$(($var1+$var2));
var4=$(($var1 - $var2)) ;
var5=$(($var1 * $var2)) ;
var6=$(($var1 / $var2)) ;
echo $var3;
echo $var4; 
echo $var5;
echo $var6; 





i=1
for day in Mon Tues Wed Thu F r i
do
echo "Weekday $((i++)) : $day "
done

for i in { 0 . . 1 0 . . 2 }
do
echo "Welcome $i times"
done


echo " Enter password "
read trythis
while [ " $trythis " != "secret" ] ; 
do 
echo "Sorry , try again"
read trythis
done

while [ $n -le 5 ]
do
echo " Welcome $n t imes ." 
n=$ (( n+1 ))
done

























