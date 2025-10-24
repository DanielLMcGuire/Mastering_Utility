#!/usr/bin/env bash

apps=( "MasteringUtility" "MasteringWizard" )
mkdir -p dist

if [ ! -x ./appimagetool-x86_64.AppImage ]; then
  echo "Downloading appimagetool..."
  wget -q --show-progress "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
  chmod +x ./appimagetool-x86_64.AppImage
fi

for app in "${apps[@]}"; do
  echo "---- $app ----"

  src="../build/$app"
  if [ ! -f "$src" ]; then
    echo "SKIP $app — binary not found at $src"
    continue
  fi
  if [ ! -x "$src" ]; then
    echo "Making $src executable"
    chmod +x "$src" || { echo "chmod failed"; }
  fi

  appdir="${app}.AppDir"
  rm -rf "$appdir"
  mkdir -p "$appdir/bin"

  echo "Copying binary"
  cp "$src" "$appdir/bin/$app" || { echo "COPY failed for $app"; continue; }
  chmod +x "$appdir/bin/$app"

  # AppRun
  cat > "$appdir/AppRun" <<'EOF'
#!/bin/bash
HERE="$(dirname "$(readlink -f "${0}")")"
exec "$HERE/bin/'"$app"'" "$@"
EOF

  sed -i "s/'\"$app\"'/$app/" "$appdir/AppRun"
  chmod +x "$appdir/AppRun"

  cat > "$appdir/$app.desktop" <<EOF
[Desktop Entry]
Name=$app
Exec=bin/$app
Icon=$app
Type=Application
Categories=Utility;
EOF
    # dummy icon (1x1 px png)
  base64 -d > "$appdir/$app.png" <<'PNGBASE64'
iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR4nGNgYAAAAAMAASsJTYQAAAAASUVORK5CYII=
PNGBASE64

  out="dist/${app}-x86_64.AppImage"
  echo "Running appimagetool -> $out"
  ./appimagetool-x86_64.AppImage "$appdir" "$out"
  rc=$?
  if [ $rc -ne 0 ]; then
    echo "appimagetool failed for $app (exit $rc) — continuing to next app"
    continue
  fi

  echo "Built $out"
done

echo "All done. Check ./dist/"