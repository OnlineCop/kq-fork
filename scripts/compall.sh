for a in `cat allsc.txt`
do
  echo "Processing $a"
  perl prep.pl < $a.lua | ./luac5.1.exe -o $a.lob -
done  
