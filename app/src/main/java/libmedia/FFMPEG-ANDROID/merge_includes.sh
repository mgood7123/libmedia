# locate the prebuilts folder

if [[ -e include ]]
  then
    echo cleaning include folder
    rm -rf include
fi

origin=$(pwd)
path=$(pwd)
ffmpegdir=ffmpeg/prebuilt
while [[ "$path" != "" && ! -e "$path/$ffmpegdir" ]]; do
  path=${path%/*}
done
if [[ "$path" == "" ]]
  then
    echo "$ffmpegdir directory could not be located"
else
  echo "$ffmpegdir directory located at $path/$ffmpegdir"
  ffmpegdir="$path/$ffmpegdir"
  cd "$ffmpegdir" 2>/dev/null
  if [[ $? != 0 ]]
    then
      echo failed to cd into "$ffmpegdir"
      echo exiting
      exit 1
  fi
  for arch in $(ls "$ffmpegdir" | sed s/android\-aar// | xargs)
    do
      if [[ ! -e "$origin/include/$arch" ]]
        then
          echo "creating directory: include/$arch"
          mkdir -p "$origin/include/$arch"
      fi
      if [[ ! -e "$origin/include/$arch/ffmobile-headers" ]]
        then
          echo "creating directory: include/$arch/ffmobile-headers"
          mkdir -p "$origin/include/$arch/ffmobile-headers"
      fi
      echo "copying ffmobile headers"
      cp "$origin/cpp/fftools_cmdutils.h" "$origin/include/$arch/ffmobile-headers"
      cp "$origin/cpp/fftools_ffmpeg.h" "$origin/include/$arch/ffmobile-headers"
      cp "$origin/cpp/mobileffmpeg.h" "$origin/include/$arch/ffmobile-headers"
      cp "$origin/cpp/mobileffmpeg_abidetect.h" "$origin/include/$arch/ffmobile-headers"
      cp "$origin/cpp/mobileffmpeg_exception.h" "$origin/include/$arch/ffmobile-headers"
      cp "$origin/cpp/mobileffprobe.h" "$origin/include/$arch/ffmobile-headers"
      echo "ffmobile headers -> $origin/include/$arch/ffmobile-headers"
      for dir in $(find "$arch" | grep include$ | xargs)
        do
          echo "copying .../$dir"
          for f in $(ls "$dir" | xargs)
            do
              cp -RPi "$dir/$f" "$origin/include/$arch"
          done
          echo ".../$dir -> $origin/include/$arch"
      done
    done
fi
#for apk in $(find . | grep \\.apk$ | xargs)
#  do
#    echo "removing: $apk"
#    rm "$apk"
#    echo "removed :  $apk"
#done