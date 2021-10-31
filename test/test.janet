# dressupgeekout's Linenoise test app

(import /build/linenoise :as linenoise)

(defn LINENOISE-COMPLETION
  "This is the only way I know how to hook into things."
  [lc buf]
  (linenoise/add-completion lc "foo")
  (linenoise/add-completion lc "bar")
  (linenoise/add-completion lc "baz"))

(linenoise/set-completion-callback LINENOISE-COMPLETION)

(def PROMPT "app> ")
(def HISTORY-FILE "/tmp/history.txt")

##### ##### #####

(linenoise/set-multi-line false)
(linenoise/history-set-max-len 10)
(linenoise/history-load HISTORY-FILE)

(linenoise/clear-screen)

(loop [line :iterate (linenoise/linenoise PROMPT)]
  (cond
    (= (string/ascii-lower line) "quit")
      (break)
    (do
      (print "You said: " line)
      (linenoise/history-add line)
      (linenoise/history-save HISTORY-FILE))))
