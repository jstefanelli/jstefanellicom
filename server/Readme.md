# JWX (John's Webserver eXperiment)

This is an experimental HTTP/1.1 server application to serve static files.

The project is set-up with CMake, and has no external dependencies.

It's target platforms are `Linux` and `Darwin`, however on MacOS it cannot be built with the version of `apple-clang` provided wit XCode. Please use a new-ish version of GCC or Clang.

## Building

```
cmake -B ./build .
cmake --build ./build
```

## Run

The programs accepts 2 parameters:

 - **--port [number]**: port for the webserver, default is **4955**
 - **--content-path [path]**: path to the content to serve, default is **`./content`**

```
jwx --content-path ../ui/build --port 8080
```