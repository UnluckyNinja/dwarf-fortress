#!/bin/bash

root32=doc/coverage/x86_32/
root64=doc/coverage/x86_64/

esc_root32=${root32//\//\\\/}
esc_root64=${root64//\//\\\/}

if [ ! -d ./doc/coverage/ ]
then
  install -d $root32
  install -d $root64

  lcov --directory ./bin --zerocounters

  cp -rf ./g_src ./src ./include $root64
  eval `find ./bin -iname "*.gc*" | grep -v address-model-32 | sed -re "s/(.*)(threading-multi\/)(.*)/install -D \1\2\3 $esc_root64\3;/g"`
  lcov --directory $root32 --zerocounters

  cp -rf ./g_src ./src ./include $root32
  eval `find ./bin -iname "*.gc*" | grep address-model-32 | sed -re "s/(.*)(threading-multi\/)(.*)/install -D \1\2\3 $esc_root32\3;/g"`
  lcov --directory $root64 --zerocounters

  lcov --capture --directory $root32 --base-directory $root32 --initial --output-file $root32/libgraphics_base.info
  lcov --capture --directory $root64 --base-directory $root64 --initial --output-file $root64/tetris_base.info

else
  eval `find ./bin -iname "*.gc*" | grep -v address-model-32 | sed -re "s/(.*)(threading-multi\/)(.*)/install -D \1\2\3 $esc_root64\3;/g"`
  eval `find ./bin -iname "*.gc*" | grep address-model-32 | sed -re "s/(.*)(threading-multi\/)(.*)/install -D \1\2\3 $esc_root32\3;/g"`

  lcov --capture --directory $root32 --base-directory $root32 --output-file $root32/libgraphics_extra.info
  lcov --capture --directory $root64 --base-directory $root64 --output-file $root64/tetris_extra.info

  lcov --add-tracefile $root32/libgraphics_base.info --add-tracefile $root32/libgraphics_extra.info --output-file $root32/libgraphics.info
  lcov --add-tracefile $root64/tetris_base.info      --add-tracefile $root64/tetris_extra.info      --output-file $root64/tetris.info

  lcov --extract $root32/libgraphics.info "*$root32*" --output-file $root32/libgraphics.info
  lcov --extract $root64/tetris.info      "*$root64*" --output-file $root64/tetris.info
  lcov --remove $root32/libgraphics.info "*boost/logging*" --output-file $root32/libgraphics.info
  lcov --remove $root64/tetris.info      "*boost/logging*" --output-file $root64/tetris.info

  genhtml --demangle-cpp --css coverage.css --no-branch-coverage --num-spaces 2 --output-directory $root32 $root32/libgraphics.info
  genhtml --demangle-cpp --css coverage.css --no-branch-coverage --num-spaces 2 --output-directory $root64 $root64/tetris.info
fi

#files=`ls -d *.gcno`

#gcov -a -b -f g_src/${files//.gcno/.cpp} 2>&1 #-o ../ *
#lcov -c -d ./ -o dwarf_full.info
#lcov -r dwarf_full.info "/usr/include/*" "*bits" "ext"  -o dwarf.info
#genhtml --demangle-cpp -s --num-spaces 2 dwarf.info

