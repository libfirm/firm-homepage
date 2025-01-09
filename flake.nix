{
  description = "firm-homepage build environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    /*
    ycomp = {
      type = "file";
      url = "http://pp.ipd.kit.edu/firm/download/yComp-1.3.19.zip";
      flake = false;
      unpack = true;
      };
      */
  };

  outputs = inputs @ { self, ... }:
    inputs.flake-utils.lib.eachDefaultSystem (system: let
      pkgs = import (inputs.nixpkgs) { inherit system; };

      ycomp = stdenv.mkDerivation {
        name = "ycomp";
        src = pkgs.fetchzip {
          url = "http://pp.ipd.kit.edu/firm/download/yComp-1.3.19.zip";
          sha256 = "sha256-INfOay5TS/6YH6vGJG0W4+OyUOxLTLvDr2MxwJiSUyk=";
        };
        buildPhase = ''
          chmod +x ycomp
        '';
        installPhase = ''
          mkdir -p $out
          cp -rvp * $out
        '';
      };

      inherit (pkgs) stdenv;
    in {
      devShell = pkgs.mkShell {
        name = "firm-homepage-shell";
        buildInputs = with pkgs; [
          asciidoc
          doxygen
          graphviz
          (python312.withPackages (python-pkgs: [
            python-pkgs.pygments
          ]))
          jre8
          unzip
          libxml2
        ];
        /*
        env = {
          YCOMP = "${pkgs.ycomp.out}";
          };
          */
          shellHook = ''
    export YCOMP="${ycomp.out}/ycomp"
  '';

      };
    });
}
