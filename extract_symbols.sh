#!/bin/bash

./bootstrap

objdump -Tw dist/dwarf-fortress.bin \
  | egrep -v "^$|^DYNAMIC|file format elf32-i386" \
  | awk -F $'\t' '{ printf("%-25s%s\n", $1, $2); }' \
  | awk 'BEGIN { FIELDWIDTHS="8 1 1 1 1 1 1 1 1 1 7 1 8 2 5000" } { gsub(/ /, "", $11); gsub(/ +/, "|", $15); printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $1, $3, $4, $5, $6, $7, $8, $9, $11, $13, $15); }' \
  | tr -d ' ' \
  | awk -F'|' '{ if(!$11) $11="Extern"; printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $11, $9, $2, $3, $4, $5, $6, $7, $8, $1, $10, $12); }' \
  | sort \
  | uniq \
  | egrep "^Extern" \
  | cut -d'|' -f 12- \
  > generated/dwarf-fortress.extern.sym

objdump -Tw dist/dwarf-fortress.bin \
  | egrep -v "^$|^DYNAMIC|file format elf32-i386" \
  | awk -F $'\t' '{ printf("%-25s%s\n", $1, $2); }' \
  | awk 'BEGIN { FIELDWIDTHS="8 1 1 1 1 1 1 1 1 1 7 1 8 2 5000" } { gsub(/ /, "", $11); gsub(/ +/, "|", $15); printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $1, $3, $4, $5, $6, $7, $8, $9, $11, $13, $15); }' \
  | tr -d ' ' \
  | awk -F'|' '{ if(!$11) $11="Extern"; printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $11, $9, $2, $3, $4, $5, $6, $7, $8, $1, $10, $12); }' \
  | sort \
  | uniq \
  | egrep "^Base" \
  | cut -d'|' -f 12- \
  > generated/dwarf-fortress.base.sym

git checkout upstream/`cat version` libs/libgraphics.so -q
git reset -q

objdump -Tw libs/libgraphics.so \
  | egrep -v "^$|^DYNAMIC|file format elf32-i386" \
  | awk -F $'\t' '{ printf("%-25s%s\n", $1, $2); }' \
  | awk 'BEGIN { FIELDWIDTHS="8 1 1 1 1 1 1 1 1 1 7 1 8 2 5000" } { gsub(/ /, "", $11); gsub(/ +/, "|", $15); printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $1, $3, $4, $5, $6, $7, $8, $9, $11, $13, $15); }' \
  | tr -d ' ' \
  | awk -F'|' '{ if(!$11) $11="Extern"; printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $11, $9, $2, $3, $4, $5, $6, $7, $8, $1, $10, $12); }' \
  | sort \
  | uniq \
  | egrep "^Extern" \
  | cut -d'|' -f 12- \
  > generated/libgraphics.extern.sym

objdump -Tw libs/libgraphics.so \
  | egrep -v "^$|^DYNAMIC|file format elf32-i386" \
  | awk -F $'\t' '{ printf("%-25s%s\n", $1, $2); }' \
  | awk 'BEGIN { FIELDWIDTHS="8 1 1 1 1 1 1 1 1 1 7 1 8 2 5000" } { gsub(/ /, "", $11); gsub(/ +/, "|", $15); printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $1, $3, $4, $5, $6, $7, $8, $9, $11, $13, $15); }' \
  | tr -d ' ' \
  | awk -F'|' '{ if(!$11) $11="Extern"; printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", $11, $9, $2, $3, $4, $5, $6, $7, $8, $1, $10, $12); }' \
  | sort \
  | uniq \
  | egrep "^Base" \
  | cut -d'|' -f 12- \
  > generated/libgraphics.base.sym

rm libs/libgraphics.so
rmdir libs/ 2>/dev/null

echo -e "{\n  global:" > generated/symbols.txt
cat generated/dwarf-fortress.extern.sym | sed -re 's/^(.*)$/    \1;/g' >> generated/symbols.txt
echo -e "  local:\n    *;\n};" >> generated/symbols.txt

cat generated/libgraphics.extern.sym | c++filt \
  | sed -re 's/^(.*)$/\1;/g' \
  | sed -re 's/std::basic_string<char, std::char_traits<char>, std::allocator<char> >/::std::string/g' \
  | sed -re 's/std::set<long, std::less<long>, std::allocator<long> >/::std::set< long >/g' \
  | sed -re 's/svector<char\*, std::allocator<char\*> >/svector< char* >/g' \
  | sort > generated/include/libgraphics_externals.h

cat generated/dwarf-fortress.base.sym | c++filt \
  | sed -re 's/^(.*)$/\1;/g' \
  | sed -re 's/std::basic_string<char, std::char_traits<char>, std::allocator<char> >/::std::string/g' \
  | sed -re 's/std::set<long, std::less<long>, std::allocator<long> >/::std::set< long >/g' \
  | sed -re 's/svector<char\*, std::allocator<char\*> >/svector< char* >/g' \
  | sed -re 's/<char, std::char_traits<char>, std::allocator<char> >//g' \
  | grep -v '^__bss_start' \
  | grep -v '^_edata' \
  | grep -v '^_end' \
  | grep -v '^_fini' \
  | grep -v '^_init' \
  | grep -v '^_IO_stdin_used' \
  | sort > generated/include/precompiled_internals.h

cat generated/dwarf-fortress.extern.sym | c++filt \
  | sed -re 's/^(.*)$/\1;/g' \
  | sed -re 's/std::basic_string<char, std::char_traits<char>, std::allocator<char> >/::std::string/g' \
  | sed -re 's/std::set<long, std::less<long>, std::allocator<long> >/::std::set< long >/g' \
  | sed -re 's/svector<char\*, std::allocator<char\*> >/svector< char* >/g' \
  | sort > generated/include/libgraphics_symbols.h

git diff generated/include/libgraphics_symbols.h generated/include/precompiled_internals.h
