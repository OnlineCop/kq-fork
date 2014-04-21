perl prep.pl < %1.lua > _tmpfile.$$$
luac5.1 -o %1.lob _tmpfile.$$$
erase _tmpfile.$$$

