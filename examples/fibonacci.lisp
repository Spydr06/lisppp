(defun fibonacci(n)
    (cond
        (eq n 1) 1
        (eq n 2) 1
        ((+ (fibonacci (- n 1)) (fibonacci (- n 2))))))
(print (fibonacci 25))
