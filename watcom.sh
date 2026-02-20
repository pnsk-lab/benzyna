#!/bin/sh
INC=""
mkdir -p obj bin
for i in external/*; do
	INC="$INC -I $i"
done
for i in `find lib -name "*.c"`; do
	obj="obj/`echo "$i" | sed 's/\//_/g' | sed 's/\.c/.o/'`"

	if [ -f "$obj" ]; then
		continue
	fi

	owcc -bnt_dll -mtune=i486 -c -D_BENZYNA -DMA_YIELD_USE_NOP -DSTBI_NO_SIMD -I include $INC -o "$obj" "$i" || exit 1
done

owcc -bnt_dll -mtune=i486 -Wl,"option implib=obj/ba.lib" -o bin/ba.dll obj/*.o
