# A script to run the Landlock selftests in Qemu

## Usage

* Install the `bluebox` tool: `go install github.com/florianl/bluebox`.
  Make sure it's in the $PATH (Install location is `~/go/bin`)
* Look at the `landlock-selftests` script and modify the paths at the top.
  (Patches to make the script accept these as flags are welcome... 😬)
* Symlink `landlock-selftests` somewhere from your `~/bin` or so.
