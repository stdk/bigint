#!/usr/bin/env python
# -*- coding: utf-8 -*-

from subprocess import check_output

executable = 'calc.exe'

def get_result(opcode,arg1,arg2):
    return check_output([executable,opcode,str(arg1),str(arg2)]).strip()

def test_operation(opcode,reference_func):
    for a in xrange(1,100):
        for b in xrange(1,100):
            reference = str(reference_func(a,b))
            value = get_result(opcode,a,b)

            if reference != value:
                print '%s %s %s: expected[%s] got[%s]' % (str(a),opcode,str(b),reference,value)
                return
            else:
                print '%s %s %s = %s' % (str(a),opcode,str(b),value)

    print 'Test for %s ok' % (opcode,)

if __name__ == '__main__':
    test_operation('plus',lambda a,b: a+b);
    test_operation('mul',lambda a,b: a*b);
    test_operation('div',lambda a,b: a/b);
