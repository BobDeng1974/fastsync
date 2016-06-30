# fastsync
    fast (real time) synchronize bulky files between servers.

# project home dir
    ${fastsync}

# prepare for build
    Below packages shuld be installed beforw build fastsync:

    # mxml-2.9
        $ cd ${fastsync}/prepare
        $ tar -zxf mxml-2.9.tgz
        $ cd mxml-2.9/
        $ ./configure
        $ make && sudo make install

    # expat
        $ tar -zxf expat-2.1.0.tar.gz
        $ cd expat-2.1.0
        $ ./configure
        $ make && sudo make install

    # jemalloc
        $ tar -xf jemalloc-3.6.0.tar.bz2
        $ cd jemalloc-3.6.0
        $ ./configure
        $ make && sudo make install

    # log4c
        $ tar -zxf log4c-1.2.4.tar.gz
        $ cd log4c-1.2.4
        $ ./configure --without-expat
        $ make && sudo make install

    # sqlite3
        $ tar -zxf sqlite-autoconf-3130000.tar.gz
        $ cd sqlite-autoconf-3130000
        $ ./configure
        $ make && sudo make install
