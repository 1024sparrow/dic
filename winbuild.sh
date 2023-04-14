#!/bin/bash
# https://github.com/mdimura/docker-mingw-arch

pushd $(dirname $0)
	if [ -d winbuild ]
	then
		rm -rf winbuild
	fi
	docker run --rm -it -v $PWD:/opt/src burningdaylight/mingw-arch:qt /bin/bash -c "cd /opt/src && mkdir winbuild && cd winbuild && x86_64-w64-mingw32-cmake .. && make"
popd
