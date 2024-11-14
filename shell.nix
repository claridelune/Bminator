{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = [
        pkgs.cmake
        pkgs.python312
        pkgs.python312Packages.networkx
        pkgs.python312Packages.matplotlib
        pkgs.python312Packages.pygraphviz
    ];

    shellHook = ''
        echo "nixpkgs inicializados"
    '';
}
