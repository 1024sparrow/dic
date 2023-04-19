#!/bin/bash
# https://github.com/mdimura/docker-mingw-arch

declare -a dependList stack
declare parent tmp
declare -i n=0
declare -a build_targets

function ERROR {
	echo "Error: $1"
	exit 1
}

[ -r $(dirname $0)/winbuild_targets ] || ERROR 'файл winbuild_targets не найден (или не доступен для чтения)'
build_targets=($(cat $(dirname $0)/winbuild_targets))
for tmp in ${build_targets[@]}
do
	if [ "${tmp:0:1}" == '#' ] # оставляем возможность комментировать цели сборки
	then
		continue
	fi
	[[ "$tmp" =~ ^[[:alpha:]] ]] || ERROR "Цель сборки под Винду \"$tmp\" имеет недопустимое имя. Прервано"
done
echo "boris debug: ${build_targets[@]}"

function inList {
	local i state=0 target
	for i in $@
	do
		if [ $state == 0 ]
		then
			state=1
			target=$i
		else
			if [ "$i" == "$target" ]
			then
				return 0
			fi
		fi
	done
	return 1
}

function getDependencies {
	local i
	for i in $(strings $1 | grep -i '\.dll$' | grep -i '^[[:alpha:]]')
	do
		if [ "$i" == "$1" ]
		then
			continue
		fi
		echo $i
	done
}

function findLibByName {
# Arguments:
# 1. target library name
# 2. variable name to store

	local i iSrc
	for iSrc in /usr/x86_64-w64-mingw32 /usr
	do
		for i in $(find $iSrc -name $1 2> /dev/null)
		do
			eval "$2=$i"
			return 0
		done
	done
	return 1
}

if [ "$INDOCKER" == true ]
then
	cd /opt/src
	cd winbuild
	#x86_64-w64-mingw32-cmake .. && make || ERROR 'Ошибка сборки'
	for iExe in $(find . -name *.exe)
	do
		echo "
$iExe:"
		stack=($(getDependencies $iExe))
		while [ ${#stack[@]} -gt 0 ]
		do
			parent=${stack[0]}
			stack=(${stack[@]:1})

			if inList $parent "${dependList[@]}"
			then
				continue
			else
				dependList=(${dependList[@]} $parent)
			fi

			if ! findLibByName $parent tmp
			then
				echo "  * $parent (не найдена)"
				continue
			fi
			#echo "boris: $parent $tmp"
			echo "  * $tmp"
			stack=($(getDependencies $tmp) ${stack[@]})
		done
	done
	echo "FINISHED
"
else
	pushd $(dirname $0) > /dev/null
		docker run --rm -it -v $PWD:/opt/src -e INDOCKER=true burningdaylight/mingw-arch:qt /bin/bash /opt/src/winbuild_new.sh
	popd > /dev/null
fi
