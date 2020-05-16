# locate the prebuilts folder

origin=$(pwd)
path=$(pwd)

function copyArch() {
  arch=$1
  arch_=$2
  if [[ -n "$arch" ]]
    then
      if [[ ! -e "$libsdir/$arch" ]]
        then
          mkdir "$libsdir/$arch"
      fi

      # copy same arch executables
      if [[ -e "$ffmpegdir/prebuilt/$arch_/ffmpeg/bin" ]]
        then
          echo "$ffmpegdir/prebuilt/$arch_/ffmpeg/bin found"
          if [[ ! -e "$assetsdir/usr" ]]
            then
              mkdir "$assetsdir/usr"
          fi
          if [[ ! -e "$assetsdir/usr/bin" ]]
            then
              mkdir "$assetsdir/usr/bin"
          fi
          if [[ ! -e "$assetsdir/usr/bin/$arch" ]]
            then
              mkdir "$assetsdir/usr/bin/$arch"
          fi
          for exe in $(find "$ffmpegdir/prebuilt/$arch_/ffmpeg/bin" -d 1 | xargs)
            do
              exe_basename="$(basename "$exe")"
              echo "copying .../$exe_basename"
              cp -P "$exe" "$assetsdir/usr/bin/$arch/$exe_basename"
              echo ".../$exe_basename -> $assetsdir/usr/bin/$arch/$exe_basename"
          done
      else
        echo "$ffmpegdir/prebuilt/$arch_/ffmpeg/bin not found"
      fi
      # copy same arch libs
      if [[ -e "$ffmpegdir/prebuilt/$arch_/ffmpeg/lib" ]]
        then
          echo "$ffmpegdir/prebuilt/$arch_/ffmpeg/lib found"
          if [[ ! -e "$assetsdir/usr" ]]
            then
              mkdir "$assetsdir/usr"
          fi
          if [[ ! -e "$assetsdir/usr/libs" ]]
            then
              mkdir "$assetsdir/usr/libs"
          fi
          if [[ ! -e "$assetsdir/usr/libs/$arch" ]]
            then
              mkdir "$assetsdir/usr/libs/$arch"
          fi
          for lib in $(find "$ffmpegdir/prebuilt/$arch_/ffmpeg/lib" -d 1 | grep \.so | xargs)
            do
              lib_basename="$(basename "$lib")"
              echo "copying .../$lib_basename"
              cp -P "$lib" "$libsdir/$arch/$lib_basename"
              echo ".../$lib_basename -> $libsdir/$arch/$lib_basename"
              cp -P "$lib" "$assetsdir/usr/libs/$arch/$lib_basename"
              echo ".../$lib_basename -> $assetsdir/usr/libs/$arch/$lib_basename"
          done
          echo "copying .../libcpufeatures.so"
          cp -P "$ffmpegdir/android/libs/$arch/libcpufeatures.so" "$libsdir/$arch/libcpufeatures.so"
          echo ".../$lib_basename -> $libsdir/$arch/libcpufeatures.so"
          cp -P "$ffmpegdir/android/libs/$arch/libcpufeatures.so" "$assetsdir/usr/libs/$arch/libcpufeatures.so"
          echo ".../$lib_basename -> $assetsdir/usr/libs/$arch/libcpufeatures.so"
          echo "copying .../libc++_shared.so"
          cp -P "$ffmpegdir/android/libs/$arch/libc++_shared.so" "$libsdir/$arch/libc++_shared.so"
          echo ".../$lib_basename -> $libsdir/$arch/libc++_shared.so"
          cp -P "$ffmpegdir/android/libs/$arch/libc++_shared.so" "$assetsdir/usr/libs/$arch/libc++_shared.so"
          echo ".../$lib_basename -> $assetsdir/usr/libs/$arch/libc++_shared.so"
      else
        echo "$ffmpegdir/prebuilt/$arch_/ffmpeg/lib not found"
      fi
  fi
}

function find_dir() {
  path=$(pwd)
  target=$1
  found_dir=""
  while [[ "$path" != "" && ! -e "$path/$1" ]]; do
    path=${path%/*}
  done
  if [[ "$path" == "" ]]
    then
      echo "$target directory could not be located"
      return 1
  else
    echo "$target directory located at $path/$target"
    found_dir="$path/$target"
  fi
  return 0
}
if ! find_dir ffmpeg
  then
    echo requires ffmpeg directory to exist somewhere in parent directories
    exit 1
fi
ffmpegdir="$found_dir"
if ! find_dir libs
  then
    echo requires libs directory to exist somewhere in parent directories, in order to copy libs
    exit 1
fi
libsdir="$found_dir"
if ! find_dir assets
  then
    echo requires assets directory to exist somewhere in parent directories, in order to copy ffmpeg
    exit 1
fi
assetsdir="$found_dir"

libs="$(ls "$ffmpegdir/android/libs")"
prebuilt="$(ls "$ffmpegdir/prebuilt")"
libs_arm__="$(echo "$libs" | grep arm)"
libs_x86__="$(echo "$libs" | grep x86)"
prebuilt_arm__="$(echo "$prebuilt" | grep arm)"
prebuilt_x86__="$(echo "$prebuilt" | grep x86)"

libs____arm="$(echo "$libs_arm__" | grep -v 64 | xargs)"
libs__arm64="$(echo "$libs_arm__" | grep 64 | xargs)"
libs____x86="$(echo "$libs_x86__" | grep -v 64 | xargs)"
libs_x86_64="$(echo "$libs_x86__" | grep 64 | xargs)"
prebuilt____arm="$(echo "$prebuilt_arm__" | grep -v 64 | xargs)"
prebuilt__arm64="$(echo "$prebuilt_arm__" | grep 64 | xargs)"
prebuilt____x86="$(echo "$prebuilt_x86__" | grep -v 64 | xargs)"
prebuilt_x86_64="$(echo "$prebuilt_x86__" | grep 64 | xargs)"

copyArch "$libs____arm" "$prebuilt____arm"
copyArch "$libs__arm64" "$prebuilt__arm64"
copyArch "$libs____x86" "$prebuilt____x86"
copyArch "$libs_x86_64" "$prebuilt_x86_64"