#!/bin/bash
if [[ "$1" != "" ]] ; then
    compiler="$1"
else
    compiler="bin/c_compiler"
fi

have_compiler=0
if [[ ! -f bin/c_compiler ]] ; then
    >&2 echo "Warning : cannot find compiler at path ${compiler}. Only checking C reference against python reference."
    have_compiler=1
fi

input_dir="compiler_tests/control_flow"

working="tmp"
mkdir -p ${working}

for i in ${input_dir}/*_driver.c ; do
    base=$(echo $i | sed -E -e "s|${input_dir}/([^.]+)_driver[.]c|\1|g");
		echo ${base}

		$compiler -S ${input_dir}/${base}.c -o $working/${base}.s

		mips-linux-gnu-gcc -mfp32 -o $working/${base}.o -c $working/${base}.s

		mips-linux-gnu-gcc -mfp32 -static -o $working/${base} $working/${base}.o ${i}

		qemu-mips $working/${base}
		RES=$?
		if [[ ${RES} -ne 0 ]] ; then
			echo "$base, Fail, got $RES"
		else
			echo "$base, Pass"
		fi

done
