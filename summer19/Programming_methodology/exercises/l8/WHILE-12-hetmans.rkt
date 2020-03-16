#lang racket

(require "w8-while-interp.rkt")

(define 12-queens-in-WHILE
  (var-block
   'in-progress
   (const #t)
   (while
    (op '&& (variable 'in-progress) (op '< (variable '|1|) (const 12)))
    (comp
     (assign '|1| (op '+ (variable '|1|) (const 1)))
     (comp
      (assign '|2| (const 0))
      (if-stm
       (const #t)
       (while
        (op '&& (variable 'in-progress) (op '< (variable '|2|) (const 12)))
        (comp
         (assign '|2| (op '+ (variable '|2|) (const 1)))
         (comp
          (assign '|3| (const 0))
          (if-stm
           (op
            '&&
            (op '!= (variable '|2|) (variable '|1|))
            (op
             '&&
             (op
              '!=
              (if-expr
               (op '>= (op '- (variable '|2|) (variable '|1|)) (const 0))
               (op '- (variable '|2|) (variable '|1|))
               (op '- (const 0) (op '- (variable '|2|) (variable '|1|))))
              (const 1))
             (const #t)))
           (while
            (op '&& (variable 'in-progress) (op '< (variable '|3|) (const 12)))
            (comp
             (assign '|3| (op '+ (variable '|3|) (const 1)))
             (comp
              (assign '|4| (const 0))
              (if-stm
               (op
                '&&
                (op '!= (variable '|3|) (variable '|2|))
                (op
                 '&&
                 (op
                  '!=
                  (if-expr
                   (op '>= (op '- (variable '|3|) (variable '|2|)) (const 0))
                   (op '- (variable '|3|) (variable '|2|))
                   (op '- (const 0) (op '- (variable '|3|) (variable '|2|))))
                  (const 1))
                 (op
                  '&&
                  (op '!= (variable '|3|) (variable '|1|))
                  (op
                   '&&
                   (op
                    '!=
                    (if-expr
                     (op '>= (op '- (variable '|3|) (variable '|1|)) (const 0))
                     (op '- (variable '|3|) (variable '|1|))
                     (op '- (const 0) (op '- (variable '|3|) (variable '|1|))))
                    (const 2))
                   (const #t)))))
               (while
                (op '&& (variable 'in-progress) (op '< (variable '|4|) (const 12)))
                (comp
                 (assign '|4| (op '+ (variable '|4|) (const 1)))
                 (comp
                  (assign '|5| (const 0))
                  (if-stm
                   (op
                    '&&
                    (op '!= (variable '|4|) (variable '|3|))
                    (op
                     '&&
                     (op
                      '!=
                      (if-expr
                       (op '>= (op '- (variable '|4|) (variable '|3|)) (const 0))
                       (op '- (variable '|4|) (variable '|3|))
                       (op '- (const 0) (op '- (variable '|4|) (variable '|3|))))
                      (const 1))
                     (op
                      '&&
                      (op '!= (variable '|4|) (variable '|2|))
                      (op
                       '&&
                       (op
                        '!=
                        (if-expr
                         (op '>= (op '- (variable '|4|) (variable '|2|)) (const 0))
                         (op '- (variable '|4|) (variable '|2|))
                         (op '- (const 0) (op '- (variable '|4|) (variable '|2|))))
                        (const 2))
                       (op
                        '&&
                        (op '!= (variable '|4|) (variable '|1|))
                        (op
                         '&&
                         (op
                          '!=
                          (if-expr
                           (op '>= (op '- (variable '|4|) (variable '|1|)) (const 0))
                           (op '- (variable '|4|) (variable '|1|))
                           (op '- (const 0) (op '- (variable '|4|) (variable '|1|))))
                          (const 3))
                         (const #t)))))))
                   (while
                    (op '&& (variable 'in-progress) (op '< (variable '|5|) (const 12)))
                    (comp
                     (assign '|5| (op '+ (variable '|5|) (const 1)))
                     (comp
                      (assign '|6| (const 0))
                      (if-stm
                       (op
                        '&&
                        (op '!= (variable '|5|) (variable '|4|))
                        (op
                         '&&
                         (op
                          '!=
                          (if-expr
                           (op '>= (op '- (variable '|5|) (variable '|4|)) (const 0))
                           (op '- (variable '|5|) (variable '|4|))
                           (op '- (const 0) (op '- (variable '|5|) (variable '|4|))))
                          (const 1))
                         (op
                          '&&
                          (op '!= (variable '|5|) (variable '|3|))
                          (op
                           '&&
                           (op
                            '!=
                            (if-expr
                             (op '>= (op '- (variable '|5|) (variable '|3|)) (const 0))
                             (op '- (variable '|5|) (variable '|3|))
                             (op '- (const 0) (op '- (variable '|5|) (variable '|3|))))
                            (const 2))
                           (op
                            '&&
                            (op '!= (variable '|5|) (variable '|2|))
                            (op
                             '&&
                             (op
                              '!=
                              (if-expr
                               (op '>= (op '- (variable '|5|) (variable '|2|)) (const 0))
                               (op '- (variable '|5|) (variable '|2|))
                               (op '- (const 0) (op '- (variable '|5|) (variable '|2|))))
                              (const 3))
                             (op
                              '&&
                              (op '!= (variable '|5|) (variable '|1|))
                              (op
                               '&&
                               (op
                                '!=
                                (if-expr
                                 (op '>= (op '- (variable '|5|) (variable '|1|)) (const 0))
                                 (op '- (variable '|5|) (variable '|1|))
                                 (op '- (const 0) (op '- (variable '|5|) (variable '|1|))))
                                (const 4))
                               (const #t)))))))))
                       (while
                        (op '&& (variable 'in-progress) (op '< (variable '|6|) (const 12)))
                        (comp
                         (assign '|6| (op '+ (variable '|6|) (const 1)))
                         (comp
                          (assign '|7| (const 0))
                          (if-stm
                           (op
                            '&&
                            (op '!= (variable '|6|) (variable '|5|))
                            (op
                             '&&
                             (op
                              '!=
                              (if-expr
                               (op '>= (op '- (variable '|6|) (variable '|5|)) (const 0))
                               (op '- (variable '|6|) (variable '|5|))
                               (op '- (const 0) (op '- (variable '|6|) (variable '|5|))))
                              (const 1))
                             (op
                              '&&
                              (op '!= (variable '|6|) (variable '|4|))
                              (op
                               '&&
                               (op
                                '!=
                                (if-expr
                                 (op '>= (op '- (variable '|6|) (variable '|4|)) (const 0))
                                 (op '- (variable '|6|) (variable '|4|))
                                 (op '- (const 0) (op '- (variable '|6|) (variable '|4|))))
                                (const 2))
                               (op
                                '&&
                                (op '!= (variable '|6|) (variable '|3|))
                                (op
                                 '&&
                                 (op
                                  '!=
                                  (if-expr
                                   (op '>= (op '- (variable '|6|) (variable '|3|)) (const 0))
                                   (op '- (variable '|6|) (variable '|3|))
                                   (op '- (const 0) (op '- (variable '|6|) (variable '|3|))))
                                  (const 3))
                                 (op
                                  '&&
                                  (op '!= (variable '|6|) (variable '|2|))
                                  (op
                                   '&&
                                   (op
                                    '!=
                                    (if-expr
                                     (op '>= (op '- (variable '|6|) (variable '|2|)) (const 0))
                                     (op '- (variable '|6|) (variable '|2|))
                                     (op '- (const 0) (op '- (variable '|6|) (variable '|2|))))
                                    (const 4))
                                   (op
                                    '&&
                                    (op '!= (variable '|6|) (variable '|1|))
                                    (op
                                     '&&
                                     (op
                                      '!=
                                      (if-expr
                                       (op '>= (op '- (variable '|6|) (variable '|1|)) (const 0))
                                       (op '- (variable '|6|) (variable '|1|))
                                       (op '- (const 0) (op '- (variable '|6|) (variable '|1|))))
                                      (const 5))
                                     (const #t)))))))))))
                           (while
                            (op '&& (variable 'in-progress) (op '< (variable '|7|) (const 12)))
                            (comp
                             (assign '|7| (op '+ (variable '|7|) (const 1)))
                             (comp
                              (assign '|8| (const 0))
                              (if-stm
                               (op
                                '&&
                                (op '!= (variable '|7|) (variable '|6|))
                                (op
                                 '&&
                                 (op
                                  '!=
                                  (if-expr
                                   (op '>= (op '- (variable '|7|) (variable '|6|)) (const 0))
                                   (op '- (variable '|7|) (variable '|6|))
                                   (op '- (const 0) (op '- (variable '|7|) (variable '|6|))))
                                  (const 1))
                                 (op
                                  '&&
                                  (op '!= (variable '|7|) (variable '|5|))
                                  (op
                                   '&&
                                   (op
                                    '!=
                                    (if-expr
                                     (op '>= (op '- (variable '|7|) (variable '|5|)) (const 0))
                                     (op '- (variable '|7|) (variable '|5|))
                                     (op '- (const 0) (op '- (variable '|7|) (variable '|5|))))
                                    (const 2))
                                   (op
                                    '&&
                                    (op '!= (variable '|7|) (variable '|4|))
                                    (op
                                     '&&
                                     (op
                                      '!=
                                      (if-expr
                                       (op '>= (op '- (variable '|7|) (variable '|4|)) (const 0))
                                       (op '- (variable '|7|) (variable '|4|))
                                       (op '- (const 0) (op '- (variable '|7|) (variable '|4|))))
                                      (const 3))
                                     (op
                                      '&&
                                      (op '!= (variable '|7|) (variable '|3|))
                                      (op
                                       '&&
                                       (op
                                        '!=
                                        (if-expr
                                         (op '>= (op '- (variable '|7|) (variable '|3|)) (const 0))
                                         (op '- (variable '|7|) (variable '|3|))
                                         (op '- (const 0) (op '- (variable '|7|) (variable '|3|))))
                                        (const 4))
                                       (op
                                        '&&
                                        (op '!= (variable '|7|) (variable '|2|))
                                        (op
                                         '&&
                                         (op
                                          '!=
                                          (if-expr
                                           (op '>= (op '- (variable '|7|) (variable '|2|)) (const 0))
                                           (op '- (variable '|7|) (variable '|2|))
                                           (op '- (const 0) (op '- (variable '|7|) (variable '|2|))))
                                          (const 5))
                                         (op
                                          '&&
                                          (op '!= (variable '|7|) (variable '|1|))
                                          (op
                                           '&&
                                           (op
                                            '!=
                                            (if-expr
                                             (op '>= (op '- (variable '|7|) (variable '|1|)) (const 0))
                                             (op '- (variable '|7|) (variable '|1|))
                                             (op '- (const 0) (op '- (variable '|7|) (variable '|1|))))
                                            (const 6))
                                           (const #t)))))))))))))
                               (while
                                (op '&& (variable 'in-progress) (op '< (variable '|8|) (const 12)))
                                (comp
                                 (assign '|8| (op '+ (variable '|8|) (const 1)))
                                 (comp
                                  (assign '|9| (const 0))
                                  (if-stm
                                   (op
                                    '&&
                                    (op '!= (variable '|8|) (variable '|7|))
                                    (op
                                     '&&
                                     (op
                                      '!=
                                      (if-expr
                                       (op '>= (op '- (variable '|8|) (variable '|7|)) (const 0))
                                       (op '- (variable '|8|) (variable '|7|))
                                       (op '- (const 0) (op '- (variable '|8|) (variable '|7|))))
                                      (const 1))
                                     (op
                                      '&&
                                      (op '!= (variable '|8|) (variable '|6|))
                                      (op
                                       '&&
                                       (op
                                        '!=
                                        (if-expr
                                         (op '>= (op '- (variable '|8|) (variable '|6|)) (const 0))
                                         (op '- (variable '|8|) (variable '|6|))
                                         (op '- (const 0) (op '- (variable '|8|) (variable '|6|))))
                                        (const 2))
                                       (op
                                        '&&
                                        (op '!= (variable '|8|) (variable '|5|))
                                        (op
                                         '&&
                                         (op
                                          '!=
                                          (if-expr
                                           (op '>= (op '- (variable '|8|) (variable '|5|)) (const 0))
                                           (op '- (variable '|8|) (variable '|5|))
                                           (op '- (const 0) (op '- (variable '|8|) (variable '|5|))))
                                          (const 3))
                                         (op
                                          '&&
                                          (op '!= (variable '|8|) (variable '|4|))
                                          (op
                                           '&&
                                           (op
                                            '!=
                                            (if-expr
                                             (op '>= (op '- (variable '|8|) (variable '|4|)) (const 0))
                                             (op '- (variable '|8|) (variable '|4|))
                                             (op '- (const 0) (op '- (variable '|8|) (variable '|4|))))
                                            (const 4))
                                           (op
                                            '&&
                                            (op '!= (variable '|8|) (variable '|3|))
                                            (op
                                             '&&
                                             (op
                                              '!=
                                              (if-expr
                                               (op '>= (op '- (variable '|8|) (variable '|3|)) (const 0))
                                               (op '- (variable '|8|) (variable '|3|))
                                               (op '- (const 0) (op '- (variable '|8|) (variable '|3|))))
                                              (const 5))
                                             (op
                                              '&&
                                              (op '!= (variable '|8|) (variable '|2|))
                                              (op
                                               '&&
                                               (op
                                                '!=
                                                (if-expr
                                                 (op '>= (op '- (variable '|8|) (variable '|2|)) (const 0))
                                                 (op '- (variable '|8|) (variable '|2|))
                                                 (op '- (const 0) (op '- (variable '|8|) (variable '|2|))))
                                                (const 6))
                                               (op
                                                '&&
                                                (op '!= (variable '|8|) (variable '|1|))
                                                (op
                                                 '&&
                                                 (op
                                                  '!=
                                                  (if-expr
                                                   (op '>= (op '- (variable '|8|) (variable '|1|)) (const 0))
                                                   (op '- (variable '|8|) (variable '|1|))
                                                   (op '- (const 0) (op '- (variable '|8|) (variable '|1|))))
                                                  (const 7))
                                                 (const #t)))))))))))))))
                                   (while
                                    (op '&& (variable 'in-progress) (op '< (variable '|9|) (const 12)))
                                    (comp
                                     (assign '|9| (op '+ (variable '|9|) (const 1)))
                                     (comp
                                      (assign '|10| (const 0))
                                      (if-stm
                                       (op
                                        '&&
                                        (op '!= (variable '|9|) (variable '|8|))
                                        (op
                                         '&&
                                         (op
                                          '!=
                                          (if-expr
                                           (op '>= (op '- (variable '|9|) (variable '|8|)) (const 0))
                                           (op '- (variable '|9|) (variable '|8|))
                                           (op '- (const 0) (op '- (variable '|9|) (variable '|8|))))
                                          (const 1))
                                         (op
                                          '&&
                                          (op '!= (variable '|9|) (variable '|7|))
                                          (op
                                           '&&
                                           (op
                                            '!=
                                            (if-expr
                                             (op '>= (op '- (variable '|9|) (variable '|7|)) (const 0))
                                             (op '- (variable '|9|) (variable '|7|))
                                             (op '- (const 0) (op '- (variable '|9|) (variable '|7|))))
                                            (const 2))
                                           (op
                                            '&&
                                            (op '!= (variable '|9|) (variable '|6|))
                                            (op
                                             '&&
                                             (op
                                              '!=
                                              (if-expr
                                               (op '>= (op '- (variable '|9|) (variable '|6|)) (const 0))
                                               (op '- (variable '|9|) (variable '|6|))
                                               (op '- (const 0) (op '- (variable '|9|) (variable '|6|))))
                                              (const 3))
                                             (op
                                              '&&
                                              (op '!= (variable '|9|) (variable '|5|))
                                              (op
                                               '&&
                                               (op
                                                '!=
                                                (if-expr
                                                 (op '>= (op '- (variable '|9|) (variable '|5|)) (const 0))
                                                 (op '- (variable '|9|) (variable '|5|))
                                                 (op '- (const 0) (op '- (variable '|9|) (variable '|5|))))
                                                (const 4))
                                               (op
                                                '&&
                                                (op '!= (variable '|9|) (variable '|4|))
                                                (op
                                                 '&&
                                                 (op
                                                  '!=
                                                  (if-expr
                                                   (op '>= (op '- (variable '|9|) (variable '|4|)) (const 0))
                                                   (op '- (variable '|9|) (variable '|4|))
                                                   (op '- (const 0) (op '- (variable '|9|) (variable '|4|))))
                                                  (const 5))
                                                 (op
                                                  '&&
                                                  (op '!= (variable '|9|) (variable '|3|))
                                                  (op
                                                   '&&
                                                   (op
                                                    '!=
                                                    (if-expr
                                                     (op '>= (op '- (variable '|9|) (variable '|3|)) (const 0))
                                                     (op '- (variable '|9|) (variable '|3|))
                                                     (op '- (const 0) (op '- (variable '|9|) (variable '|3|))))
                                                    (const 6))
                                                   (op
                                                    '&&
                                                    (op '!= (variable '|9|) (variable '|2|))
                                                    (op
                                                     '&&
                                                     (op
                                                      '!=
                                                      (if-expr
                                                       (op '>= (op '- (variable '|9|) (variable '|2|)) (const 0))
                                                       (op '- (variable '|9|) (variable '|2|))
                                                       (op '- (const 0) (op '- (variable '|9|) (variable '|2|))))
                                                      (const 7))
                                                     (op
                                                      '&&
                                                      (op '!= (variable '|9|) (variable '|1|))
                                                      (op
                                                       '&&
                                                       (op
                                                        '!=
                                                        (if-expr
                                                         (op '>= (op '- (variable '|9|) (variable '|1|)) (const 0))
                                                         (op '- (variable '|9|) (variable '|1|))
                                                         (op '- (const 0) (op '- (variable '|9|) (variable '|1|))))
                                                        (const 8))
                                                       (const #t)))))))))))))))))
                                       (while
                                        (op '&& (variable 'in-progress) (op '< (variable '|10|) (const 12)))
                                        (comp
                                         (assign '|10| (op '+ (variable '|10|) (const 1)))
                                         (comp
                                          (assign '|11| (const 0))
                                          (if-stm
                                           (op
                                            '&&
                                            (op '!= (variable '|10|) (variable '|9|))
                                            (op
                                             '&&
                                             (op
                                              '!=
                                              (if-expr
                                               (op '>= (op '- (variable '|10|) (variable '|9|)) (const 0))
                                               (op '- (variable '|10|) (variable '|9|))
                                               (op '- (const 0) (op '- (variable '|10|) (variable '|9|))))
                                              (const 1))
                                             (op
                                              '&&
                                              (op '!= (variable '|10|) (variable '|8|))
                                              (op
                                               '&&
                                               (op
                                                '!=
                                                (if-expr
                                                 (op '>= (op '- (variable '|10|) (variable '|8|)) (const 0))
                                                 (op '- (variable '|10|) (variable '|8|))
                                                 (op '- (const 0) (op '- (variable '|10|) (variable '|8|))))
                                                (const 2))
                                               (op
                                                '&&
                                                (op '!= (variable '|10|) (variable '|7|))
                                                (op
                                                 '&&
                                                 (op
                                                  '!=
                                                  (if-expr
                                                   (op '>= (op '- (variable '|10|) (variable '|7|)) (const 0))
                                                   (op '- (variable '|10|) (variable '|7|))
                                                   (op '- (const 0) (op '- (variable '|10|) (variable '|7|))))
                                                  (const 3))
                                                 (op
                                                  '&&
                                                  (op '!= (variable '|10|) (variable '|6|))
                                                  (op
                                                   '&&
                                                   (op
                                                    '!=
                                                    (if-expr
                                                     (op '>= (op '- (variable '|10|) (variable '|6|)) (const 0))
                                                     (op '- (variable '|10|) (variable '|6|))
                                                     (op '- (const 0) (op '- (variable '|10|) (variable '|6|))))
                                                    (const 4))
                                                   (op
                                                    '&&
                                                    (op '!= (variable '|10|) (variable '|5|))
                                                    (op
                                                     '&&
                                                     (op
                                                      '!=
                                                      (if-expr
                                                       (op '>= (op '- (variable '|10|) (variable '|5|)) (const 0))
                                                       (op '- (variable '|10|) (variable '|5|))
                                                       (op '- (const 0) (op '- (variable '|10|) (variable '|5|))))
                                                      (const 5))
                                                     (op
                                                      '&&
                                                      (op '!= (variable '|10|) (variable '|4|))
                                                      (op
                                                       '&&
                                                       (op
                                                        '!=
                                                        (if-expr
                                                         (op '>= (op '- (variable '|10|) (variable '|4|)) (const 0))
                                                         (op '- (variable '|10|) (variable '|4|))
                                                         (op '- (const 0) (op '- (variable '|10|) (variable '|4|))))
                                                        (const 6))
                                                       (op
                                                        '&&
                                                        (op '!= (variable '|10|) (variable '|3|))
                                                        (op
                                                         '&&
                                                         (op
                                                          '!=
                                                          (if-expr
                                                           (op '>= (op '- (variable '|10|) (variable '|3|)) (const 0))
                                                           (op '- (variable '|10|) (variable '|3|))
                                                           (op '- (const 0) (op '- (variable '|10|) (variable '|3|))))
                                                          (const 7))
                                                         (op
                                                          '&&
                                                          (op '!= (variable '|10|) (variable '|2|))
                                                          (op
                                                           '&&
                                                           (op
                                                            '!=
                                                            (if-expr
                                                             (op '>= (op '- (variable '|10|) (variable '|2|)) (const 0))
                                                             (op '- (variable '|10|) (variable '|2|))
                                                             (op '- (const 0) (op '- (variable '|10|) (variable '|2|))))
                                                            (const 8))
                                                           (op
                                                            '&&
                                                            (op '!= (variable '|10|) (variable '|1|))
                                                            (op
                                                             '&&
                                                             (op
                                                              '!=
                                                              (if-expr
                                                               (op '>= (op '- (variable '|10|) (variable '|1|)) (const 0))
                                                               (op '- (variable '|10|) (variable '|1|))
                                                               (op '- (const 0) (op '- (variable '|10|) (variable '|1|))))
                                                              (const 9))
                                                             (const #t)))))))))))))))))))
                                           (while
                                            (op '&& (variable 'in-progress) (op '< (variable '|11|) (const 12)))
                                            (comp
                                             (assign '|11| (op '+ (variable '|11|) (const 1)))
                                             (comp
                                              (assign '|12| (const 0))
                                              (if-stm
                                               (op
                                                '&&
                                                (op '!= (variable '|11|) (variable '|10|))
                                                (op
                                                 '&&
                                                 (op
                                                  '!=
                                                  (if-expr
                                                   (op '>= (op '- (variable '|11|) (variable '|10|)) (const 0))
                                                   (op '- (variable '|11|) (variable '|10|))
                                                   (op '- (const 0) (op '- (variable '|11|) (variable '|10|))))
                                                  (const 1))
                                                 (op
                                                  '&&
                                                  (op '!= (variable '|11|) (variable '|9|))
                                                  (op
                                                   '&&
                                                   (op
                                                    '!=
                                                    (if-expr
                                                     (op '>= (op '- (variable '|11|) (variable '|9|)) (const 0))
                                                     (op '- (variable '|11|) (variable '|9|))
                                                     (op '- (const 0) (op '- (variable '|11|) (variable '|9|))))
                                                    (const 2))
                                                   (op
                                                    '&&
                                                    (op '!= (variable '|11|) (variable '|8|))
                                                    (op
                                                     '&&
                                                     (op
                                                      '!=
                                                      (if-expr
                                                       (op '>= (op '- (variable '|11|) (variable '|8|)) (const 0))
                                                       (op '- (variable '|11|) (variable '|8|))
                                                       (op '- (const 0) (op '- (variable '|11|) (variable '|8|))))
                                                      (const 3))
                                                     (op
                                                      '&&
                                                      (op '!= (variable '|11|) (variable '|7|))
                                                      (op
                                                       '&&
                                                       (op
                                                        '!=
                                                        (if-expr
                                                         (op '>= (op '- (variable '|11|) (variable '|7|)) (const 0))
                                                         (op '- (variable '|11|) (variable '|7|))
                                                         (op '- (const 0) (op '- (variable '|11|) (variable '|7|))))
                                                        (const 4))
                                                       (op
                                                        '&&
                                                        (op '!= (variable '|11|) (variable '|6|))
                                                        (op
                                                         '&&
                                                         (op
                                                          '!=
                                                          (if-expr
                                                           (op '>= (op '- (variable '|11|) (variable '|6|)) (const 0))
                                                           (op '- (variable '|11|) (variable '|6|))
                                                           (op '- (const 0) (op '- (variable '|11|) (variable '|6|))))
                                                          (const 5))
                                                         (op
                                                          '&&
                                                          (op '!= (variable '|11|) (variable '|5|))
                                                          (op
                                                           '&&
                                                           (op
                                                            '!=
                                                            (if-expr
                                                             (op '>= (op '- (variable '|11|) (variable '|5|)) (const 0))
                                                             (op '- (variable '|11|) (variable '|5|))
                                                             (op '- (const 0) (op '- (variable '|11|) (variable '|5|))))
                                                            (const 6))
                                                           (op
                                                            '&&
                                                            (op '!= (variable '|11|) (variable '|4|))
                                                            (op
                                                             '&&
                                                             (op
                                                              '!=
                                                              (if-expr
                                                               (op '>= (op '- (variable '|11|) (variable '|4|)) (const 0))
                                                               (op '- (variable '|11|) (variable '|4|))
                                                               (op '- (const 0) (op '- (variable '|11|) (variable '|4|))))
                                                              (const 7))
                                                             (op
                                                              '&&
                                                              (op '!= (variable '|11|) (variable '|3|))
                                                              (op
                                                               '&&
                                                               (op
                                                                '!=
                                                                (if-expr
                                                                 (op '>= (op '- (variable '|11|) (variable '|3|)) (const 0))
                                                                 (op '- (variable '|11|) (variable '|3|))
                                                                 (op '- (const 0) (op '- (variable '|11|) (variable '|3|))))
                                                                (const 8))
                                                               (op
                                                                '&&
                                                                (op '!= (variable '|11|) (variable '|2|))
                                                                (op
                                                                 '&&
                                                                 (op
                                                                  '!=
                                                                  (if-expr
                                                                   (op '>= (op '- (variable '|11|) (variable '|2|)) (const 0))
                                                                   (op '- (variable '|11|) (variable '|2|))
                                                                   (op '- (const 0) (op '- (variable '|11|) (variable '|2|))))
                                                                  (const 9))
                                                                 (op
                                                                  '&&
                                                                  (op '!= (variable '|11|) (variable '|1|))
                                                                  (op
                                                                   '&&
                                                                   (op
                                                                    '!=
                                                                    (if-expr
                                                                     (op '>= (op '- (variable '|11|) (variable '|1|)) (const 0))
                                                                     (op '- (variable '|11|) (variable '|1|))
                                                                     (op '- (const 0) (op '- (variable '|11|) (variable '|1|))))
                                                                    (const 10))
                                                                   (const #t)))))))))))))))))))))
                                               (while
                                                (op '&& (variable 'in-progress) (op '< (variable '|12|) (const 12)))
                                                (comp
                                                 (assign '|12| (op '+ (variable '|12|) (const 1)))
                                                 (comp
                                                  (skip)
                                                  (if-stm
                                                   (op
                                                    '&&
                                                    (op '!= (variable '|12|) (variable '|11|))
                                                    (op
                                                     '&&
                                                     (op
                                                      '!=
                                                      (if-expr
                                                       (op '>= (op '- (variable '|12|) (variable '|11|)) (const 0))
                                                       (op '- (variable '|12|) (variable '|11|))
                                                       (op '- (const 0) (op '- (variable '|12|) (variable '|11|))))
                                                      (const 1))
                                                     (op
                                                      '&&
                                                      (op '!= (variable '|12|) (variable '|10|))
                                                      (op
                                                       '&&
                                                       (op
                                                        '!=
                                                        (if-expr
                                                         (op '>= (op '- (variable '|12|) (variable '|10|)) (const 0))
                                                         (op '- (variable '|12|) (variable '|10|))
                                                         (op '- (const 0) (op '- (variable '|12|) (variable '|10|))))
                                                        (const 2))
                                                       (op
                                                        '&&
                                                        (op '!= (variable '|12|) (variable '|9|))
                                                        (op
                                                         '&&
                                                         (op
                                                          '!=
                                                          (if-expr
                                                           (op '>= (op '- (variable '|12|) (variable '|9|)) (const 0))
                                                           (op '- (variable '|12|) (variable '|9|))
                                                           (op '- (const 0) (op '- (variable '|12|) (variable '|9|))))
                                                          (const 3))
                                                         (op
                                                          '&&
                                                          (op '!= (variable '|12|) (variable '|8|))
                                                          (op
                                                           '&&
                                                           (op
                                                            '!=
                                                            (if-expr
                                                             (op '>= (op '- (variable '|12|) (variable '|8|)) (const 0))
                                                             (op '- (variable '|12|) (variable '|8|))
                                                             (op '- (const 0) (op '- (variable '|12|) (variable '|8|))))
                                                            (const 4))
                                                           (op
                                                            '&&
                                                            (op '!= (variable '|12|) (variable '|7|))
                                                            (op
                                                             '&&
                                                             (op
                                                              '!=
                                                              (if-expr
                                                               (op '>= (op '- (variable '|12|) (variable '|7|)) (const 0))
                                                               (op '- (variable '|12|) (variable '|7|))
                                                               (op '- (const 0) (op '- (variable '|12|) (variable '|7|))))
                                                              (const 5))
                                                             (op
                                                              '&&
                                                              (op '!= (variable '|12|) (variable '|6|))
                                                              (op
                                                               '&&
                                                               (op
                                                                '!=
                                                                (if-expr
                                                                 (op '>= (op '- (variable '|12|) (variable '|6|)) (const 0))
                                                                 (op '- (variable '|12|) (variable '|6|))
                                                                 (op '- (const 0) (op '- (variable '|12|) (variable '|6|))))
                                                                (const 6))
                                                               (op
                                                                '&&
                                                                (op '!= (variable '|12|) (variable '|5|))
                                                                (op
                                                                 '&&
                                                                 (op
                                                                  '!=
                                                                  (if-expr
                                                                   (op '>= (op '- (variable '|12|) (variable '|5|)) (const 0))
                                                                   (op '- (variable '|12|) (variable '|5|))
                                                                   (op '- (const 0) (op '- (variable '|12|) (variable '|5|))))
                                                                  (const 7))
                                                                 (op
                                                                  '&&
                                                                  (op '!= (variable '|12|) (variable '|4|))
                                                                  (op
                                                                   '&&
                                                                   (op
                                                                    '!=
                                                                    (if-expr
                                                                     (op '>= (op '- (variable '|12|) (variable '|4|)) (const 0))
                                                                     (op '- (variable '|12|) (variable '|4|))
                                                                     (op '- (const 0) (op '- (variable '|12|) (variable '|4|))))
                                                                    (const 8))
                                                                   (op
                                                                    '&&
                                                                    (op '!= (variable '|12|) (variable '|3|))
                                                                    (op
                                                                     '&&
                                                                     (op
                                                                      '!=
                                                                      (if-expr
                                                                       (op '>= (op '- (variable '|12|) (variable '|3|)) (const 0))
                                                                       (op '- (variable '|12|) (variable '|3|))
                                                                       (op '- (const 0) (op '- (variable '|12|) (variable '|3|))))
                                                                      (const 9))
                                                                     (op
                                                                      '&&
                                                                      (op '!= (variable '|12|) (variable '|2|))
                                                                      (op
                                                                       '&&
                                                                       (op
                                                                        '!=
                                                                        (if-expr
                                                                         (op '>= (op '- (variable '|12|) (variable '|2|)) (const 0))
                                                                         (op '- (variable '|12|) (variable '|2|))
                                                                         (op '- (const 0) (op '- (variable '|12|) (variable '|2|))))
                                                                        (const 10))
                                                                       (op
                                                                        '&&
                                                                        (op '!= (variable '|12|) (variable '|1|))
                                                                        (op
                                                                         '&&
                                                                         (op
                                                                          '!=
                                                                          (if-expr
                                                                           (op '>= (op '- (variable '|12|) (variable '|1|)) (const 0))
                                                                           (op '- (variable '|12|) (variable '|1|))
                                                                           (op '- (const 0) (op '- (variable '|12|) (variable '|1|))))
                                                                          (const 11))
                                                                         (const #t)))))))))))))))))))))))
                                                   (assign 'in-progress (const #f))
                                                   (skip)))))
                                               (skip)))))
                                           (skip)))))
                                       (skip)))))
                                   (skip)))))
                               (skip)))))
                           (skip)))))
                       (skip)))))
                   (skip)))))
               (skip)))))
           (skip)))))
       (skip)))))))

