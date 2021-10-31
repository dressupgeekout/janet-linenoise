(declare-project
  :version "0.0.0"
  :name "linenoise"
  :author "Charlotte Koch"
  :license "MIT"
  :url "https://github.com/dressupgeekout/janet-linenoise"
  :repo "git+https://github.com/dressupgeekout/janet-linenoise.git")

(declare-native
  :name "linenoise"
  :source ["linenoise.c"
           "j_linenoise.c"])
