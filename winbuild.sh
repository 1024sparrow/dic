#!/bin/bash
# https://github.com/mdimura/docker-mingw-arch

function ERROR {
	echo "Error: $1"
	exit 1
}

if [ "$INDOCKER" == true ]
then
	cd /opt/src
	if [ -d winbuild ]
	then
		rm -rf winbuild
	fi
	mkdir winbuild
	cd winbuild
	x86_64-w64-mingw32-cmake .. && make || ERROR 'Ошибка сборки'
	for iExe in $(find . -name *.exe)
	do
		echo "
	$iExe"
		for iDll in $(strings $iExe | grep -i '\.dll$')
		do
			for iSrc in $(find /usr/x86_64-w64-mingw32 -name $iDll)
			do
				echo "$iSrc"
				cp $iSrc $(dirname $iExe)/ || ERROR 'Не удалось скопировать библиотеку зависимости'
			done
		done
	done
else
	pushd $(dirname $0) > /dev/null
		if [ -d winbuild ]
		then
			rm -rf winbuild
		fi
		docker run --rm -it -v $PWD:/opt/src -e INDOCKER=true burningdaylight/mingw-arch:qt /bin/bash /opt/src/winbuild.sh
	popd > /dev/null
fi
