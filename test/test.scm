(define-module (test htl))

(use-modules
  (hdt hdt)
  (htl htl)
  (srfi srfi-1))

(test point
  (test point
    (assert (throws-exception (point "1" 2 3))))
  (test equal?
    (assert (equal? (point 1 2 3) (point 1 2 3)))
    (assert (not (equal? (point 1 2 3) (point 1 2 4)))))
  (test point?
    (assert (point? (point 1 2 3)))
    (assert (not (point? (list 1 2 3))))
    (assert (not (point? "1 2 3"))))
  (test xyz
    (assert (equal? 1 (point-x (point 1 2 3))))
    (assert (equal? 2 (point-y (point 1 2 3))))
    (assert (equal? 3 (point-z (point 1 2 3))))
    (assert (throws-exception (point-x (list 1 2 3)))))
  (test add
    (assert (equal? (point 5 7 9) (point-add (point 1 2 3) (point 4 5 6))))
    (assert (throws-exception (point-add (point 1 2 3) "4 5 6")))))

(define a (point 1 2 3))
(define b (point 4 5 6))
(define c (point 7 8 9))
(define d (point 0 0 0))

(test face
  (test face
    (assert (equal? (face a b c) (list a b c))))
  (test face-equal?
    (assert (face-equal? (face a b c) (face a b c)))
    (assert (not (face-equal? (face a b c) (face a b d))))
    (assert (face-equal? (face a b c) (face b c a)))
    (assert (not (face-equal? (face a b c) (face a c b)))))
  (test face?
    (assert (face? (face a b c)))
    (assert (not (face? "a b c")))
    (assert (face? (list a b c))))
  (test orientation
    (define f (face (point 0 0 0) (point 2 0 0) (point 0 2 0)))
    (assert (> (face-point-orientation f (point 1 1 1)) 0))
    (assert (equal? (face-point-orientation f (point 1 1 0)) 0))
    (assert (< (face-point-orientation f (point 1 1 -1)) 0))
    (assert (throws-exception (face-point-orientation (list (list 1 2 3)) (point 0 0 0)))))
  (test bounds
    (assert (equal? (point 0 0 0) (lower-bound (face a b d))))
    (assert (equal? (point 4 5 6) (higher-bound (face a b d))))))

(test surface
  (define face1 (face a b c))
  (define face2 (face b c d))
  (define face3 (face a c d))
  (test surface
    (assert (surface? (surface face1 face2)))
    (assert (surface? (surface (list face1 face2))))
    (assert (throws-exception (surface a b c))))
  (test equal?
    (assert (surface-equal? (surface face1 face2) (surface face1 face2)))
    (assert (surface-equal? (surface face1 face2) (surface face2 face1)))
    (assert (not (surface-equal? (surface face1 face2) (surface face1 face3)))))
  (test faces
    (define faces (surface-faces (surface face1 face2)))
    (assert (equal? 2 (length faces)))
    (assert (member face1 faces face-equal?))
    (assert (member face2 faces face-equal?)))
  (test add-face
    (define s1 (surface face1))
    (define s2 (surface-add-face s1 face2))
    (define faces (surface-faces s2))
    (assert (member face1 faces face-equal?))
    (assert (member face2 faces face-equal?)))
  (test contains
    (assert (contains-face? (surface face1 face2) face1))
    (assert (not (contains-face? (surface face1 face2) face3))))
  (test bounds
    (assert (equal? d (lower-bound (surface face1 face2))))
    (assert (equal? c (higher-bound (surface face1 face2)))))
  (test faces-at
    (define h1 (face (point 0 0 0) (point 1 0 0) (point 0 1 0)))
    (define h2 (face (point 0 0 1) (point 1 0 1) (point 0 1 1)))
    (define v1 (face (point 0 0 0) (point 0 1 0) (point 0 0 1)))
    (define v2 (face (point 1 0 0) (point 1 1 0) (point 1 0 1)))
    (define s (surface h1 h2 v1 v2))
    (assert (lset= face-equal? (list h1 h2 v2) (faces-at s (point 1 0 0) (point 1 1 1))))
    (assert (equal? '() (faces-at s (point 2 0 0) (point 2 1 1))))
    (assert (lset= face-equal? (list h1 v1) (faces-at s (point 0 0 0) (point 0 0 0))))))
    

