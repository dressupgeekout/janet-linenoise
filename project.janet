(declare-project
  :version "0.0.0"
  :name "linenoise")

(declare-native
  :name "linenoise"
  :source ["linenoise.c"]
  :cflags ["-I/Users/charlotte/pkg/include"]
  :ldflags ["-L/Users/charlotte/pkg/lib" "-llinenoise"])
