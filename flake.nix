{
  description = "Flake for C++ Vulkan & GLFW development";

  outputs = { self, nixpkgs }:
  let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in
  {
    devShell.x86_64-linux = pkgs.mkShell {
      buildInputs = with pkgs; [
        valgrind
        bear
        glew
        libGL
        glfw
      ];
      shellHook = ''
        export LD_LIBRARY_PATH="${pkgs.libGL}/lib:${pkgs.glew}/lib:$LD_LIBRARY_PATH"
      '';
    };
  };
}
