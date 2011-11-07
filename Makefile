#
# ----- The Scout Programming Language
#
# This file is distributed under an open source license by Los Alamos
# National Security, LCC.  See the file LICENSE.txt for details. 
# 

.PHONY : default
.PHONY : build 
.PHONY : test

arch        := $(shell uname -s)
date        := $(shell /bin/date "+%m-%d-%Y")
build_dir   := $(CURDIR)/build
cmake_flags := -DCMAKE_BUILD_TYPE=DEBUG  -DCMAKE_INSTALL_PREFIX=$(build_dir)

# Attempt to figure out how many processors are available for a parallel build.
# Note that this could be fragile across platforms/operating systems.
arch := $(shell uname -s)

ifeq ($(arch),Darwin) 
  sysprof = /usr/sbin/system_profiler -detailLevel mini SPHardwareDataType
  nprocs := $(shell ($(sysprof) | /usr/bin/grep -i cores | /usr/bin/awk '{print $$ NF}'))
else 
  nprocs := $(shell /bin/cat /proc/cpuinfo | /bin/grep processor | /usr/bin/wc -l)
endif 

# Since we're likely I/O bound try and sneak in twice as many build
# threads as we have cores...  If you have problems running out of
# memory you might need to back off on the factor of 2...
nprocs := $(shell expr $(nprocs) \* 2)

all: $(build_dir)/Makefile compile
.PHONY: all 

$(build_dir)/Makefile: CMakeLists.txt
	@((test -d $(build_dir)) || (mkdir $(build_dir)))
	@(cd packages/hwloc; ./configure --prefix=$(build_dir))
	@(cd packages/hwloc; make -j $(nprocs); make install)	
	@(cd packages/libpng-1.5.4; ./configure --prefix=$(build_dir))
	@(cd packages/libpng-1.5.4; make -j $(nprocs); make install)
	@(cd $(build_dir); cmake $(cmake_flags) ..;)

.PHONY: compile
compile: $(build_dir)/Makefile 
	@(cd $(build_dir); make -j $(nprocs); make install)

.PHONY: xcode
xcode:;
	@((test -d xcode) || (mkdir xcode))
	@(cd xcode; cmake -G Xcode ..)

.PHONY: clean
clean:
	-@/bin/rm -rf $(build_dir)
	-@/usr/bin/find . -name '*~' -exec rm -f {} \{\} \;
	-@/usr/bin/find . -name '._*' -exec rm -f {} \{\} \;
	-@/usr/bin/find . -name '.DS_Store' -exec rm -f {} \{\} \;
	-@((test -f packages/hwloc/Makefile) && (cd packages/hwloc; make distclean > /dev/null))
	-@((test -f packages/libpng-1.5.4/Makefile) && (cd packages/libpng-1.5.4; make distclean > /dev/null))

