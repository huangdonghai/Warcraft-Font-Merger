# 构建和打包环境

## Windows

使用 MSYS2 环境。

操作步骤：

1. 安装对应环境中的 xmake、工具链、7zip。
1. 执行 `package/windows-x64.sh`（或 `*-arm64.sh`、`*-32.sh`）。

## macOS

使用 XCode 12.4 工具链。

操作步骤：

1. 安装 CMake 并安装命令行工具。
1. 安装 XCode Command Line Tools。
1. 执行 `package/mac-x86.sh`（或 `*-arm.sh`）。

## Linux x86-64

使用 Alpine musl 环境。

操作步骤：

1. 安装 `bash`、`xmake`、`gcc`、`g++`、`xz`。

   ```sh
   apk add bash xmake gcc g++ xz
   ```

1. 执行 `package/linux-amd64.sh`。

注：推荐通过 Podman 启动 Alpine musl 环境。

```sh
# 启动 Alpine musl 环境
podman run -it --rm -v $PWD:/mnt -v /mnt -e XMAKE_ROOT=y alpine

# 如果需要切换镜像站
sed -i "s|dl-cdn.alpinelinux.org|mirrors.ustc.edu.cn|" /etc/apk/repositories
```
