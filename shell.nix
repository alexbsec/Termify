{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    go
    ffmpeg
    yt-dlp
    git
  ];

  shellHook = ''
    export GOPATH=$PWD/.gopath
    export GOBIN=$GOPATH/bin
    export PATH=$GOBIN:$PATH
    mkdir -p $GOBIN
    echo "🧪 termify dev environment ready."
  '';
}

