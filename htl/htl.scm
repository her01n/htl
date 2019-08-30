(define-module (htl htl))

(use-modules
  (srfi srfi-1)
  (system foreign))

(load-extension "htl" "init_htl")

(define (assert-type test value)
  (if (not (test value)) (throw 'wrong-type "Wrong type, expecting ~a: ~a" test value)))

(define-public (point x y z)
  (assert-type integer? x)
  (assert-type integer? y)
  (assert-type integer? z)
  (list 'point (+ 0 x) (+ 0 y) (+ 0 z)))

(define-public (point? p)
  (and
    (list? p)
    (equal? 4 (length p))
    (equal? 'point (first p))
    (every integer? (cdr p))))

(define-public (point-x point) (assert-type point? point) (second point))
(define-public (point-y point) (assert-type point? point) (third point))
(define-public (point-z point) (assert-type point? point) (fourth point))

(define-public (point-add p1 p2)
  (assert-type point? p1)
  (assert-type point? p2)
  (point (+ (point-x p1) (point-x p2)) (+ (point-y p1) (point-y p2)) (+ (point-z p1) (point-z p2))))

(define-public (face a b c)
  (assert-type point? a)
  (assert-type point? b)
  (assert-type point? c)
  (list a b c))

(define-public (face? f) 
  (and (list? f) (equal? 3 (length f)) (every point? f)))

(define-public (face-equal? f1 f2) 
  (assert-type face? f1)
  (assert-type face? f2)
  (htl-face-equal f1 f2))

(define-public (face-point-orientation f p) 
  (assert-type face? f)
  (assert-type point? p)
  (htl-face-point-orientation f p))

(define-public (surface . faces)
  (define face-list
    (or
      (and (equal? 1 (length faces)) (list? (car faces)) (every face? (car faces)) (car faces))
      (and (every face? faces) faces)
      (throw 'wrong-type "Wrong type, expecting faces or list of faces: ~a" faces)))
  (htl-surface-add-all (list 'surface %null-pointer) face-list))

(define-public (surface? s)
  (and
    (list? s)
    (equal? 2 (length s))
    (equal? 'surface (first s))
    (pointer? (second s))))

(define-public (surface-equal? s1 s2)
  (assert-type surface? s1)
  (assert-type surface? s2)
  (htl-surface-equal s1 s2))

(define-public (surface-faces s)
  (assert-type surface? s)
  (htl-surface-faces s))

(define-public (surface-add-face s f)
  (assert-type surface? s)
  (assert-type face? f)
  (htl-surface-add-all s (list f)))

(define-public (contains-face? s f)
  (assert-type surface? s)
  (assert-type face? f)
  (htl-surface-contains-face s f))

(define-public (lower-bound o)
  (or
    (and (face? o) (htl-face-lower-bound o))
    (and (surface? o) (htl-surface-lower-bound o))
    (throw 'wrong-type "Wrong type, expecting a face or a surface: ~a" o)))

(define-public (higher-bound o)
  (or
    (and (face? o) (htl-face-higher-bound o))
    (and (surface? o) (htl-surface-higher-bound o))
    (throw 'wrong-type "Wrong type, expecting a face or a surface: ~a" o)))

(define-public (faces-at s l h)
  (assert-type surface? s)
  (assert-type point? l)
  (assert-type point? h)
  (htl-surface-faces-at s l h))

