for a in `cat allsc.txt`
do
  echo "Processing $a"
  perl prep.pl < $a.lua | ./luac.exe -o $a.lob -
done  
