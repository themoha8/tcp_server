#!/bin/bash

PROJECT=tcp_server

VERBOSITY=0

if [ "$1" = "-v" ]; then
	VERBOSITY=1
	shift
fi

run()
{
	if [ $VERBOSITY -gt 0 ]; then
		echo "$@";
	fi

	"$@" || exit 1
}

CFLAGS="-Wall -std=c99 -nostdlib -static"

SRCT=`ls tests/*.c`
SRC=`ls *.c`
HED=`ls *.h`

STARTTIME=`date +%s`

case $1 in
	'' | debug)
		run cc -o $PROJECT -O0 -g $CFLAGS $LDFLAGS $SRC
		;;
	clean)
		run rm -f $PROJECT *.o
		;;
	release)
		run cc -o $PROJECT -O2 -mavx2 $CFLAGS $LDFLAGS $SRC
		;;
	macro)
		run cc -E $LDFLAGS $CFLAGS $SRC
		;;
	disas | disasm)
		#for file in $SRC; do
			#run cc -o $file.disas -O0 -S $CFLAGS $LDFLAGS $file
			objdump -M intel -d $PROJECT > $PROJECT.s
		#done
		#cat *.disas > $PROJECT.s
		#rm -f *.disas
		;;
	fmt)
		if which indent > /dev/null; then
            for file in $SRC; do
                # -kr is Kernighan & Ritchie coding style, -ts4 is tabs.
                run indent -kr -ts4 -l120 $file
            done
            for file in $HED; do
                run indent -kr -ts4 -l120 $file
            done
            for file in $SRCT; do
                run indent -kr -ts4 -l120 $file
            done
            rm -f *.c~ *.h~ tests/*.c~
        fi
        ;;
	prof)
		run cc -o $PROJECT -O2 -mavx2 -fno-omit-frame-pointer $CFLAGS $LDFLAGS $SRC
		;;
	vet)
		if which clang-tidy >/dev/null; then
			# -* - disable all default checks, enable all clang-analyser-* checks
			# except c++ ones (-clang-analyzer-cplusplus*), enable perfomance checks.
			# -header-filter - what header files to analyze.
			CHECKS='-*,clang-analyzer-*,-clang-analyzer-cplusplus*,performance*'
			echo -n $CFLAGS $LDFLAGS | sed 's/ /\n/g' > compile_flags.txt
			if [ "$#" -gt 1 ]; then
				run clang-tidy -warnings-as-errors=$CHECKS --header-filter=.* --system-headers ./tests/${2}.c
			else
				run clang-tidy -warnings-as-errors=$CHECKS --header-filter=.* --system-headers *.c
			fi
			rm -f compile_flags.txt
		fi
			;;
	test)
			# SRCWMAIN=`echo $SRC | sed 's/main\.c//g'`
			# for file in $SRCT; do
                # run cc -o $file.out $CFLAGS $LDFLAGS $file $SRCWMAIN -lm
                # ./$file.out
                # rm -f $file.out
                # echo "-----------------------------------------"
            # done

			if [ "$#" -gt 1 ]; then
				if [ "$2" == "clean" ]; then
					run rm -f ./tests/*.out
				else
					run cc -I . -o ./tests/$2.out -O2 -mavx2 $CFLAGS $LDFLAGS ./tests/${2}.c $SRC
				fi
			else
				echo "Type a program name (<name_test> without .c) or clean for deleting *.out files"
				exit 1
			fi
			;;
esac

ENDTIME=`date +%s`

echo Done $1 in $((ENDTIME-STARTTIME))s
