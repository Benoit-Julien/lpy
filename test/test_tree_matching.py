from openalea.lpy import *
import warnings

def matching_run(code,optionvalues = range(3)): 
    if type(optionvalues) == int:
        optionvalues = [optionvalues]
    for i in range(2):
        l = Lsystem()
        if i in optionvalues:
            l.set(code)
            l.context().options.setSelection('String matching',i)
            l.iterate()
        else:
            try:
                l.set(code)
                l.context().options.setSelection('String matching',i)
                l.iterate()
                print "Test do not fail for unsupported string matching mode : %i." % i
                warnings.warn("Test do not fail for unsupported string matching mode : %i." % i)
            except:
                pass

def test_axial_match() : 
    """ Test matching with axial tree context modification"""
    f = open('test_axial_matching.lpy')
    code = f.read()
    f.close()
    matching_run(code,[1,2])

def test_ms_match() : 
    """ Test matching with multiscale axial tree context modification"""
    f = open('test_msmatch.lpy')
    code = f.read()
    f.close()
    matching_run(code,2)

#def test_match_future() : matching_run('test_matching_future.lpy')

########################################################

if __name__ == '__main__':
    import traceback as tb
    test_func = [ (n,v) for n,v in globals().items() if 'test' in n]
    test_func.sort(lambda x,y : cmp(x[1].func_code.co_firstlineno,y[1].func_code.co_firstlineno))
    for tfn,tf in test_func:
        print tfn
        try:
            tf()
        except:
            tb.print_exc()
