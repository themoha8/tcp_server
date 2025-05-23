#!/bin/bash

PROJECT=tcp_server

VERBOSITY=0
# Don't use now.
#VERBOSITYFLAGS=""

while [ "$1" = "-v" ]; do
	VERBOSITY=$((VERBOSITY+1))
	# concatenate strings
	#VERBOSITYFLAGS="$VERBOSITYFLAGS -v"
	shift
done

# Don't use now.
# run()
# {
# 	# "$@" - all argumetns (quotes preserve all spaces in arguments)
# 	# if "$@" then prog -v "   -v" equals -v    -v
# 	# if $@ then prog "   -v" equals -v -v
# 	if [ $VERBOSITY -gt 1 ]; then
# 		echo "$@";
# 	fi

# 	"$@" || exit 1
# }

# printv()
# {
# 	if [ $VERBOSITY -gt 0 ]; then
# 		echo "$@";
# 	fi
# }

run()
{
	# "$@" - all argumetns (quotes preserve all spaces in arguments).
	# if "$@" then prog -v "   -v" equals -v    -v.
	# if $@ then prog "   -v" equals -v -v.
	if [ $VERBOSITY -gt 0 ]; then
		echo "$@";
	fi

	"$@" || exit 1
}

CFLAGS="-Wall -Wextra -Wpedantic -Werror -ansi -std=c89"
LDFLAGS="-static -nostdlib"

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
		run cc -o $PROJECT -O3 -mavx2 $CFLAGS $LDFLAGS $SRC
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
				run indent -kr -ts4 $file
			done
			for file in $HED; do
				run indent -kr -ts4 $file
			done
			rm -f *.c~ *.h~
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
			if [ "$#" -gt 1 ]; then
				if [ "$2" == "clean" ]; then
					run rm -f ./tests/*.out
				else
					run cc -o ./tests/$2.out -O2 -mavx2 $CFLAGS $LDFLAGS ./tests/${2}.c $SRC
				fi
			else
				echo "Type a program name (<name_test> without .c) or clean for deleting *.out files"
				exit 1
			fi
			;;
esac

ENDTIME=`date +%s`

echo Done $1 in $((ENDTIME-STARTTIME))s
