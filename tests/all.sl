(test=, 42 isa Int; t)
(test=, Int Num? t)

(test=, 42 nil? f)
(test=, nil nil? t)
(test=, nil bool; f)

(test=, t bool; t)
(test=, f bool; f)

(test=, 0 bool; f)
(test=, 1 bool; t)

(test=, 1 = 1; t)
(test=, 1 = 3; f)

(test= 1 1? t)
(test= 1 3? f)

(test= 'foo 'foo? t)
(test= 'foo 'bar? f)

(test=, 1 < 3; t)
(test=, 3 < 1; f)

(test=, 3.14 int; 3)

(test=, 1 + 3; 4)
(test= (1 +, 3 * 2) 7)
(test= (1 + (3 * 2)) 7)
(test= (1 +, 5 - 2) 4)

(test= ''foo'bar'' ''foo'bar'')

(test=, ''abc'' iter; call! #a)

(test=, 1 3 5 drop! +; 4)
(test=, 21 dup! +; 42)
(test=, 1 3 swap! -; 2)
(test= (1 2 3 rot! -; -) 4)

(test=, 1 2 [3 4 5] len; 3)
(test=, (|1 2 [..3 4 5]) len; 5)
(test= (|3..; [..] len) 3)

(test=, (|3.. [..]) 3)

(42 let: foo)
(test=  @foo 42)
(test=, {@foo} call! 42)

(test=, 1 +<_ Int> 3; 4)

(test= (1 < 3) if: 5 7 5)
(test= (3 < 1) if: 5 7 7)

(test= (7 switch:, (drop! t) 'foo) 'foo)
(test= (35 switch:, (< 42) 'foo, drop! 'bar) 'foo)
(test= (35 switch:, (< 7) 'foo, drop! 'bar) 'bar)
(test= (35 switch:, (< 7) 'foo (< 42) 'bar, drop! 'baz) 'bar) 

(test=, 2 3 times: ++ 5)

func: double<T> (* 2)
(test=, 21 double; 42)

42 let: result
func: closure<> @result
(test=, closure; 42)

func: early<> (1 return! 3)
(test=, early; 1)

(test= (try: (drop! 7) 42 -) 35)
(test= (try: (catch; ++), throw 41) 42)
(test= (try: (catch; ++), try: throw, throw 41) 42)

(test= (3 iter; dup! call! swap! dup! call! swap! call! +; +) 3)