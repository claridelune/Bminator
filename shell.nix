{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = [
        pkgs.cmake
    ];

    shellHook = ''
        echo "nixpkgs inicializados"
    '';
}
