perl prep.pl < %1.lua > _tmpfile.$$$
luac -o %1.lob _tmpfile.$$$
erase _tmpfile.$$$

