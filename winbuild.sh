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
	for iSrc in /usr/x86_64-w64-mingw32 /usr /
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
	# Выводим версию Qt
	for i in $(find / -name qmake 2> /dev/null | grep mingw)
	do
		$i --version
	done
	cd /opt/src
	if [ -d winbuild ]
	then
		rm -rf winbuild
	fi
	mkdir winbuild
	cd winbuild
	x86_64-w64-mingw32-cmake .. || ERROR 'Произошла ошибка на этапе cmake'
	if [ ${#build_targets[@]} -eq 0 ]
	then
		make || ERROR 'Не удалось собрать проект'
	else
		for i in ${build_targets[@]}
		do
			if [ "${i:0:1}" == '#' ]
			then
				continue
			fi
			make $i || ERROR "Не удалось собрать цель \"$i\""
		done
	fi
	mkdir bin || ERROR 'Не удалось создать директорию bin в сборочной-под-винду директории (winbuild)'
	for iExe in $(find . -name *.exe)
	do
		echo "
$iExe:"
		appName=$(basename "$iExe")
		appName=${appName:0:-4} # отрезаем ".exe" в конце имени программы
		mkdir bin/$appName || ERROR "Не удалось создать диркторию под бинарник \"$appName\ (источник - \"$iExe\")"
		cp $iExe bin/$appName/
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
			cp "$tmp" bin/$appName/
			stack=($(getDependencies $tmp) ${stack[@]})
		done
	done
	cd bin
	for i in *
	do
		if [ "$i" == a ]
		then
			continue
		fi
		cp a/*.dll $i/
		#cp -r /usr/{x86_64-w64-mingw32,i686-w64-mingw32}/{bin,lib/qt/plugins/{imageformats,iconengines,platforms}}/*.dll $i/
		cp -r /usr/x86_64-w64-mingw32/bin/*.dll $i/
		cp -r /usr/x86_64-w64-mingw32/lib/qt/plugins/* $i/
	done
	rm -r a
	echo "FINISHED
"
else
	pushd $(dirname $0) > /dev/null
		#docker run --rm -it -v $PWD:/opt/src -e INDOCKER=true burningdaylight/mingw-arch:qt /bin/bash /opt/src/winbuild_new.sh
		docker run --rm -it -v $PWD:/opt/src -e INDOCKER=true burningdaylight/mingw-arch:qt /bin/bash /opt/src/$(basename $0)
	popd > /dev/null
fi
