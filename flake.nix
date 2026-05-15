{
  description = "Flake for C++ Vulkan & GLFW development";

  outputs = { self, nixpkgs }:
  let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in
  {
    devShell.x86_64-linux = pkgs.mkShell {
      buildInputs = with pkgs; [
        bear
        glfw
        vulkan-headers
        vulkan-loader
        vulkan-tools 
      ];
      shellHook = ''
        export CPLUS_INCLUDE_PATH="${pkgs.vulkan-headers}/include:$CPLUS_INCLUDE_PATH"
        export LD_LIBRARY_PATH=${pkgs.vulkan-loader}/lib:$LD_LIBRARY_PATH
      '';
    };
  };
}
