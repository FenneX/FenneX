# Versions

Current cocos2d-x comes with 1.1.0c. That's way too old and do not support Android build.
We use version 1.1.1w, which is the latest 1.1.1 version

# Building

After downloading and uncompression openssl, go into the folder.
This assume you have NDK_ROOT configured

## Initial config

```
export ANDROID_NDK_HOME=$NDK_ROOT
export CXXFLAGS="-fPIC -Os"
export CPPFLAGS="-DANDROID -fPIC -Os"
PATH=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64/bin:$PATH
```

## Build for arm-v7a

```
make distclean 
./Configure android-arm -D__ANDROID_API__=29 -static no-asm no-engine no-gost no-shared no-ssl no-tests no-zlib no-ui-console no-stdio
make build_libs
```
You can copy libcrypto.a and libssl.a, which are built for arm-v7a only. Do the same for arm64.

## Build for arm64-v8a

Same as for arm but with arm64.

```
make distclean 
./Configure android-arm64 -D__ANDROID_API__=29 -static no-asm no-engine no-gost no-shared no-ssl no-tests no-zlib no-ui-console no-stdio
make build_libs
```

# Acknowledgements

Based in part on https://github.com/217heidai/openssl_for_android/blob/master/openssl_build_new.sh
And in part in asking ChatGPT for help.

