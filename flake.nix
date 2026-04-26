{
  # Needs cmake compatible with 3.0, nixos-25.11 has cmake 4.x which is only
  # compatible with cmake files from ≥3.5.
  inputs.nixpkgs.url = "github:nixos/nixpkgs/nixos-25.05";

  outputs = { flake-parts, ... }@inputs: flake-parts.lib.mkFlake { inherit inputs; } {
    systems = ["aarch64-darwin" "x86_64-linux" "aarch64-linux" ];
    perSystem = { pkgs, system, ... }: {
      packages.default = pkgs.stdenv.mkDerivation {
        nativeBuildInputs = [ pkgs.cmake ];
        buildInputs = [ pkgs.raylib ];
        name = "pacman_raylib";
        src = ./.;
      };
    };
  };
}
