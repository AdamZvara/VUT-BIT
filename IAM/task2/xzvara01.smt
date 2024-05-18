; run with
;
;   $ z3 -smt2 queens.smt

; declare the sorts of pos --- pos represents the chessboard (an 8x8 array) ,
; indexed by "(pos row column)":
; true  --- a queen is on [row, column]
; false --- a queen is not on [row, column]
(declare-fun pos (Int Int) Bool)

; a helper function that checks that 1 <= x <= 8
; is-in-range(x): (x > 0) and (x <= 8)
(define-fun is-in-range ((x Int)) Bool (and (> x 0) (<= x 8)))

; first, we say that in all rows, there is at least one queen
; for all i: if is-in-range(i) then exists j, which is-in-range(y) and queen is at position [i,j]
(assert (forall ((i Int)) (=> (is-in-range i) (exists ((j Int)) (and (is-in-range j) (pos i j)) ))))

; second, we say that if a queen is on [k; l], then there is no queen on any [m; l] (for m != k) and on any [k; m] (for m != l)
; for all k, l:
;   if k is in range:
;       if there is queen at position [k, l], then for all m:
;           if m is in range:
;               if m != k, then queen is not at position [m, l]
;               AND
;               if m != l, then queen is not at position [k, m]
(assert (forall ((k Int)) (forall ((l Int)) (=> (is-in-range k) (=> (pos k l) (forall ((m Int)) (=> (is-in-range m) (and (=> (not (= m k)) (not (pos m l))) (=> (not (= m l)) (not (pos k m))))))) ))))

; ADD YOUR CONSTRAINTS HERE
;============================= START ==============

; checking first diagonal, which goes left and up and right and down (for negative m)
; for all k, l:
;   if k is in range:
;       if there is queen at position [k, l], then for all m:
;           if k-m is in range and l-m is in range:
;               if m ! 0; there is no queen at position [k-m, l-m]

(assert (forall ((k Int)) (forall ((l Int)) (=> (is-in-range k) (=> (pos k l) (forall ((m Int)) (=> (and (is-in-range (- k m)) (is-in-range (- l m))) (=> (not (= m 0)) (not (pos (- k m) (- l m))) )) )) )) ))

; do the same thing, but for second diagonal
(assert (forall ((k Int)) (forall ((l Int)) (=> (is-in-range k) (=> (pos k l) (forall ((m Int)) (=> (and (is-in-range (- k m)) (is-in-range (+ l m))) (=> (not (= m 0)) (not (pos (- k m) (+ l m))) )) )) )) ))

;============================= END ================

(declare-const y-pos-a Int)
(declare-const y-pos-b Int)
(declare-const y-pos-c Int)
(declare-const y-pos-d Int)
(declare-const y-pos-e Int)
(declare-const y-pos-f Int)
(declare-const y-pos-g Int)
(declare-const y-pos-h Int)

(assert (and (is-in-range y-pos-a) (pos 1 y-pos-a)))
(assert (and (is-in-range y-pos-b) (pos 2 y-pos-b)))
(assert (and (is-in-range y-pos-c) (pos 3 y-pos-c)))
(assert (and (is-in-range y-pos-d) (pos 4 y-pos-d)))
(assert (and (is-in-range y-pos-e) (pos 5 y-pos-e)))
(assert (and (is-in-range y-pos-f) (pos 6 y-pos-f)))
(assert (and (is-in-range y-pos-g) (pos 7 y-pos-g)))
(assert (and (is-in-range y-pos-h) (pos 8 y-pos-h)))

(check-sat)
(get-model)